// soft2scene.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <assert.h>
#include <iostream>
#include <fstream>
#include <strstream>
#include <sstream>
#include <iostream>
#include <Windows.h>
#include <vector>

#include <SAA.h>

#include "BinaryFile.h"
#include "Element.h"

//// Global Variables ////

// Global tables
static std::vector<Element *> elements;

// Global strings.
static char default_scene_name[] = "suitA-zero.1-0";
static char *scene_name = default_scene_name;
static char *model_name = nullptr;
static char default_database_name[] = "C:/Users/theclashingfritz/player/softimage";
static char *database_name = default_database_name;
static char default_rsrc_path[] = "D:/Softimage/SOFT3D_4.0/3D/rsrc";
static char *rsrc_path = default_rsrc_path;
static char *texture_list_filename = nullptr;

// Global SAA variables
static SAA_Database database;
static SAA_Scene scene;
static SAA_Boolean uv_swap = FALSE;

// Global toggles.
static bool make_poly = true;
static bool make_nurbs = true;
static bool make_duv = false;

// Other global variables. 
static FILE *log_file = nullptr;
static std::ofstream texture_list_file;
static int pose_frame = 0;
static int nurbs_step = 1;
static int num_tex_loc = 0;
static int num_tex_glb = 0;
static int verbose = 3;

// Constants
static const int TEX_PER_MAT = 1;

#ifdef _DEBUG
  #define dprintf(format, ...) printf(format, __VA_ARGS__);
#else
  #define dprintf(format, ...)
#endif

#define lprintf(format, verbose_level, ...) if (verbose >= verbose_level) { printf(format, __VA_ARGS__); }
#define lfprintf(file, format, verbose_level, ...) if (verbose >= verbose_level) { fprintf(file, format, __VA_ARGS__); }

Element *ProcessElement(SAA_Scene *scene, SAA_Elem *model, Element *last_element = nullptr, Element *last_joint = nullptr);


void safe_exit(int code) {
    // Close the log for debugging.
    if (log_file) {
        fflush(log_file);
        fclose(log_file);
        log_file = nullptr;
    }

    exit(code);
}

char *GetName(SAA_Scene *scene, SAA_Elem *element) {
    int name_len = 0;

    // Get the name
    SAA_elementGetNameLength(scene, element, &name_len);
    char* name = new char[++name_len];
    SAA_elementGetName(scene, element, name_len, name);
    name[--name_len] = 0;

    return name;
}

char *GetFullName(SAA_Scene *scene, SAA_Elem *element) {
    int prefix_len = 0;

    // Get the prefix
    SAA_elementGetPrefixLength(scene, element, &prefix_len);
    char* prefix = new char[++prefix_len];
    SAA_elementGetPrefix(scene, element, prefix_len, prefix);
    prefix[--prefix_len] = 0;

    int name_len = 0;

    // Get the name
    SAA_elementGetNameLength(scene, element, &name_len);
    char *name = new char[++name_len];
    SAA_elementGetName(scene, element, name_len, name);
    name[--name_len] = 0;

    // Construct the full name from both the prefix and name.
    size_t fullname_len = name_len + prefix_len + 1;
    char *fullname = new char[fullname_len + 1];
    strncpy_s(fullname, fullname_len, prefix, prefix_len);
    strncpy_s(fullname + prefix_len + 1, fullname_len - prefix_len, name, name_len);
    fullname[prefix_len] = '-';

    delete[] name;
    delete[] prefix;

    return fullname;
}

/**
  * Given a string, return a copy of the string without the leading file path.
  */
char *GetTextureName(SAA_Scene *scene, SAA_Elem *texture) {
    char *filename = NULL;
    int filename_len = 0;

    // Get the textures name.
    SAA_texture2DGetPicNameLength(scene, texture, &filename_len);

    if (filename_len) {
        filename = new char[++filename_len];
        memset(filename, 0, filename_len);
        SAA_texture2DGetPicName(scene, texture, filename_len, filename);
    }

    // make sure we are not being passed a NULL image, an empty image string or
    // the default image created by egg2soft
    if ((filename != nullptr) && strlen(filename) && (strstr(filename, "noIcon") == nullptr)) {
        return filename;
    } else {
        fprintf(log_file, "Warning: GetTextureName received NULL filename\n");
        return nullptr;
    }
}

int *MakeIndexMap(int *indices, int num_indices, int map_size) {
    int i, j;

    // Allocate map array
    int *map = new int[map_size];

    if (map != NULL) {
        for (i = 0; i < map_size; i++) {
            j = 0;
            int found = 0;
            while (j < num_indices) {
                if (indices[j] == i) {
                    map[i] = j;
                    lfprintf(log_file, "map[%d] = %d\n", 2, i, map[i]);
                    found = 1;
                    break;
                }
                j++;
            }
            if (!found) {
                lfprintf(log_file, "WARNING: Orphan vertex (%d)\n", 2, i);
                // default to -1 for now
                map[i] = -1;
            }
        }
    } else {
        fprintf(log_file, "Not enough memory for index map...\n");
    }

    return map;
}

void HandleElementChildren(SAA_Scene *scene, SAA_Elem *model, Element *new_element, Element *new_joint) {
    // Check for children.
    int num_children = 0;
    SAA_Elem *children = nullptr;

    SAA_modelGetNbChildren(scene, model, &num_children);
    lfprintf(log_file, "Children Amount: %d\n", 1, num_children);

    if (num_children) {
        children = new SAA_Elem[num_children];
        if (children == nullptr) {
            fprintf(log_file, "ERROR: Not enough memory for children.\n");
            fflush(log_file);
            return;
        }
        SAA_modelGetChildren(scene, model, num_children, children);
        if (children != nullptr) {
            new_element->prepare_children(num_children);
            for (int this_child = 0; this_child < num_children; this_child++) {
                lfprintf(log_file, "\nProcessing child %d...\n", 1, this_child);
                Element *child = ProcessElement(scene, &children[this_child], new_element, new_joint);
                assert(child != nullptr);
                new_element->set_child(this_child, child);
                dprintf("DEBUG: Storing processed element child %s with index %d!\n", child->get_name().c_str(), elements.size());
                elements.push_back(child);
            }
        }
        delete[] children;
    } else {
        lfprintf(log_file, "Don't descend this branch!\n", 1);
    }
}

