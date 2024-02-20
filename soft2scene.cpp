// soft2scene.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <strstream>
#include <sstream>
#include <iostream>
#include <Windows.h>

#include <SAA.h>

#include "Element.h"

//// Global Variables ////

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
static SAA_Elem *textures = nullptr;

// Global toggles.
static bool make_poly = true;
static bool make_nurbs = true;
static bool make_duv = false;

// Other global variables. 
static std::ofstream texture_list_file;
static int pose_frame = 0;
static int nurbs_step = 1;
static int num_tex_loc = 0;
static int num_tex_glb = 0;
static int verbose = 3;

// Constants
static const int TEX_PER_MAT = 1;

#define dprintf(format, verbose_level, ...) if (verbose >= verbose_level) { printf(format, __VA_ARGS__); }

Element *ProcessModel(SAA_Scene *scene, SAA_Elem *model, Element *last_element = nullptr, Element *last_joint = nullptr);

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
    char *fullname = new char[fullname_len];
    strncpy_s(fullname, fullname_len, prefix, prefix_len);
    strncpy_s(fullname + prefix_len + 1, fullname_len - prefix_len, name, name_len);
    fullname[prefix_len] = '-';

    delete[] name;
    delete[] prefix;

    return fullname;
}

SI_Error HandleNill(SAA_Scene *scene, SAA_Elem *model, Element *new_element, Element *last_element = nullptr, Element *last_joint = nullptr) {
    // Get the assigned from the Element.
    const char *name = new_element->get_name().c_str();

    SAA_AlgorithmType type = SAA_ALG_STANDARD;
    SI_Error error = SAA_modelGetAlgorithm(scene, model, &type);
    if (error != SI_ERR_NONE) {
        dprintf("Error: Couldn't get algorithm type of nill!\n", 1);
        dprintf("\tBailing on nill: '%s'\n", 1, name);
        return error;
    }

    switch (type) {
        case SAA_ALG_STANDARD:
            dprintf("Nill <%s> is standard.\n", 2, name);
            break;
        case SAA_ALG_INV_KIN:
            dprintf("Nill <%s> has inverse kinetics.\n", 2, name);
            break;
        case SAA_ALG_DYNAMIC:
            dprintf("Nill <%s> is dynamic.\n", 2, name);
            break;
        case SAA_ALG_INV_KIN_LEAF:
            dprintf("Nill <%s> is a inverse kinetics leaf.\n", 2, name);
            break;
        case SAA_ALG_DYNA_LEAF:
            dprintf("Nill <%s> is a dynamic leaf.\n", 2, name);
            break;
        case SAA_ALG_GRAVITY:
            dprintf("Nill <%s> is gravity.\n", 2, name);
            break;
        case SAA_ALG_FORCE:
            dprintf("Nill <%s> is a force.\n", 2, name);
            break;
        case SAA_ALG_WIND:
            dprintf("Nill <%s> has wind.\n", 2, name);
            break;
        case SAA_ALG_DEF_GRAVITY:
            dprintf("Nill <%s> has definied gravity.\n", 2, name);
            break;
        case SAA_ALG_FAN:
            dprintf("Nill <%s> is a fan.\n", 2, name);
            break;
        case SAA_ALG_NAIL:
            dprintf("Nill <%s> is a nail.\n", 2, name);
            break;
        case SAA_ALG_DYN_MODEL:
            dprintf("Nill <%s> is a dynamic model.\n", 2, name);
            break;
        case SAA_ALG_CUSTOM_ICON:
            dprintf("Nill <%s> is a custom icon.\n", 2, name);
            break;
        case SAA_ALG_INSTANCE:
            dprintf("Nill <%s> is a instance.\n", 2, name);
            break;
        default:
            dprintf("Nill <%s> is a unknown algorithm type: %d, Skipping!\n", 2, name, type);
            return SI_ERR_BAD_PARAM;
    }

    // Get the nills matrix.
    Matrix4f &matrix = new_element->get_transformation_matrix();
    SAA_modelGetMatrix(scene, model, SAA_COORDSYS_GLOBAL, matrix.mat);

    if (verbose >= 2) {
        printf("Nill Matrix = %f %f %f %f\n", matrix.mat[0][0], matrix.mat[0][1], matrix.mat[0][2], matrix.mat[0][3]);
        printf("              %f %f %f %f\n", matrix.mat[1][0], matrix.mat[1][1], matrix.mat[1][2], matrix.mat[1][3]);
        printf("              %f %f %f %f\n", matrix.mat[2][0], matrix.mat[2][1], matrix.mat[2][2], matrix.mat[2][3]);
        printf("              %f %f %f %f\n", matrix.mat[3][0], matrix.mat[3][1], matrix.mat[3][2], matrix.mat[3][3]);
    }

    // Get the position, rotation and scale indivdually.
    Vector3f &pos = new_element->get_position();
    Vector3f &rot = new_element->get_rotation();
    Vector3f &scale = new_element->get_scale();

    SAA_modelGetTranslation(scene, model, SAA_COORDSYS_LOCAL, &pos.x, &pos.y, &pos.z);
    SAA_modelGetRotation(scene, model, SAA_COORDSYS_LOCAL, &rot.x, &rot.y, &rot.z);
    SAA_modelGetScaling(scene, model, SAA_COORDSYS_LOCAL, &scale.x, &scale.y, &scale.z);

    if (verbose >= 3) {
        printf("Nill Pos = %f %f %f\n", pos.x, pos.y, pos.z);
        printf("Nill Rot = %f %f %f\n", rot.x, rot.y, rot.z);
        printf("Nill Scale = %f %f %f\n", scale.x, scale.y, scale.z);
    }

    SAA_Boolean visible = FALSE;
    SAA_modelGetNodeVisibility(scene, model, &visible);
    new_element->set_visibility(visible != FALSE);
    dprintf("Visibility: %d\n", 1, visible);


    // Check for children.
    int num_children = 0;
    SAA_Elem *children = nullptr;

    SAA_modelGetNbChildren(scene, model, &num_children);
    dprintf("Children Amount: %d\n", 1, num_children);

    if (num_children) {
        children = new SAA_Elem[num_children];
        if (children == nullptr) {
            printf("ERROR: Not enough memory for children.\n");
            return SI_ERR_ALLOC_PROBLEM;
        }
        SAA_modelGetChildren(scene, model, num_children, children);
        if (children != nullptr) {
            new_element->prepare_children(num_children);
            for (int this_child = 0; this_child < num_children; this_child++) {
                dprintf("\nProcessing child %d...\n", 1, this_child);
                Element *child = ProcessModel(scene, &children[this_child], new_element, last_joint);
                new_element->set_child(this_child, child);
            }
        }
        delete[] children;
    } else {
        dprintf("Don't descend this branch!\n", 1);
    }

    return error;
}

Element *ProcessModel(SAA_Scene *scene, SAA_Elem *model, Element *last_element, Element *last_joint) {
    Element *new_element = new Element;

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

    dprintf("Element name <%s>\n", 1, name);

    // If we have a previous element, It is our parent element!
    if (last_element != nullptr) { new_element->set_parent(last_element); }

    // Find out what type of node we're dealing with.
    SAA_ModelType type = SAA_MNILL;
    SI_Error error = SAA_modelGetType(scene, model, &type);

    switch (type) {
        case SAA_MNILL:
            dprintf("Element <%s> is a nill.\n", 2, name);
            HandleNill(scene, model, new_element, last_element, last_joint);
            return new_element;
        case SAA_MPTCH:
            dprintf("Element <%s> is a patch.\n", 2, name);
            break;
        case SAA_MFACE:
            dprintf("Element <%s> is a face.\n", 2, name);
            break;
        case SAA_MSMSH:
            dprintf("Element <%s> is a mesh.\n", 2, name);
            break;
        case SAA_MJNT:
            dprintf("Element <%s> is a joint.\n", 2, name);
            break;
        case SAA_MSPLN:
            dprintf("Element <%s> is a spline.\n", 2, name);
            break;
        case SAA_MMETA:
            dprintf("Element <%s> is a meta element.\n", 2, name);
            break;
        case SAA_MBALL:
            dprintf("Element <%s> is a metaball.\n", 2, name);
            break;
        case SAA_MNCRV:
            dprintf("Element <%s> is a nurb curve.\n", 2, name);
            break;
        case SAA_MNSRF:
            dprintf("Element <%s> is a nurbs surf.\n", 2, name);
            break;
        default:
            dprintf("Element <%s> is a unknown type: %d, Skipping!\n", 2, name, type);
            delete new_element;
            return nullptr;
    }

    // Get the models matrix.
    Matrix4f &matrix = new_element->get_transformation_matrix();
    SAA_modelGetMatrix(scene, model, SAA_COORDSYS_GLOBAL, matrix.mat);

    if (verbose >= 2) {
        printf("Model Matrix = %f %f %f %f\n", matrix.mat[0][0], matrix.mat[0][1], matrix.mat[0][2], matrix.mat[0][3]);
        printf("               %f %f %f %f\n", matrix.mat[1][0], matrix.mat[1][1], matrix.mat[1][2], matrix.mat[1][3]);
        printf("               %f %f %f %f\n", matrix.mat[2][0], matrix.mat[2][1], matrix.mat[2][2], matrix.mat[2][3]);
        printf("               %f %f %f %f\n", matrix.mat[3][0], matrix.mat[3][1], matrix.mat[3][2], matrix.mat[3][3]);
    }

    // Get the position, rotation and scale indivdually.
    Vector3f &pos = new_element->get_position();
    Vector3f &rot = new_element->get_rotation();
    Vector3f &scale = new_element->get_scale();

    SAA_modelGetTranslation(scene, model, SAA_COORDSYS_LOCAL, &pos.x, &pos.y, &pos.z);
    SAA_modelGetRotation(scene, model, SAA_COORDSYS_LOCAL, &rot.x, &rot.y, &rot.z);
    SAA_modelGetScaling(scene, model, SAA_COORDSYS_LOCAL, &scale.x, &scale.y, &scale.z);

    if (verbose >= 3) {
        printf("Model Pos = %f %f %f\n", pos.x, pos.y, pos.z);
        printf("Model Rot = %f %f %f\n", rot.x, rot.y, rot.z);
        printf("Model Scale = %f %f %f\n", scale.x, scale.y, scale.z);
    }

    SAA_Boolean visible = FALSE;
    SAA_modelGetNodeVisibility(scene, model, &visible);
    new_element->set_visibility(visible != FALSE);
    dprintf("Visibility: %d\n", 1, visible);

    // Only create egg polygon data if: the node is visible, and its not a
    // NULL or a Joint, and we're outputing polys (or if we are outputing
    // NURBS and the model is a poly mesh or a face)
    if (!visible || (type == SAA_MNILL) || (type == SAA_MJNT)) {
        delete new_element;
        return nullptr;
    }
    if (((!make_poly && !(make_nurbs && ((type == SAA_MSMSH) || (type == SAA_MFACE)))))) {
        delete new_element;
        return nullptr;
    }

    // If the model is a NURBS in soft, set its step before tesselating
    if (type == SAA_MNSRF) {
        error = SAA_nurbsSurfaceSetStep(scene, model, nurbs_step, nurbs_step);
    } else if (type == SAA_MPTCH) { // If the model is a PATCH in soft, set its step before tesselating
        error = SAA_patchSetStep(scene, model, nurbs_step, nurbs_step);
    }

    SAA_GeomType gtype = SAA_GEOM_ORIGINAL;

    // If making a pose - Get deformed geometry.
    if (false) { gtype = SAA_GEOM_DEFORMED; } // make_pose

    // Get the number of key shapes
    int num_shapes = 0;
    SAA_modelGetNbShapes(scene, model, &num_shapes);
    if (verbose >= 1) {
        printf("ProcessModel: Amount of shapes: %d\n", num_shapes);
    }

    // TODO: Process all of our shapes. 
    //for (int i = 0; i < num_shapes; i++) {
    //}

    // Get the number of triangles
    int num_tri = 0;
    int id = 0;
    error = SAA_modelGetNbTriangles(scene, model, gtype, id, &num_tri);
    if (error != SI_ERR_NONE) {
        if (verbose >= 1) {
           printf("Error: Couldn't get number of triangles!\n");
           printf("\tBailing on model: '%s'\n", name);
        }
        delete new_element;
        return nullptr;
    } else if (verbose >= 1) {
        printf("Model Triangle Count: %d\n", num_tri);
    }

    // Check also to see if the surface is a skeleton.
    SAA_Boolean is_skeleton = FALSE;
    SAA_modelIsSkeleton(scene, model, &is_skeleton);

    // check to see if this surface is used as a skeleton or is animated via
    // constraint only ( these nodes are tagged by the animator with the
    // keyword "joint" somewhere in the nodes name)
    if (is_skeleton || (strstr(name, "joint") != NULL)) {
        if (verbose >= 1) {
            printf("Animating polys as joint!!!\n");
        }

        //MakeJoint(scene, lastJoint, lastAnim, model, name);
    }

    // Model is not a null and has no triangles!
    if (!num_tri && verbose >= 1) {
        printf("No triangles!\n");
    }

    // Allocate array of triangles and copy them all into it.
    SAA_SubElem *triangles = new SAA_SubElem[num_tri];
    if (triangles != nullptr) {
        // Triangulate model and read the triangles into array.
        SAA_modelGetTriangles(scene, model, gtype, id, num_tri, triangles);
    } else {
        printf("Not enough memory for triangles! Aborting!\n");
        exit(1);
    }

    // Allocate array of materials for each triangle and copy them all into it.
    SAA_Elem *materials = new SAA_Elem[num_tri];
    if (materials != nullptr) {
        // Read each triangle's material into array.
        SAA_triangleGetMaterials(scene, model, num_tri, triangles, materials);
    } else {
        printf("Not enough memory for materials! Aborting!\n");
        exit(1);
    }

    // Allocate array of textures per triangle.
    int *num_tex_tri = new int[num_tri];

    // Find out how many local textures per triangle.
    void *rel_info = nullptr;
    for (int i = 0; i < num_tri; i++) {
        error = SAA_materialRelationGetT2DLocNbElements(scene, &materials[i], FALSE, (const void **)&rel_info, &num_tex_tri[i]);
        // polytex
        if (error == SI_ERR_NONE) {
            num_tex_loc += num_tex_tri[i];
        }
    }

    // Get local textures if present
    if (num_tex_loc) {
        // ASSUME only one texture per material
        textures = new SAA_Elem[num_tri];

        for (int i = 0; i < num_tri; i++) {
            // and read all referenced local textures into array
            SAA_materialRelationGetT2DLocElements(scene, &materials[i], TEX_PER_MAT, &textures[i]);
        }

        if (verbose >= 1) {
            printf("num_tex_loc = %d\n", num_tex_loc);
        }
    }
    // If no local textures, Try to get global textures.
    else {
        SAA_modelRelationGetT2DGlbNbElements(scene, model, FALSE, (const void **)&rel_info, &num_tex_glb);

        if (num_tex_glb) {
            // ASSUME only one texture per model
            textures = new SAA_Elem[1];

            // get the referenced texture
            SAA_modelRelationGetT2DGlbElements(scene, model, TEX_PER_MAT, textures);

            if (verbose >= 1) {
                printf("num_tex_glb = %d\n", num_tex_glb);
            }
        }
    }

    // Allocate array of control vertices
    SAA_SubElem *cvertices = new SAA_SubElem[num_tri * 3];
    if (cvertices != nullptr) {
        // Read each triangles control vertices into the array.
        SAA_triangleGetCtrlVertices(scene, model, gtype, id, num_tri, triangles, cvertices);

        if (verbose >= 2) {
            SAA_DVector *cvertPos = new SAA_DVector[num_tri * 3];
            SAA_ctrlVertexGetPositions(scene, model, num_tri * 3, cvertices, cvertPos);

            for (int i = 0; i < num_tri * 3; i++) {
                printf("cvert[%d] = %f %f %f %f\n", i,cvertPos[i].x, cvertPos[i].y, cvertPos[i].z, cvertPos[i].w);
            }

            delete[] cvertPos;
        }
    } else {
        printf("Not enough memory for control vertices! Aborting!\n");
        exit(1);
    }

    // Check for children.
    int num_children = 0;
    SAA_Elem *children = nullptr;

    SAA_modelGetNbChildren(scene, model, &num_children);
    dprintf("Children Amount: %d\n", 1, num_children);

    if (num_children) {
        children = new SAA_Elem[num_children];
        if (children == nullptr) {
            printf("ERROR: Not enough memory for children.\n");
            delete new_element;
            return nullptr;
        }
        SAA_modelGetChildren(scene, model, num_children, children);
        if (children != nullptr) {
            new_element->prepare_children(num_children);
            for (int this_child = 0; this_child < num_children; this_child++) {
                dprintf("\nProcessing child %d...\n", 1, this_child);
                Element *child = ProcessModel(scene, &children[this_child], new_element, last_joint);
                new_element->set_child(this_child, child);
            }
        }
        delete[] children;
    } else {
        dprintf("Don't descend this branch!\n", 1);
    }

    delete[] triangles;
    delete[] materials;
    delete[] num_tex_tri;
    if (textures) { delete[] textures; }
    delete[] cvertices;
}