/*
 * This is the default handler for any element that isn't either supported or is unknown.
 * It only saves information all Elements must have and nothing else. 
 * 
 * Don't use this unless you have to.
 */
Element *HandleStandardElement(SAA_Scene *scene, SAA_Elem *model, Element *last_element = nullptr, Element *last_joint = nullptr) {
    // Allocate and pre-initalize the Element.
    Element *new_element = new Element;
    memset(new_element, 0, sizeof(Element));

    // Make the name assignment a local affair.
    {
        // Get the name of the element.
        char *name = nullptr;
        if (true) { // use_prefix
            // Get the FULL name of the trim curve
            name = GetFullName(scene, model);
        } else {
            // Get the name of the trim curve
            name = GetName(scene, model);
        }

        new_element->set_name(name);

        // Free the copy of the name. We no longer need it.
        delete[] name;
    }

    // Get the assigned from the Element.
    const char *name = new_element->get_name().c_str();

    lfprintf(log_file, "Element name <%s>\n", 1, name);

    // If we have a previous element, It is our parent element!
    if (last_element != nullptr) { new_element->set_parent(last_element); }

    // Get the nills matrix.
    Matrix4f& matrix = new_element->get_transformation_matrix();
    SAA_modelGetMatrix(scene, model, SAA_COORDSYS_GLOBAL, matrix.mat);

    if (verbose >= 2) {
        fprintf(log_file, "Element Matrix = %f %f %f %f\n", matrix.mat[0][0], matrix.mat[0][1], matrix.mat[0][2], matrix.mat[0][3]);
        fprintf(log_file, "                 %f %f %f %f\n", matrix.mat[1][0], matrix.mat[1][1], matrix.mat[1][2], matrix.mat[1][3]);
        fprintf(log_file, "                 %f %f %f %f\n", matrix.mat[2][0], matrix.mat[2][1], matrix.mat[2][2], matrix.mat[2][3]);
        fprintf(log_file, "                 %f %f %f %f\n", matrix.mat[3][0], matrix.mat[3][1], matrix.mat[3][2], matrix.mat[3][3]);
    }

    // Get the position, rotation and scale indivdually.
    Vector3f& pos = new_element->get_position();
    Vector3f& rot = new_element->get_rotation();
    Vector3f& scale = new_element->get_scale();

    SAA_modelGetTranslation(scene, model, SAA_COORDSYS_LOCAL, &pos.x, &pos.y, &pos.z);
    SAA_modelGetRotation(scene, model, SAA_COORDSYS_LOCAL, &rot.x, &rot.y, &rot.z);
    SAA_modelGetScaling(scene, model, SAA_COORDSYS_LOCAL, &scale.x, &scale.y, &scale.z);

    if (verbose >= 3) {
        fprintf(log_file, "Element Pos = %f %f %f\n", pos.x, pos.y, pos.z);
        fprintf(log_file, "Element Rot = %f %f %f\n", rot.x, rot.y, rot.z);
        fprintf(log_file, "Element Scale = %f %f %f\n", scale.x, scale.y, scale.z);
    }

    SAA_Boolean visible = FALSE;
    SAA_modelGetNodeVisibility(scene, model, &visible);
    new_element->set_visibility(visible != FALSE);
    lfprintf(log_file, "Visibility: %d\n", 1, visible);

    HandleElementChildren(scene, model, new_element, last_joint);

    return new_element;

}

Element *HandleNill(SAA_Scene *scene, SAA_Elem *model, Element *last_element = nullptr, Element *last_joint = nullptr) {
    // Allocate and pre-initalize the Element.
    Element *new_element = new Element;
    memset(new_element, 0, sizeof(Element));

    // Make the name assignment a local affair.
    {
        // Get the name of the element.
        char* name = nullptr;
        if (true) { // use_prefix
            // Get the FULL name of the trim curve
            name = GetFullName(scene, model);
        } else {
            // Get the name of the trim curve
            name = GetName(scene, model);
        }

        new_element->set_name(name);

        // Free the copy of the name. We no longer need it.
        delete[] name;
    }

    // Get the assigned from the Element.
    const char *name = new_element->get_name().c_str();

    lfprintf(log_file, "Element name <%s>\n", 1, name);

    // If we have a previous element, It is our parent element!
    if (last_element != nullptr) { new_element->set_parent(last_element); }

    SAA_AlgorithmType type = SAA_ALG_STANDARD;
    SI_Error error = SAA_modelGetAlgorithm(scene, model, &type);
    if (error != SI_ERR_NONE) {
        lfprintf(log_file, "Error: Couldn't get algorithm type of nill!\n", 1);
        lfprintf(log_file, "\tBailing on nill: '%s'\n", 1, name);
        fflush(log_file);
        return new_element;
    }

    switch (type) {
        case SAA_ALG_STANDARD:
            lfprintf(log_file, "Nill <%s> is standard.\n", 2, name);
            break;
        case SAA_ALG_INV_KIN:
            lfprintf(log_file, "Nill <%s> has inverse kinetics.\n", 2, name);
            break;
        case SAA_ALG_DYNAMIC:
            lfprintf(log_file, "Nill <%s> is dynamic.\n", 2, name);
            break;
        case SAA_ALG_INV_KIN_LEAF:
            lfprintf(log_file, "Nill <%s> is a inverse kinetics leaf.\n", 2, name);
            break;
        case SAA_ALG_DYNA_LEAF:
            lfprintf(log_file, "Nill <%s> is a dynamic leaf.\n", 2, name);
            break;
        case SAA_ALG_GRAVITY:
            lfprintf(log_file, "Nill <%s> is gravity.\n", 2, name);
            break;
        case SAA_ALG_FORCE:
            lfprintf(log_file, "Nill <%s> is a force.\n", 2, name);
            break;
        case SAA_ALG_WIND:
            lfprintf(log_file, "Nill <%s> has wind.\n", 2, name);
            break;
        case SAA_ALG_DEF_GRAVITY:
            lfprintf(log_file, "Nill <%s> has definied gravity.\n", 2, name);
            break;
        case SAA_ALG_FAN:
            lfprintf(log_file, "Nill <%s> is a fan.\n", 2, name);
            break;
        case SAA_ALG_NAIL:
            lfprintf(log_file, "Nill <%s> is a nail.\n", 2, name);
            break;
        case SAA_ALG_DYN_MODEL:
            lfprintf(log_file, "Nill <%s> is a dynamic model.\n", 2, name);
            break;
        case SAA_ALG_CUSTOM_ICON:
            lfprintf(log_file, "Nill <%s> is a custom icon.\n", 2, name);
            break;
        case SAA_ALG_INSTANCE:
            lfprintf(log_file, "Nill <%s> is a instance.\n", 2, name);
            break;
        default:
            lfprintf(log_file, "Nill <%s> is a unknown algorithm type: %d, Skipping!\n", 2, name, type);
            fflush(log_file);
            return new_element;
    }

    // Get the nills matrix.
    Matrix4f &matrix = new_element->get_transformation_matrix();
    SAA_modelGetMatrix(scene, model, SAA_COORDSYS_GLOBAL, matrix.mat);

    if (verbose >= 2) {
        fprintf(log_file, "Nill Matrix = %f %f %f %f\n", matrix.mat[0][0], matrix.mat[0][1], matrix.mat[0][2], matrix.mat[0][3]);
        fprintf(log_file, "              %f %f %f %f\n", matrix.mat[1][0], matrix.mat[1][1], matrix.mat[1][2], matrix.mat[1][3]);
        fprintf(log_file, "              %f %f %f %f\n", matrix.mat[2][0], matrix.mat[2][1], matrix.mat[2][2], matrix.mat[2][3]);
        fprintf(log_file, "              %f %f %f %f\n", matrix.mat[3][0], matrix.mat[3][1], matrix.mat[3][2], matrix.mat[3][3]);
    }

    // Get the position, rotation and scale indivdually.
    Vector3f &pos = new_element->get_position();
    Vector3f &rot = new_element->get_rotation();
    Vector3f &scale = new_element->get_scale();

    SAA_modelGetTranslation(scene, model, SAA_COORDSYS_LOCAL, &pos.x, &pos.y, &pos.z);
    SAA_modelGetRotation(scene, model, SAA_COORDSYS_LOCAL, &rot.x, &rot.y, &rot.z);
    SAA_modelGetScaling(scene, model, SAA_COORDSYS_LOCAL, &scale.x, &scale.y, &scale.z);

    if (verbose >= 3) {
        fprintf(log_file, "Nill Pos = %f %f %f\n", pos.x, pos.y, pos.z);
        fprintf(log_file, "Nill Rot = %f %f %f\n", rot.x, rot.y, rot.z);
        fprintf(log_file, "Nill Scale = %f %f %f\n", scale.x, scale.y, scale.z);
    }

    SAA_Boolean visible = FALSE;
    SAA_modelGetNodeVisibility(scene, model, &visible);
    new_element->set_visibility(visible != FALSE);
    lfprintf(log_file, "Visibility: %d\n", 1, visible);

    HandleElementChildren(scene, model, new_element, last_joint);

    return new_element;
}