void IndentStream(std::stringstream &ss, int indent_level) {
    for (int indent = 0; indent < indent_level; indent++) {
        ss << ' ';
    }
}

void ListHierarchy(SAA_Scene *scene, SAA_Elem *model, int indent_level) {
    // Get the name of the element.
    char *name = nullptr;
    if (true) { // use_prefix
        // Get the FULL name of the trim curve
        name = GetFullName(scene, model);
    } else {
        // Get the name of the trim curve
        name = GetName(scene, model);
    }

    std::stringstream ss;
    IndentStream(ss, indent_level);
    ss << name << ' ';

    // Free the allocated name.
    delete[] name; 

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
                dprintf("Error: Couldn't get algorithm type of nill!\n", 1);
                dprintf("\tBailing on nill: '%s'\n", 1, name);
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
        printf("ERROR: Not enough memory for children to list!\n");
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
        printf("Failed to load scene %s with error: %d\n", scene_name, error);
        exit(1);
    }

    // Open an output file for texture list if specified.
    if (texture_list_filename != NULL) {
        _unlink(texture_list_filename);
        texture_list_file.open(texture_list_filename, std::ios::out, 0666);

        if (!texture_list_file) {
            printf("Couldn't open output file: %s\n", texture_list_filename);
            exit(1);
        }
    }

    if (SAA_updatelistGet(scene) == SI_SUCCESS) {
        float time;

        printf("Setting Scene to frame %d...\n", pose_frame);
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
        printf("Scene %s has no models, Doing nothing.\n", scene_name);
        exit(1);
    }

    // Allocate array of models. 
    SAA_Elem *models = new SAA_Elem[num_models];
    if (!models) {
        printf("FATAL ERROR: Failed to allocate SAA model pool!\n");
        exit(2);
    }

    // Get all of the models from the scene.
    error = SAA_sceneGetModels(scene, num_models, models);
    if (error != SI_ERR_NONE) {
        printf("Failed to get models in scene %s with error: %d\n", scene_name, error);
        exit(1);
    }


    // FOR DEBBUGING: List Hierarchy. 
    if (verbose >= 3) {
        printf("HIERARCHY:\n");

        for (int i = 0; i < num_models; i++) {
            int level = 0;

            error = SAA_elementGetHierarchyLevel(scene, &models[i], &level);
            if (error != SI_ERR_NONE) {
                printf("Failed to get hierarchy level for model in scene %s with error: %d\n", scene_name, error);
                exit(1);
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
            printf("Failed to get hierarchy level for model in scene %s with error: %d\n", scene_name, error);
            exit(1);
        }

        // If the model isn't root level, Skip the model. 
        if (level > 0) { continue; }

        if (verbose >= 1) {
            printf("\nProcessing scene model[%d]\n", i);
        }

        ProcessModel(scene, &models[i], nullptr, nullptr);
    }

    // Free the array of models.
    delete[] models;

    return 0;
}

SI_Error init_soft2scene(int argc, char* argv[]) {
    SI_Error error = SI_SUCCESS;

    std::cout << "Running soft2scene!\n";

    if ((scene_name == nullptr && model_name == nullptr) || database_name == nullptr) {
        //Usage();
        exit(1);
    }
    
    if ((error = SAA_Init(rsrc_path, FALSE)) != SI_SUCCESS) {
        printf("Error: Couldn't get resource path!\n");
        exit(1);
    }
    
    if ((error = SAA_databaseLoad(database_name, &database)) != SI_SUCCESS) {
        printf("Error: Couldn't load database!\n");
        exit(1);
    }

    if ((error = SAA_sceneGetCurrent(&scene)) != SI_SUCCESS) {
        printf("Error: Couldn't load current scene!\n");
        exit(1);
    }

    if (scene_name != NULL) {
        ProcessScene(&database, &scene, scene_name);
    } else if (model_name != NULL) {

    }

    return error;
}


extern "C" {

/**
 * Instantiate converter and process a scenr
 */
__declspec(dllexport) SI_Error soft2scene(int argc, char *argv[]) {
    // Pass control to the c++ system
    return init_soft2scene(argc, argv);
}

}