Element *HandleMesh(SAA_Scene* scene, SAA_Elem* model, Element* last_element = nullptr, Element* last_joint = nullptr) {
    SI_Error error;

    // Allocate and initalize the Element.
    Element *new_element = new Element;
    memset(new_element, 0, sizeof(Element));

    // Allocate and initalize the Mesh Info for the Element.
    new_element->mesh_info = new MeshInfo;
    memset(new_element->mesh_info, 0, sizeof(MeshInfo));
    // Set the flag for having mesh info.
    uint64_t flags = new_element->get_flags();
    new_element->set_flags(flags |= (uint64_t)Element::InfoTypes::Mesh);

    // Make the name assignment a local affair.
    {
        // Get the name of the element.
        char* name = nullptr;
        if (true) { // use_prefix
            // Get the FULL name of the trim curve
            name = GetFullName(scene, model);
        } else {
            // Get the name of the trim curve
            name = GetName(scene, model);
        }

        new_element->set_name(name);

        // Free the copy of the name. We no longer need it.
        delete[] name;
    }

    // Get the assigned from the Element.
    const char* name = new_element->get_name().c_str();

    // Get the models matrix.
    Matrix4f& matrix = new_element->get_transformation_matrix();
    SAA_modelGetMatrix(scene, model, SAA_COORDSYS_GLOBAL, matrix.mat);

    if (verbose >= 2) {
        fprintf(log_file, "Model Matrix = %f %f %f %f\n", matrix.mat[0][0], matrix.mat[0][1], matrix.mat[0][2], matrix.mat[0][3]);
        fprintf(log_file, "               %f %f %f %f\n", matrix.mat[1][0], matrix.mat[1][1], matrix.mat[1][2], matrix.mat[1][3]);
        fprintf(log_file, "               %f %f %f %f\n", matrix.mat[2][0], matrix.mat[2][1], matrix.mat[2][2], matrix.mat[2][3]);
        fprintf(log_file, "               %f %f %f %f\n", matrix.mat[3][0], matrix.mat[3][1], matrix.mat[3][2], matrix.mat[3][3]);
    }

    // Get the position, rotation and scale indivdually.
    Vector3f& pos = new_element->get_position();
    Vector3f& rot = new_element->get_rotation();
    Vector3f& scale = new_element->get_scale();

    SAA_modelGetTranslation(scene, model, SAA_COORDSYS_LOCAL, &pos.x, &pos.y, &pos.z);
    SAA_modelGetRotation(scene, model, SAA_COORDSYS_LOCAL, &rot.x, &rot.y, &rot.z);
    SAA_modelGetScaling(scene, model, SAA_COORDSYS_LOCAL, &scale.x, &scale.y, &scale.z);

    if (verbose >= 3) {
        fprintf(log_file, "Model Pos = %f %f %f\n", pos.x, pos.y, pos.z);
        fprintf(log_file, "Model Rot = %f %f %f\n", rot.x, rot.y, rot.z);
        fprintf(log_file, "Model Scale = %f %f %f\n", scale.x, scale.y, scale.z);
    }

    SAA_Boolean visible = FALSE;
    SAA_modelGetNodeVisibility(scene, model, &visible);
    new_element->set_visibility(visible != FALSE);
    lfprintf(log_file, "Visibility: %d\n", 1, visible);

    // Only create egg polygon data if: the node is visible, and we're outputing polys.
    if (!visible || !make_poly) {
        lfprintf(log_file, "Skipping creating polygon data.\n", 1);
        fflush(log_file);
        return new_element;
    }

    SAA_GeomType gtype = SAA_GEOM_ORIGINAL;

    // If making a pose - Get deformed geometry.
    if (false) { gtype = SAA_GEOM_DEFORMED; } // make_pose

    // Get the number of key shapes
    int num_shapes = 0;
    SAA_modelGetNbShapes(scene, model, &num_shapes);
    lfprintf(log_file, "Amount of shapes: %d\n", 1, num_shapes);

    // TODO: Process all of our shapes. 
    //for (int i = 0; i < num_shapes; i++) {
    //}

    // Get the number of triangles
    int num_tri = 0;
    int id = 0;
    error = SAA_modelGetNbTriangles(scene, model, gtype, id, &num_tri);
    if (error != SI_ERR_NONE) {
        if (verbose >= 1) {
            fprintf(log_file, "Error: Couldn't get number of triangles!\n");
            fprintf(log_file, "\tBailing on model: '%s'\n", name);
            fflush(log_file);
        }
        return new_element;
    } else if (verbose >= 1) {
        fprintf(log_file, "Model Triangle Count: %d\n", num_tri);
    }

    new_element->mesh_info->set_triangle_count(num_tri);

    // Check also to see if the surface is a skeleton.
    SAA_Boolean is_skeleton = FALSE;
    SAA_modelIsSkeleton(scene, model, &is_skeleton);

    // check to see if this surface is used as a skeleton or is animated via
    // constraint only ( these nodes are tagged by the animator with the
    // keyword "joint" somewhere in the nodes name)
    if (is_skeleton || (strstr(name, "joint") != NULL)) {
        if (verbose >= 1) {
            fprintf(log_file, "Animating polys as joint!!!\n");
        }

        //MakeJoint(scene, lastJoint, lastAnim, model, name);
    }

    // Model is not a null and has no triangles!
    if (!num_tri && verbose >= 1) {
        fprintf(log_file, "No triangles!\n");
    }

    // Allocate array of triangles and copy them all into it.
    SAA_SubElem* triangles = new SAA_SubElem[num_tri];
    if (triangles != nullptr) {
        // Triangulate model and read the triangles into array.
        SAA_modelGetTriangles(scene, model, gtype, id, num_tri, triangles);
    } else {
        fprintf(log_file, "Not enough memory for triangles! Aborting!\n");
        safe_exit(1);
    }

    // Allocate array of materials for each triangle and copy them all into it.
    SAA_Elem* materials = new SAA_Elem[num_tri];
    if (materials != nullptr) {
        // Read each triangle's material into array.
        SAA_triangleGetMaterials(scene, model, num_tri, triangles, materials);
    } else {
        fprintf(log_file, "Not enough memory for materials! Aborting!\n");
        safe_exit(1);
    }

    // Allocate array of textures per triangle.
    int* num_tex_tri = new int[num_tri];

    // Find out how many local textures per triangle.
    void* rel_info = nullptr;
    for (int i = 0; i < num_tri; i++) {
        error = SAA_materialRelationGetT2DLocNbElements(scene, &materials[i], FALSE, (const void**)&rel_info, &num_tex_tri[i]);
        // polytex
        if (error == SI_ERR_NONE) {
            num_tex_loc += num_tex_tri[i];
        }
    }

    // Get local textures if present
    SAA_Elem* textures = nullptr;
    if (num_tex_loc) {
        // ASSUME only one texture per material
        textures = new SAA_Elem[num_tri];

        for (int i = 0; i < num_tri; i++) {
            // and read all referenced local textures into array
            SAA_materialRelationGetT2DLocElements(scene, &materials[i], TEX_PER_MAT, &textures[i]);
        }

        if (verbose >= 1) {
            fprintf(log_file, "num_tex_loc = %d\n", num_tex_loc);
        }
    }
    // If no local textures, Try to get global textures.
    else {
        SAA_modelRelationGetT2DGlbNbElements(scene, model, FALSE, (const void**)&rel_info, &num_tex_glb);

        if (num_tex_glb) {
            // ASSUME only one texture per model
            textures = new SAA_Elem[1];

            // get the referenced texture
            SAA_modelRelationGetT2DGlbElements(scene, model, TEX_PER_MAT, textures);

            if (verbose >= 1) {
                fprintf(log_file, "num_tex_glb = %d\n", num_tex_glb);
            }
        }
    }

    // Allocate array of control vertices
    SAA_SubElem* cvertices = new SAA_SubElem[num_tri * 3];
    new_element->mesh_info->prepare_control_vertices(num_tri * 3);
    Vector4d* cvert_pos = new_element->mesh_info->get_control_vertices();
    if (cvertices != nullptr) {
        // Read each triangles control vertices into the array.
        SAA_triangleGetCtrlVertices(scene, model, gtype, id, num_tri, triangles, cvertices);
        SAA_ctrlVertexGetPositions(scene, model, num_tri * 3, cvertices, (SAA_DVector*)cvert_pos);

        if (verbose >= 2) {
            for (int i = 0; i < num_tri * 3; i++) {
                fprintf(log_file, "cvert[%d] = %f %f %f %f\n", i, cvert_pos[i].x, cvert_pos[i].y, cvert_pos[i].z, cvert_pos[i].w);
            }
        }
    } else {
        fprintf(log_file, "Not enough memory for control vertices! Aborting!\n");
        safe_exit(1);
    }

    // Allocate array of control vertex indices, This array maps from the
    // redundant cvertices array into the unique vertices array
    // (cvertices->vertices)
    new_element->mesh_info->prepare_indicies(num_tri * 3);
    int* indices = new_element->mesh_info->get_indicies();
    if (indices != nullptr) {
        SAA_ctrlVertexGetIndices(scene, model, num_tri * 3, cvertices, indices);

        if (verbose >= 2) {
            for (int i = 0; i < num_tri * 3; i++) {
                fprintf(log_file, "indices[%d] = %d\n", i, indices[i]);
            }
        }
    } else {
        fprintf(log_file, "Not enough memory for indices! Aborting!\n");
        safe_exit(1);
    }

    // Get the number of UNIQUE vertices in model.
    int num_vert = 0;
    SAA_modelGetNbTriVertices(scene, model, &num_vert);

    lfprintf(log_file, "Amount of unique verts = %d\n", 2, num_vert);

    // Allocate array of vertices.
    new_element->mesh_info->prepare_vertices(num_vert);
    Vector4d* vertices = new_element->mesh_info->get_vertices();

    // get the UNIQUE vertices of all triangles in model
    SAA_modelGetTriVertices(scene, model, num_vert, (SAA_DVector*)vertices);

    if (verbose >= 2) {
        for (int i = 0; i < num_vert; i++) {
            fprintf(log_file, "vertices[%d] = %f ", i, vertices[i].x);
            fprintf(log_file, "%f %f %f\n", vertices[i].y, vertices[i].z, vertices[i].w);
        }
    }

    // Allocate a index map array, We contruct this array to map from the
    // unique vertices array to the redundant cvertices array - it will
    // save us from doing repetitive searches later.
    int* index_map = MakeIndexMap(indices, num_tri * 3, num_vert);

    // Allocate array of normals
    new_element->mesh_info->prepare_normals(num_tri * 3);
    Vector4d* normals = new_element->mesh_info->get_normals();
    if (normals != NULL) {
        // read each control vertex's normals into an array
        SAA_ctrlVertexGetNormals(scene, model, num_tri * 3, cvertices, (SAA_DVector*)normals);
    } else {
        fprintf(log_file, "Not enough memory for normals! Aborting!\n");
        safe_exit(1);
    }

    if (verbose >= 2) {
        for (int i = 0; i < num_tri * 3; i++) {
            fprintf(log_file, "normals[%d] = %f %f %f %f\n", i, normals[i].x, normals[i].y, normals[i].z, normals[i].w);
        }
    }

    // Make sure we have textures before we get texture coordinates.
    if (num_tex_loc) {
        dprintf("DEBUG: Processing local textures (%d) for Element <%s>.\n", num_tex_loc, new_element->get_name().c_str());
        // Allocate arrays for u & v coords and texture info
        new_element->mesh_info->prepare_uvs_and_textures(num_tex_loc, num_tri);

        float *u_coords = new_element->mesh_info->get_u_coords();
        float *v_coords = new_element->mesh_info->get_v_coords();

        // read the u & v coords into the arrays
        if (u_coords != NULL && v_coords != NULL) {
            for (int i = 0; i < num_tri * num_tex_loc * 3; i++) {
                u_coords[i] = 0.0f;
                v_coords[i] = 0.0f;
            }

            SAA_ctrlVertexGetUVTxtCoords(scene, model, num_tri * 3, cvertices, num_tex_loc * 3, u_coords, v_coords);
        } else {
            fprintf(log_file, "Not enough memory for texture coords! Aborting!\n");
            safe_exit(1);
        }

        if (verbose >= 2) {
            for (int i = 0; i < num_tex_loc * 3; i++) {
                fprintf(log_file, "texcoords[%d] = ( %f , %f )\n", i, u_coords[i], v_coords[i]);
            }
        }

        // Get arays of texture info.
        int32_t &u_repeat = new_element->mesh_info->get_u_repeat();
        int32_t &v_repeat = new_element->mesh_info->get_v_repeat();
        float *u_scale = new_element->mesh_info->get_u_scale();
        float *v_scale = new_element->mesh_info->get_v_scale();
        float *u_offset = new_element->mesh_info->get_u_offset();
        float *v_offset = new_element->mesh_info->get_v_offset();
        char **texture_names = new_element->mesh_info->get_texture_names();

        for (int i = 0; i < num_tri; i++) {
            SAA_Boolean valid = FALSE;
            // check to see if texture is present
            error = SAA_elementIsValid(scene, &textures[i], &valid);
            if (error != SI_SUCCESS) {
                fprintf(log_file, "SAA_elementIsValid failed!!!!\n");
            }

            // texture present - Get the name and uv info.
            if (valid) {
                new_element->mesh_info->set_texture_name(i, GetTextureName(scene, &textures[i]));

                lfprintf(log_file, " tritex[%d] named: %s\n", 2, i, texture_names[i]);

                SAA_texture2DGetUVSwap(scene, &textures[i], &uv_swap);

                if (verbose >= 2 && uv_swap == TRUE) {
                    fprintf(log_file, " swapping u and v...\n");
                }

                SAA_texture2DGetUScale(scene, &textures[i], &u_scale[i]);
                SAA_texture2DGetVScale(scene, &textures[i], &v_scale[i]);
                SAA_texture2DGetUOffset(scene, &textures[i], &u_offset[i]);
                SAA_texture2DGetVOffset(scene, &textures[i], &v_offset[i]);

                if (verbose >= 2) {
                    fprintf(log_file, "tritex[%d] uScale: %f vScale: %f\n", i, u_scale[i], v_scale[i]);
                    fprintf(log_file, " uOffset: %f vOffset: %f\n", u_offset[i], v_offset[i]);
                }

                SAA_texture2DGetRepeats(scene, &textures[i], &u_repeat, &v_repeat);

                lfprintf(log_file, "uRepeat = %d, vRepeat = %d\n", 2, u_repeat, v_repeat);
            } else {
                if (verbose >= 2) {
                    fprintf(log_file, "Invalid texture...\n");
                    fprintf(log_file, " tritex[%d] named: (null)\n", i);
                }
            }
        }
    } else if (num_tex_glb) {
        dprintf("DEBUG: Processing global textures (%d) for Element <%s>.\n", num_tex_glb, new_element->get_name().c_str());

        SAA_Boolean valid;

        // Check to see if texture is present
        SAA_elementIsValid(scene, textures, &valid);

        // texture present - get the name and uv info
        if (valid) {
            SAA_texture2DGetUVSwap(scene, textures, &uv_swap);

            if (verbose >= 1 && uv_swap == TRUE) {
                fprintf(log_file, " swapping u and v...\n");
            }

            // Allocate arrays for u & v coords and texture info
            new_element->mesh_info->prepare_uvs_and_textures(num_tex_glb, num_tri);

            float *u_coords = new_element->mesh_info->get_u_coords();
            float *v_coords = new_element->mesh_info->get_v_coords();

            for (int i = 0; i < num_tri * num_tex_glb * 3; i++) {
                u_coords[i] = 0.0f;
                v_coords[i] = 0.0f;
            }

            // read the u & v coords into the arrays
            if (u_coords != NULL && v_coords != NULL) {
                SAA_triCtrlVertexGetGlobalUVTxtCoords(scene, model, num_tri * 3, cvertices, num_tex_glb, textures, u_coords, v_coords);
            } else {
                fprintf(log_file, "Not enough memory for texture coords! Aborting!\n");
                safe_exit(1);
            }

            if (verbose >= 2) {
                for (int i = 0; i < num_tri * num_tex_glb * 3; i++) {
                    fprintf(log_file, "texcoords[%d] = ( %f , %f )\n", i, u_coords[i], v_coords[i]);
                }
            }

            char **texture_names = new_element->mesh_info->get_texture_names();
            new_element->mesh_info->set_texture_name(0, GetTextureName(scene, textures));

            lfprintf(log_file, " global tex named: %s\n", 1, texture_names[0]);

            // Get arays of texture info.
            int32_t& u_repeat = new_element->mesh_info->get_u_repeat();
            int32_t& v_repeat = new_element->mesh_info->get_v_repeat();
            float* u_scale = new_element->mesh_info->get_u_scale();
            float* v_scale = new_element->mesh_info->get_v_scale();
            float* u_offset = new_element->mesh_info->get_u_offset();
            float* v_offset = new_element->mesh_info->get_v_offset();

            SAA_texture2DGetUScale(scene, textures, u_scale);
            SAA_texture2DGetVScale(scene, textures, v_scale);
            SAA_texture2DGetUOffset(scene, textures, u_offset);
            SAA_texture2DGetVOffset(scene, textures, v_offset);

            if (verbose >= 1) {
                fprintf(log_file, " global tex uScale: %f vScale: %f\n", *u_scale, *v_scale);
                fprintf(log_file, " uOffset: %f vOffset: %f\n", *u_offset, *v_offset);
            }

            SAA_texture2DGetRepeats(scene, textures, &u_repeat, &v_repeat);

            if (verbose >= 2) {
                fprintf(log_file, "uRepeat = %d, vRepeat = %d\n", u_repeat, v_repeat);
            }
        } else {
            fprintf(log_file, "Invalid texture...\n");
        }
    }

    // We're done - Trash triangles.
    SAA_modelClearTriangles(scene, model);

    HandleElementChildren(scene, model, new_element, last_joint);

    delete[] triangles;
    delete[] materials;
    delete[] num_tex_tri;
    if (textures) { delete[] textures; }
    delete[] cvertices;

    return new_element;
}

Element *ProcessElement(SAA_Scene *scene, SAA_Elem *model, Element *last_element, Element *last_joint) {
    // Find out what type of node we're dealing with.
    SAA_ModelType type = SAA_MNILL;
    SI_Error error = SAA_modelGetType(scene, model, &type);

    Element *element = nullptr;
    switch (type) {
        case SAA_MNILL:
            element = HandleNill(scene, model, last_element, last_joint);
            lfprintf(log_file, "Element <%s> is a nill.\n", 2, element->get_name().c_str());
            break;;
        case SAA_MPTCH:
            element = HandleStandardElement(scene, model, last_element, last_joint);
            lfprintf(log_file, "Element <%s> is a patch.\n", 2, element->get_name().c_str());
            break;
        case SAA_MFACE:
            element = HandleMesh(scene, model, last_element, last_joint);
            lfprintf(log_file, "Element <%s> is a face.\n", 2, element->get_name().c_str());
            break;
        case SAA_MSMSH:
            element = HandleMesh(scene, model, last_element, last_joint);
            lfprintf(log_file, "Element <%s> is a mesh.\n", 2, element->get_name().c_str());
            break;
        case SAA_MJNT:
            element = HandleStandardElement(scene, model, last_element, last_joint);
            lfprintf(log_file, "Element <%s> is a joint.\n", 2, element->get_name().c_str());
            break;
        case SAA_MSPLN:
            element = HandleStandardElement(scene, model, last_element, last_joint);
            lfprintf(log_file, "Element <%s> is a spline.\n", 2, element->get_name().c_str());
            break;
        case SAA_MMETA:
            element = HandleStandardElement(scene, model, last_element, last_joint);
            lfprintf(log_file, "Element <%s> is a meta element.\n", 2, element->get_name().c_str());
            break;
        case SAA_MBALL:
            element = HandleStandardElement(scene, model, last_element, last_joint);
            lfprintf(log_file, "Element <%s> is a metaball.\n", 2, element->get_name().c_str());
            break;
        case SAA_MNCRV:
            element = HandleStandardElement(scene, model, last_element, last_joint);
            lfprintf(log_file, "Element <%s> is a nurb curve.\n", 2, element->get_name().c_str());
            break;
        case SAA_MNSRF:
            element = HandleStandardElement(scene, model, last_element, last_joint);
            lfprintf(log_file, "Element <%s> is a nurbs surf.\n", 2, element->get_name().c_str());
            break;
        default:
            element = HandleStandardElement(scene, model, last_element, last_joint);
            lfprintf(log_file, "Element <%s> is a unknown type: %d, Using Standard Processing!\n", 2, element->get_name().c_str(), type);
            break;
    }

    // Flush the log,
    fflush(log_file);

    return element;
}

void IndentStream(std::stringstream &ss, int indent_level) {
    for (int indent = 0; indent < indent_level; indent++) {
        ss << ' ';
    }
}

void ListHierarchy(SAA_Scene *scene, SAA_Elem *model, int indent_level) {
    std::stringstream ss;
    IndentStream(ss, indent_level);

    // Make the name assignment a local affair.
    std::string name;
    {
        // Get the name of the element.
        char *name_buff = nullptr;
        if (true) { // use_prefix
            // Get the FULL name of the trim curve
            name_buff = GetFullName(scene, model);
        } else {
            // Get the name of the trim curve
            name_buff = GetName(scene, model);
        }

        name = std::string(name_buff);

        // Free the copy of the name. We no longer need it.
        delete[] name_buff;
    }

    ss << name << ' ';

    // Find out what type of node we're dealing with.
    SAA_ModelType type = SAA_MNILL;
    SAA_AlgorithmType alg_type = SAA_ALG_STANDARD;
    SI_Error error = SAA_modelGetType(scene, model, &type);

    switch (type) {
        case SAA_MNILL:
            ss << "NILL";

            alg_type = SAA_ALG_STANDARD;
            error = SAA_modelGetAlgorithm(scene, model, &alg_type);
            if (error != SI_ERR_NONE) {
                lfprintf(log_file, "Error: Couldn't get algorithm type of nill!\n", 1);
                lfprintf(log_file, "\tBailing on nill: '%s'\n", 1, name.c_str());
                break;
            }

            ss << ' ';

            switch (alg_type) {
                case SAA_ALG_STANDARD:
                    ss << "ALG_STANDARD";
                    break;
                case SAA_ALG_INV_KIN:
                    ss << "ALG_INV_KIN";
                    break;
                case SAA_ALG_DYNAMIC:
                    ss << "ALG_DYNAMIC";
                    break;
                case SAA_ALG_INV_KIN_LEAF:
                    ss << "ALG_INV_KIN_LEAF";
                    break;
                case SAA_ALG_DYNA_LEAF:
                    ss << "ALG_DYNA_LEAF";
                    break;
                case SAA_ALG_GRAVITY:
                    ss << "ALG_GRAVITY";
                    break;
                case SAA_ALG_FORCE:
                    ss << "ALG_FORCE";
                    break;
                case SAA_ALG_WIND:
                    ss << "ALG_WIND";
                    break;
                case SAA_ALG_DEF_GRAVITY:
                    ss << "ALG_DEF_GRAVITY";
                    break;
                case SAA_ALG_FAN:
                    ss << "ALG_FAN";
                    break;
                case SAA_ALG_NAIL:
                    ss << "ALG_NAIL";
                    break;
                case SAA_ALG_DYN_MODEL:
                    ss << "ALG_DYN_MODE";
                    break;
                case SAA_ALG_CUSTOM_ICON:
                    ss << "ALG_CUSTOM_ICON";
                    break;
                case SAA_ALG_INSTANCE:
                    ss << "ALG_INSTANCE";
                    break;
                default:
                    ss << "UNKNOWN";
                    break;
            }
            break;
        case SAA_MPTCH:
            ss << "MPATCH";
            break;
        case SAA_MFACE:
            ss << "MFACE";
            break;
        case SAA_MSMSH:
            ss << "MSMSH";
            break;
        case SAA_MJNT:
            ss << "MJNT";
            break;
        case SAA_MSPLN:
            ss << "MSPLN";
            break;
        case SAA_MMETA:
            ss << "MMETA";
            break;
        case SAA_MBALL:
            ss << "MBALL";
            break;
        case SAA_MNCRV:
            ss << "MNCRV";
            break;
        case SAA_MNSRF:
            ss << "MNSRF";
            break;
        default:
            ss << "UNKNWON";
            break;
    }

    std::cout << ss.str() << std::endl;

    // Check for children.
    int num_children = 0;

    SAA_modelGetNbChildren(scene, model, &num_children);
    if (!num_children) { return; }

    // Allocate the memory. 
    SAA_Elem *children = new SAA_Elem[num_children];
    if (children == nullptr) {
        fprintf(log_file, "ERROR: Not enough memory for children to list!\n");
        return;
    }
    SAA_modelGetChildren(scene, model, num_children, children);
    if (children != nullptr) {
        for (int this_child = 0; this_child < num_children; this_child++) {
            ListHierarchy(scene, &children[this_child], indent_level + 2);
        }
    }
    delete[] children;
}

int ProcessScene(SAA_Database *database, SAA_Scene *scene, const char *scene_name) {
    SI_Error error = SAA_sceneLoad(database, scene_name, scene);
    if (error != SI_ERR_NONE) {
        fprintf(log_file, "Failed to load scene %s with error: %d\n", scene_name, error);
        safe_exit(1);
    }

    // Open an output file for texture list if specified.
    if (texture_list_filename != NULL) {
        _unlink(texture_list_filename);
        texture_list_file.open(texture_list_filename, std::ios::out, 0666);

        if (!texture_list_file) {
            fprintf(log_file, "Couldn't open output file: %s\n", texture_list_filename);
            safe_exit(1);
        }
    }

    if (SAA_updatelistGet(scene) == SI_SUCCESS) {
        float time;

        fprintf(log_file, "Setting Scene to frame %d...\n", pose_frame);
        // SAA_sceneSetPlayCtrlCurrentFrame( &scene, pose_frame );
        SAA_frame2Seconds(scene, pose_frame, &time);
        SAA_updatelistEvalScene(scene, time);
        //Sleep(100);
        SAA_updatelistEvalScene(scene, time);
        if (false) { // make_pose
            SAA_sceneFreeze(scene);
        }
    }

    // Get the number of models in the scene.
    int num_models = 0;
    SAA_sceneGetNbModels(scene, &num_models);
    if (!num_models) {
        fprintf(log_file, "Scene %s has no models, Doing nothing.\n", scene_name);
        safe_exit(1);
    }

    // Allocate array of models. 
    SAA_Elem *models = new SAA_Elem[num_models];
    if (!models) {
        fprintf(log_file, "FATAL ERROR: Failed to allocate SAA model pool!\n");
        safe_exit(2);
    }

    // Get all of the models from the scene.
    error = SAA_sceneGetModels(scene, num_models, models);
    if (error != SI_ERR_NONE) {
        fprintf(log_file, "Failed to get models in scene %s with error: %d\n", scene_name, error);
        safe_exit(1);
    }


    // FOR DEBBUGING: List Hierarchy. 
    if (verbose >= 3) {
        printf("HIERARCHY:\n");

        for (int i = 0; i < num_models; i++) {
            int level = 0;

            error = SAA_elementGetHierarchyLevel(scene, &models[i], &level);
            if (error != SI_ERR_NONE) {
                printf("Failed to get hierarchy level for model in scene %s with error: %d\n", scene_name, error);
                safe_exit(1);
            }

            // If the model isn't root level, Skip the model. 
            if (level > 0) { continue; }

            ListHierarchy(scene, &models[i], 0);
        };
    }

    // Process all of the models.
    for (int i = 0; i < num_models; i++) {
        int level = 0;

        error = SAA_elementGetHierarchyLevel(scene, &models[i], &level);
        if (error != SI_ERR_NONE) {
            fprintf(log_file, "Failed to get hierarchy level for model in scene %s with error: %d\n", scene_name, error);
            safe_exit(1);
        }

        // If the model isn't root level, Skip the model. 
        if (level > 0) { continue; }

        if (verbose >= 1) {
            fprintf(log_file, "\nProcessing scene model[%d]\n", i);
        }

        Element *element = ProcessElement(scene, &models[i], nullptr, nullptr);
        dprintf("DEBUG: Storing processed element %s with index %d!\n", element->get_name().c_str(), elements.size());
        if (element) { elements.push_back(element); }
    }

    // First setup all of element indexes. This is setup for resolving
    // parents and children when written. 
    for (size_t i = 0; i < elements.size(); i++) {
        Element *element = elements[i];
        assert(element != nullptr);
        element->prepare_children_indexes();
        element->set_array_position(i);
    }

    // Setup the children positions and the parent position for all of the children.
    for (size_t i = 0; i < elements.size(); i++) {
        Element *element = elements[i];
        assert(element != nullptr);
        uint32_t children_count = element->get_children_amount();
        size_t *chidren_indexes = element->get_children_indexes();
        for (uint32_t j = 0; j < children_count; j++) {
            Element *child = element->get_child(j);
            assert(child != nullptr);
            child->set_parent_position(element->get_array_position());
            chidren_indexes[j] = child->get_array_position();
        }
    }

    // Write all of the elements.
    std::string filename(scene_name);
    std::string file_ext(".scene");
    filename += file_ext;
    dprintf("Writing scene '%s' to file '%s'.\n", scene_name, filename.c_str());
    BinaryFile file(filename.c_str());
    file.write(elements.size());
    for (size_t i = 0; i < elements.size(); i++) {
        Element *element = elements[i];
        element->write(file);
    }

    // Free all of our elements.
    for (size_t i = 0; i < elements.size(); i++) {
        Element *element = elements[i];
        elements[i] = nullptr;
        delete element;
    }
    // Clear the elements.
    elements.clear();

    // Free the array of models.
    delete[] models;

    return 0;
}

SI_Error init_soft2scene(int argc, char* argv[]) {
    SI_Error error = SI_SUCCESS;

    std::cout << "Running soft2scene!\n";

    if ((scene_name == nullptr && model_name == nullptr) || database_name == nullptr) {
        //Usage();
        safe_exit(1);
    }
    
    if ((error = SAA_Init(rsrc_path, FALSE)) != SI_SUCCESS) {
        printf("Error: Couldn't get resource path!\n");
        safe_exit(1);
    }
    
    if ((error = SAA_databaseLoad(database_name, &database)) != SI_SUCCESS) {
        printf("Error: Couldn't load database!\n");
        safe_exit(1);
    }

    if ((error = SAA_sceneGetCurrent(&scene)) != SI_SUCCESS) {
        printf("Error: Couldn't load current scene!\n");
        safe_exit(1);
    }

    // Open the log for debugging.
    errno_t err = fopen_s(&log_file, "soft2scene.log", "w+");

    // Process the scene.
    if (scene_name != NULL) {
        ProcessScene(&database, &scene, scene_name);
    } else if (model_name != NULL) {

    }

    // Close the log for debugging.
    fclose(log_file);

    return error;
}


extern "C" {

/**
 * Instantiate converter and process a scenr
 */
__declspec(dllexport) SI_Error soft2scene(int argc, char *argv[]) {
#ifdef _DEBUG
    int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    flags |= _CRTDBG_CHECK_ALWAYS_DF;
    flags |= _CRTDBG_CHECK_CRT_DF;
    _CrtSetDbgFlag(flags);
#endif

    // Pass control to the c++ system
    return init_soft2scene(argc, argv);
}

}