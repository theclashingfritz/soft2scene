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
#include "Constraint.h"
#include "Element.h"
#include "Material.h"
#include "Utilities.h"

//// Global Variables ////

// Global tables
static std::vector<Element *> elements;
static std::vector<Element *> root_joints;
static std::vector<Constraint *> constraints;

// Global strings
static char default_scene_name[] = "suitA-zero.1-0";
static char *scene_name = default_scene_name;
static char *model_name = nullptr;
static char default_database_name[] = "C:/Users/theclashingfritz/player/softimage";
static char *database_name = default_database_name;
static char default_rsrc_path[] = "D:/Softimage/SOFT3D_4.0/3D/rsrc";
static char *rsrc_path = default_rsrc_path;
static char *texture_list_filename = nullptr;

// Global SAA tables
static std::vector<Material> scene_materials;

// Global SAA variables
static SAA_Database database;
static SAA_Scene scene;
static SAA_Boolean uv_swap = FALSE;

// Global toggles.
static bool make_poly = true;
static bool make_nurbs = true;
static bool make_duv = false;
static bool use_prefix = false;

// Other global variables. 
static std::ofstream texture_list_file;
static int pose_frame = 0;
static int nurbs_step = 1;
static int num_tex_loc = 0;
static int num_tex_glb = 0;

// Constants
static const int TEX_PER_MAT = 1;

Element *ProcessElement(SAA_Scene *scene, SAA_Elem *model, Element *last_element = nullptr, Element *last_joint = nullptr);

Element *SAAElemToElement(SAA_Scene *scene, SAA_Elem *model) {
    std::string name;
    {
        // Get the name of the element.
        char *namebuffer = nullptr;
        if (use_prefix) {
            // Get the FULL name of the trim curve
            namebuffer = GetFullName(scene, model);
        } else {
            // Get the name of the trim curve
            namebuffer = GetName(scene, model);
        }

        name = std::string(namebuffer);

        // Free the copy of the name. We no longer need it.
        delete[] namebuffer;
    }
    
    for (size_t i = 0; i < elements.size(); i++) {
        Element *element = elements[i];
        std::string &element_name = element->get_name();
        if (element_name.compare(name) == 0) { return element; }
    }
    
    lprintf("ERROR: Couldn't find matching Element for SAA Element.", 0);
    return nullptr;
}

void CreateConstraints(SAA_Scene *scene, SAA_Elem *model) {
    Element *element = SAAElemToElement(scene, model);

    void *rel_info = nullptr;
    SAA_Elem *elements = nullptr;
    int active_element_count = 0;
    int passive_element_count = 0;
    int total_element_count = 0;
    
    // Constrains the centers of the passive elements to the center of the active element.
    SAA_modelRelationGetCnsPosNbElements(scene, model, 0, (const void **)&rel_info, &passive_element_count);
    SAA_modelRelationGetCnsPosNbElements(scene, model, 1, (const void **)&rel_info, &active_element_count);
    dprintf("Passive Pos Constraint Elements: %d\n", passive_element_count);
    dprintf("Active Pos Constraint Elements: %d\n", active_element_count);
    
    // Only save a contraint if we have active elements.
    if (active_element_count > 0) {
        // Allocate buffer for all of the SAA Elements.
        elements = new SAA_Elem[active_element_count + 1];

        // Extract all of the active elements into the buffer.
        SAA_modelRelationGetCnsPosElements(scene, model, rel_info, active_element_count, elements);

        // Allocate our constraint.
        Constraint *pos_constraint = new Constraint(Constraint::Type::POSITION);
        // Set the passive element to the element we're checking for constraints on.
        pos_constraint->set_passive_element(element);
        // Prepare the buffer for all of the active elements.
        pos_constraint->prepare_active_elements(active_element_count);
        // Get the buffer to act upon.
        Element **active_constraint_elements = pos_constraint->get_active_elements();
        
        // Get if the constraint is active or inactive.
        SAA_Boolean active = FALSE;
        SAA_modelRelationGetCnsPosActive(scene, model, model, &active);
        // Set the activity of the constraint.
        pos_constraint->set_active(active == TRUE);

        // Iterate all of the active elements.
        for (int i = 0; i < active_element_count; i++) {
            // Get the active element.
            SAA_Elem &saa_element = elements[i];
            // Convert from a Softimage Element to our own Element type.
            Element *active_element = SAAElemToElement(scene, &saa_element);
            printf("Active Pos Constraint Element <%s> for Element <%s>\n", active_element->get_name().c_str(), element->get_name().c_str());
            // Add the active element we found to the buffer.
            active_constraint_elements[i] = active_element;
        }
        constraints.push_back(pos_constraint);

        delete[] elements;
    }
    
    // Constrains the rotation of the passive elements to the rotation of the active element.
    SAA_modelRelationGetCnsOriNbElements(scene, model, 0, (const void **)&rel_info, &passive_element_count);
    SAA_modelRelationGetCnsOriNbElements(scene, model, 1, (const void **)&rel_info, &active_element_count);
    dprintf("Passive Ori Constraint Elements: %d\n", passive_element_count);
    dprintf("Active Ori Constraint Elements: %d\n", active_element_count);

    // Only save a contraint if we have active elements.
    if (active_element_count > 0) {
        // Allocate buffer for all of the SAA Elements.
        elements = new SAA_Elem[active_element_count + 1];

        // Extract all of the active elements into the buffer.
        SAA_modelRelationGetCnsOriElements(scene, model, rel_info, active_element_count, elements);

        // Allocate our constraint.
        Constraint *ori_constraint = new Constraint(Constraint::Type::ORIENTATION);
        // Set the passive element to the element we're checking for constraints on.
        ori_constraint->set_passive_element(element);
        // Prepare the buffer for all of the active elements.
        ori_constraint->prepare_active_elements(active_element_count);
        // Get the buffer to act upon.
        Element **active_constraint_elements = ori_constraint->get_active_elements();
        
        // Get if the constraint is active or inactive.
        SAA_Boolean active = FALSE;
        SAA_modelRelationGetCnsOriActive(scene, model, model, &active);
        // Set the activity of the constraint.
        ori_constraint->set_active(active == TRUE);

        // Iterate all of the active elements.
        for (int i = 0; i < active_element_count; i++) {
            // Get the active element
            SAA_Elem &saa_element = elements[i];
            // Convert from a Softimage Element to our own Element type.
            Element *active_element = SAAElemToElement(scene, &saa_element);
            printf("Active Ori Constraint Element <%s> for Element <%s>\n", active_element->get_name().c_str(), element->get_name().c_str());
            // Add the active element we found to the buffer.
            active_constraint_elements[i] = active_element;
        }
        constraints.push_back(ori_constraint);

        delete[] elements;
    }

    // Constrains the scaling of the passive elements to the same scaling as the active element.
    SAA_modelRelationGetCnsSclNbElements(scene, model, 0, (const void **)&rel_info, &passive_element_count);
    SAA_modelRelationGetCnsSclNbElements(scene, model, 1, (const void **)&rel_info, &active_element_count);
    dprintf("Passive Scl Constraint Elements: %d\n", passive_element_count);
    dprintf("Active Scl Constraint Elements: %d\n", active_element_count);

    // Only save a contraint if we have active elements.
    if (active_element_count > 0) {
        // Allocate buffer for all of the SAA Elements.
        elements = new SAA_Elem[active_element_count + 1];

        // Extract all of the active elements into the buffer.
        SAA_modelRelationGetCnsSclElements(scene, model, rel_info, active_element_count, elements);

        // Allocate our constraint.
        Constraint *scl_constraint = new Constraint(Constraint::Type::SCALE);
        // Set the passive element to the element we're checking for constraints on.
        scl_constraint->set_passive_element(element);
        // Prepare the buffer for all of the active elements.
        scl_constraint->prepare_active_elements(active_element_count);
        // Get the buffer to act upon.
        Element **active_constraint_elements = scl_constraint->get_active_elements();
        
        // Get if the constraint is active or inactive.
        SAA_Boolean active = FALSE;
        SAA_modelRelationGetCnsSclActive(scene, model, model, &active);
        // Set the activity of the constraint.
        scl_constraint->set_active(active == TRUE);

        // Iterate all of the active elements.
        for (int i = 0; i < active_element_count; i++) {
            // Get the active element
            SAA_Elem &saa_element = elements[i];
            // Convert from a Softimage Element to our own Element type.
            Element *active_element = SAAElemToElement(scene, &saa_element);
            printf("Active Scl Constraint Element <%s> for Element <%s>\n", active_element->get_name().c_str(), element->get_name().c_str());
            // Add the active element we found to the buffer.
            active_constraint_elements[i] = active_element;
        }
        constraints.push_back(scl_constraint);

        delete[] elements;
    }

    // Constrains the range of position of the passive element relative to the active element. 
    SAA_modelRelationGetCnsPosLimNbElements(scene, model, 0, (const void **)&rel_info, &passive_element_count);
    SAA_modelRelationGetCnsPosLimNbElements(scene, model, 1, (const void **)&rel_info, &active_element_count);
    dprintf("Passive Pos Limit Constraint Elements: %d\n", passive_element_count);
    dprintf("Active Pos Limit Constraint Elements: %d\n", active_element_count);

    // Only save a contraint if we have a active and passive element (1)
    if (passive_element_count == 1 && active_element_count == 1) {
        // Allocate buffer for all of the SAA Elements.
        elements = new SAA_Elem[active_element_count + 1];

        // Extract all of the active elements into the buffer.
        SAA_modelRelationGetCnsPosLimElements(scene, model, rel_info, active_element_count, elements);

        // Allocate our constraint.
        Constraint *pos_limit_constraint = new Constraint(Constraint::Type::POSITION_LIMIT);
        // Set the passive element to the element we're checking for constraints on.
        pos_limit_constraint->set_passive_element(element);
        // Prepare the buffer for all of the active elements.
        pos_limit_constraint->prepare_active_elements(active_element_count);
        // Get the buffer to act upon.
        Element **active_constraint_elements = pos_limit_constraint->get_active_elements();

        // Get the active element
        SAA_Elem &saa_element = elements[0];
        // Convert from a Softimage Element to our own Element type.
        Element *active_element = SAAElemToElement(scene, &saa_element);
        printf("Active Pos Limit Constraint Element <%s> for Element <%s>\n", active_element->get_name().c_str(), element->get_name().c_str());
        // Add the active element we found to the buffer.
        active_constraint_elements[0] = active_element;
        
        // Get if the constraint is active or inactive.
        SAA_Boolean active = FALSE;
        SAA_modelRelationGetCnsPosLimActive(scene, model, model, &active);
        // Set the activity of the constraint.
        pos_limit_constraint->set_active(active == TRUE);

        // Handle constraint attributes
        PositionLimits *limits = pos_limit_constraint->get_position_limits();

        // Get and store the coordinate system.
        SAA_CoordSys system = SAA_COORDSYS_GLOBAL;
        SAA_modelRelationGetCnsPosLimCoordSys(scene, &saa_element, model, &system);
        limits->set_coordinate_system((CoordinateSystem)system);

        // Get and store the shape type.
        SAA_CnsPosLimType type = SAA_CNSPOSLIM_SPHERE;
        SAA_modelRelationGetCnsPosLimType(scene, &saa_element, model, &type);
        limits->set_shape_type((PositionLimits::ShapeType)type);

        // Get and store our damping attributes.
        float width, strength;
        SAA_modelRelationGetCnsPosLimDamping(scene, &saa_element, model, &width, &strength);
        limits->set_damping(width, strength);

        // Get and store our radius.
        float radius;
        SAA_modelRelationGetCnsPosLimRadius(scene, &saa_element, model, &radius);
        limits->set_radius(radius);

        // Get and store the maximum position.
        Vector3f &max_pos = limits->get_max_pos();
        SAA_modelRelationGetCnsPosLimMax(scene, &saa_element, model, &max_pos.x, &max_pos.y, &max_pos.z);

        // Get and store the minimum position.
        Vector3f &min_pos = limits->get_min_pos();
        SAA_modelRelationGetCnsPosLimMin(scene, &saa_element, model, &min_pos.x, &min_pos.y, &min_pos.z);

        // Active booleans.
        SAA_Boolean x_active = FALSE;
        SAA_Boolean y_active = FALSE;
        SAA_Boolean z_active = FALSE;

        // Get and store the maximum position activeness.
        SAA_modelRelationGetCnsPosLimMaxActive(scene, &saa_element, model, &x_active, &y_active, &z_active);
        limits->set_max_pos_active(x_active != FALSE, y_active != FALSE, z_active != FALSE);

        // Get and store the minimum position activeness.
        SAA_modelRelationGetCnsPosLimMinActive(scene, &saa_element, model, &x_active, &y_active, &z_active);
        limits->set_min_pos_active(x_active != FALSE, y_active != FALSE, z_active != FALSE);

        // We're done, Add the constraint to the vector.
        constraints.push_back(pos_limit_constraint);

        // Free the allocated element space.
        delete[] elements;
    }

    // Constrains the up (y-axis) direction of the passive element.
    SAA_modelRelationGetCnsUpVctNbElements(scene, model, 0, (const void **)&rel_info, &passive_element_count);
    SAA_modelRelationGetCnsUpVctNbElements(scene, model, 1, (const void **)&rel_info, &active_element_count);
    dprintf("Passive Up-Vct Constraint Elements: %d\n", passive_element_count);
    dprintf("Active Up-Vct Constraint Elements: %d\n", active_element_count);
    
    // Only save a contraint if we have active elements.
    if (active_element_count > 0) {
        // Allocate buffer for all of the SAA Elements.
        elements = new SAA_Elem[active_element_count + 1];
        
        // Extract all of the active elements into the buffer.
        SAA_modelRelationGetCnsUpVctElements(scene, model, rel_info, active_element_count, elements);
        
        // Allocate our constraint.
        Constraint *up_vct_constraint = new Constraint(Constraint::Type::UP_VCT);
        // Set the passive element to the element we're checking for constraints on.
        up_vct_constraint->set_passive_element(element);
        // Prepare the buffer for all of the active elements.
        up_vct_constraint->prepare_active_elements(active_element_count);
        // Get the buffer to act upon.
        Element **active_constraint_elements = up_vct_constraint->get_active_elements();
        
        // Get if the constraint is active or inactive.
        SAA_Boolean active = FALSE;
        SAA_modelRelationGetCnsUpVctActive(scene, model, model, &active);
        // Set the activity of the constraint.
        up_vct_constraint->set_active(active == TRUE);
        
        // Iterate all of the active elements.
        for (int i = 0; i < active_element_count; i++) {
            // Get the active element
            SAA_Elem &saa_element = elements[i];
            // Convert from a Softimage Element to our own Element type.
            Element *active_element = SAAElemToElement(scene, &saa_element);
            printf("Active Up-Vct Constraint Element <%s> for Element <%s>\n", active_element->get_name().c_str(), element->get_name().c_str());
            // Add the active element we found to the buffer.
            active_constraint_elements[i] = active_element;
        }
        constraints.push_back(up_vct_constraint);
        
        // Free the allocated element space.
        delete[] elements;
    }
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
        if (use_prefix) {
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
        char *name = nullptr;
        if (use_prefix) {
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

    SAA_Boolean is_skeleton = FALSE;
    SAA_modelIsSkeleton(scene, model, &is_skeleton);

    // Check to see if this surface is used as a skeleton.
    if (is_skeleton) {
        lfprintf(log_file, "Animating nill as joint!!!\n", 1);

        // We're a joint!
        new_element->set_as_joint(true);

        // Make sure we set the last node as our previous.
        new_element->prepare_joint_linklist_node();
        JointLinkListNode *node = new_element->get_joint_linklist_node();
        node->set_previous(last_joint);

        // Make sure we increment the amount of next nodes in the previous joint.
        if (last_joint) {
            JointLinkListNode *prev_node = last_joint->get_joint_linklist_node();
            prev_node->set_next_count(prev_node->get_next_count() + 1);
        } else {
            root_joints.push_back(new_element);
        }

        // We are now the new previous joint!
        last_joint = new_element;
    }

    HandleElementChildren(scene, model, new_element, last_joint);

    return new_element;
}

Material InitializeMaterial(SAA_Scene *scene, SAA_Elem *saa_material) {
    char *material_name = GetName(scene, saa_material);

    // Create the material for our scene materials.
    Material new_material(material_name);
    delete[] material_name;

    SAA_ShadingModelType shade_model = SAA_SHM_CONSTANT;
    SI_Error error = SAA_materialGetShadingModel(scene, saa_material, &shade_model);
    if (error != SI_ERR_NONE) {
        printf("ERROR: Failed to get shading model for material %s!\n", new_material.get_name().c_str());
    }
    new_material.set_shade_model((Material::ShadingModel)shade_model);

    SAA_DiffuseSrcType diffuse_type = SAA_MAT_DIFFUSE_SRC_MAT;
    error = SAA_materialGetDiffuseSrc(scene, saa_material, &diffuse_type);
    if (error != SI_ERR_NONE) {
        printf("ERROR: Failed to get diffuse source for material %s!\n", new_material.get_name().c_str());
    }
    new_material.set_diffuse_source((Material::ComponentSource)diffuse_type);

    SAA_TransparencySrcType transparency_type = SAA_MAT_TRANSPARENCY_SRC_MAT;
    error = SAA_materialGetTransparencySrc(scene, saa_material, &transparency_type);
    if (error != SI_ERR_NONE) {
        printf("ERROR: Failed to get transparency source for material %s!\n", new_material.get_name().c_str());
    }
    new_material.set_transparency_source((Material::ComponentSource)transparency_type);

    SAA_Boolean static_blur = FALSE;
    error = SAA_materialGetStaticBlur(scene, saa_material, &static_blur);
    if (error != SI_ERR_NONE) {
        printf("ERROR: Failed to get if static blur is enabled for material %s!\n", new_material.get_name().c_str());
    }
    new_material.set_static_blur(static_blur != FALSE);

    Vector3f &ambieance = new_material.get_ambieance();
    error = SAA_materialGetAmbient(scene, saa_material, &ambieance.x, &ambieance.y, &ambieance.z);
    if (error != SI_ERR_NONE) {
        printf("ERROR: Failed to get ambieance for material %s!\n", new_material.get_name().c_str());
    }

    Vector3f &diffuse = new_material.get_diffuse();
    error = SAA_materialGetDiffuse(scene, saa_material, &diffuse.x, &diffuse.y, &diffuse.z);
    if (error != SI_ERR_NONE) {
        printf("ERROR: Failed to get diffuse for material %s!\n", new_material.get_name().c_str());
    }

    Vector3f &specular = new_material.get_specular();
    error = SAA_materialGetSpecular(scene, saa_material, &specular.x, &specular.y, &specular.z);
    if (error != SI_ERR_NONE) {
        printf("ERROR: Failed to get specular for material %s!\n", new_material.get_name().c_str());
    }

    float specular_decay = 0.0f;
    error = SAA_materialGetSpecularDecay(scene, saa_material, &specular_decay);
    if (error != SI_ERR_NONE) {
        printf("ERROR: Failed to get specular decay for material %s!\n", new_material.get_name().c_str());
    }
    new_material.set_specular_decay(specular_decay);

    float reflection = 0.0f;
    error = SAA_materialGetReflection(scene, saa_material, &reflection);
    if (error != SI_ERR_NONE) {
        printf("ERROR: Failed to get reflection for material %s!\n", new_material.get_name().c_str());
    }
    new_material.set_reflection(reflection);

    float refractive_index = 0.0f;
    error = SAA_materialGetRefractiveIndex(scene, saa_material, &refractive_index);
    if (error != SI_ERR_NONE) {
        printf("ERROR: Failed to get refractive index for material %s!\n", new_material.get_name().c_str());
    }
    new_material.set_refractive_index(refractive_index);

    float transparency = 0.0f;
    error = SAA_materialGetTransparency(scene, saa_material, &transparency);
    if (error != SI_ERR_NONE) {
        printf("ERROR: Failed to get transparency for material %s!\n", new_material.get_name().c_str());
    }
    new_material.set_transparency(transparency);

    if (static_blur != FALSE) {
        float blur_decay = 0.0f;
        error = SAA_materialGetBlurDecay(scene, saa_material, &blur_decay);
        if (error != SI_ERR_NONE) {
            printf("ERROR: Failed to get blur decay for material %s!\n", new_material.get_name().c_str());
        }
        new_material.set_blur_decay(blur_decay);

        float blur_width = 0.0f;
        error = SAA_materialGetBlurWidth(scene, saa_material, &blur_width);
        if (error != SI_ERR_NONE) {
            printf("ERROR: Failed to get blur width for material %s!\n", new_material.get_name().c_str());
        }
        new_material.set_blur_width(blur_width);
    }
    
    return new_material;
}

Element *HandleMesh(SAA_Scene* scene, SAA_Elem* model, Element* last_element = nullptr, Element* last_joint = nullptr) {
    void *rel_info = nullptr;

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
        char *name = nullptr;
        if (use_prefix) {
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

    // Process all of our shapes.
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

    // Model is not a null and has no triangles!
    if (!num_tri && verbose >= 1) {
        fprintf(log_file, "No triangles!\n");
    }

    // Allocate array of triangles and copy them all into it.
    SAA_SubElem *triangles = new SAA_SubElem[num_tri];
    if (triangles != nullptr) {
        // Triangulate model and read the triangles into array.
        SAA_modelGetTriangles(scene, model, gtype, id, num_tri, triangles);
    } else {
        fprintf(log_file, "Not enough memory for triangles! Aborting!\n");
        safe_exit(1);
    }

    // Allocate array of materials for each triangle and copy them all into it.
    SAA_Elem *materials = new SAA_Elem[num_tri];
    if (materials != nullptr) {
        // Read each triangle's material into array.
        SAA_triangleGetMaterials(scene, model, num_tri, triangles, materials);
    } else {
        fprintf(log_file, "Not enough memory for materials! Aborting!\n");
        safe_exit(1);
    }
    
    // Remove as many duplicate materials as possible. We'll then 
    // remove duplicates from the vector.
    size_t num_materials = num_tri;
    size_t *materials_index_map = RemoveDuplicates(scene, materials, num_materials);

    // Find out how many local textures per material.
    for (size_t i = 0; i < num_materials; i++) {
        int num_active_textures = 0;
        int num_passive_textures = 0;

        error = SAA_materialRelationGetT2DLocNbElements(scene, &materials[i], TRUE, (const void **)&rel_info, &num_active_textures);
        error = SAA_materialRelationGetT2DLocNbElements(scene, &materials[i], FALSE, (const void **)&rel_info, &num_passive_textures);
        if (error == SI_ERR_NONE) {
            num_tex_loc += num_passive_textures;
        }
    }
    
    lfprintf(log_file, "num_tex_loc = %d\n", 1, num_tex_loc);

    // Get local textures if present
    SAA_Elem *textures = nullptr;
    if (num_tex_loc) {
        // Add all of the materials to the global list,
        // If already present, Don't add it again.
        for (size_t i = 0; i < num_materials; i++) {
            SAA_Elem &saa_material = materials[i];
            scene_materials.push_back(InitializeMaterial(scene, &saa_material));
        }
    
        // ASSUME only one texture per material.
        // This behavior might be incorrect but it can be fixed later.
        textures = new SAA_Elem[num_materials];

        SAA_Boolean valid;
        for (size_t i = 0; i < num_materials; i++) {
            // Read all referenced local textures into array.
            SAA_materialRelationGetT2DLocElements(scene, &materials[i], TEX_PER_MAT, &textures[i]);
            
            // Check to see if texture is present
            SAA_elementIsValid(scene, &textures[i], &valid);
            if (!valid) { continue; }
            
            Material &material = scene_materials[i];
            lprintf("Material %d: %s\n", 1, i, material.get_name().c_str());
            
            material.set_texture_count(1);
            Texture *matTextures = material.prepare_textures();
            Texture &matTexture = matTextures[0];
            
            const char *filepath = GetTextureFilepath(scene, &textures[0]);
            if (filepath) {
                matTexture.set_filepath(std::string(filepath));
                //delete[] filepath;
            } else {
                printf("ERROR: Couldn't get filepath for texture in Material %s\n", material.get_name().c_str());
                matTexture.set_filepath("MISSING");
            }
            
            // Get arays of texture info.
            int32_t u_repeat;
            int32_t v_repeat;
            float u_scale;
            float v_scale;
            float u_offset;
            float v_offset;
            float transparency;
            
            SAA_Boolean uv_wrap = FALSE;

            SAA_texture2DGetUScale(scene, &textures[i], &u_scale);
            SAA_texture2DGetVScale(scene, &textures[i], &v_scale);
            SAA_texture2DGetUOffset(scene, &textures[i], &u_offset);
            SAA_texture2DGetVOffset(scene, &textures[i], &v_offset);
            SAA_texture2DGetRepeats(scene, &textures[i], &u_repeat, &v_repeat);
            SAA_texture2DGetTransparency(scene, &textures[i], &transparency );
            SAA_texture2DGetUVSwap(scene, &textures[i], &uv_swap);
            SAA_texture2DGetUVWrapping(scene, &textures[i], &uv_wrap);
            
            matTexture.set_u_scale(u_scale);
            matTexture.set_v_scale(v_scale);
            matTexture.set_u_offset(u_offset);
            matTexture.set_v_offset(v_offset);
            matTexture.set_u_repeat(u_repeat);
            matTexture.set_v_repeat(v_repeat);
            matTexture.set_transparency(transparency);
            matTexture.set_uv_swap(uv_swap == TRUE);
            matTexture.set_uv_wrap(uv_wrap == TRUE);
            
            new_element->mesh_info->set_material_id(i);

            if (verbose >= 1) {
                fprintf(log_file, " Local Texture: %s\n", matTexture.get_filepath().c_str());
                fprintf(log_file, " uScale: %f vScale: %f\n", u_scale, v_scale);
                fprintf(log_file, " uOffset: %f vOffset: %f\n", u_offset, v_offset);
                fprintf(log_file, " uRepeat = %d, vRepeat = %d\n", u_repeat, v_repeat);
                fprintf(log_file, " uvSwap = %d, uvWrap = %d\n", uv_swap, uv_wrap);
            }
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
                fprintf(log_file, "num_tex_glb = %d\n", num_tex_glb);
            }
            
            // Check to see if texture is present
            SAA_Boolean valid;
            SAA_elementIsValid(scene, &textures[0], &valid);
            
            // texture present - get the name and uv info
            if (valid) {
                SAA_Elem scene_material;
                SAA_modelRelationGetMatCurrent(scene, model, &scene_material);
                
                Material material = InitializeMaterial(scene, &scene_material);
                const char *textureName = GetTextureName(scene, &textures[0]);
                if (textureName) {
                    material.set_name(textureName);
                    delete[] textureName;
                } else {
                    material.set_name("NO NAME");
                }
                
                material.set_texture_count(1);
                Texture *matTextures = material.prepare_textures();
                Texture &matTexture = matTextures[0];
                
                const char *filepath = GetTextureFilepath(scene, &textures[0]);
                if (filepath) {
                    matTexture.set_filepath(std::string(filepath));
                    delete[] filepath;
                } else {
                    printf("ERROR: Couldn't get filepath for texture in Material %s\n", material.get_name().c_str());
                    matTexture.set_filepath("MISSING");
                }

                //lfprintf(log_file, " global tex named: %s\n", 1, texture_names[0]);

                // Get arays of texture info.
                int32_t u_repeat;
                int32_t v_repeat;
                float u_scale;
                float v_scale;
                float u_offset;
                float v_offset;
                float transparency;
                
                SAA_Boolean uv_wrap = FALSE;

                SAA_texture2DGetUScale(scene, &textures[0], &u_scale);
                SAA_texture2DGetVScale(scene, &textures[0], &v_scale);
                SAA_texture2DGetUOffset(scene, &textures[0], &u_offset);
                SAA_texture2DGetVOffset(scene, &textures[0], &v_offset);
                SAA_texture2DGetRepeats(scene, &textures[0], &u_repeat, &v_repeat);
                SAA_texture2DGetTransparency(scene, &textures[0], &transparency );
                SAA_texture2DGetUVSwap(scene, &textures[0], &uv_swap);
                SAA_texture2DGetUVWrapping(scene, &textures[0], &uv_wrap);
                
                matTexture.set_u_scale(u_scale);
                matTexture.set_v_scale(v_scale);
                matTexture.set_u_offset(u_offset);
                matTexture.set_v_offset(v_offset);
                matTexture.set_u_repeat(u_repeat);
                matTexture.set_v_repeat(v_repeat);
                matTexture.set_transparency(transparency);
                matTexture.set_uv_swap(uv_swap == TRUE);
                matTexture.set_uv_wrap(uv_wrap == TRUE);
                
                new_element->mesh_info->set_material_id(scene_materials.size());
                scene_materials.push_back(material);

                if (verbose >= 1) {
                    fprintf(log_file, " Global Texture: %s\n", matTexture.get_filepath().c_str());
                    fprintf(log_file, " uScale: %f vScale: %f\n", u_scale, v_scale);
                    fprintf(log_file, " uOffset: %f vOffset: %f\n", u_offset, v_offset);
                    fprintf(log_file, " uRepeat = %d, vRepeat = %d\n", u_repeat, v_repeat);
                    fprintf(log_file, " uvSwap = %d, uvWrap = %d\n", uv_swap, uv_wrap);
                }
            }
        }
    }

    // Allocate array of control vertices
    SAA_SubElem *cvertices = new SAA_SubElem[num_tri * 3];
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
    int *indices = new_element->mesh_info->get_indicies();
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
    Vector4d *vertices = new_element->mesh_info->get_vertices();

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
    int *index_map = MakeIndexMap(indices, num_tri * 3, num_vert);

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
        new_element->mesh_info->prepare_uv_coords(num_tex_loc * num_tri * 3);

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
    } else if (num_tex_glb) {
        dprintf("DEBUG: Processing global textures (%d) for Element <%s>.\n", num_tex_glb, new_element->get_name().c_str());

        SAA_Boolean valid;

        // Check to see if texture is present
        SAA_elementIsValid(scene, textures, &valid);

        // texture present - get the name and uv info
        if (valid) {
            // Allocate arrays for u & v coords and texture info
            new_element->mesh_info->prepare_uv_coords(num_tex_glb * num_tri * 3);

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
        } else {
            fprintf(log_file, "Invalid texture...\n");
        }
    }

    // We're done - Trash triangles.
    SAA_modelClearTriangles(scene, model);

    HandleElementChildren(scene, model, new_element, last_joint);

    delete[] triangles;
    delete[] materials;
    if (textures) { delete[] textures; }
    delete[] cvertices;

    return new_element;
}


Element *HandleJoint(SAA_Scene *scene, SAA_Elem *model, Element *last_element = nullptr, Element *last_joint = nullptr) {
    // We are litteraly a fucking joint. Assert if this test fails.
    //SAA_Boolean is_skeleton = FALSE;
    //SAA_modelIsSkeleton(scene, model, &is_skeleton);
    //assert(is_skeleton != FALSE);
    
    // Allocate and pre-initalize the Element.
    Element *new_element = new Element;
    memset(new_element, 0, sizeof(Element));

    // Make the name assignment a local affair.
    {
        // Get the name of the element.
        char *name = nullptr;
        if (use_prefix) {
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
        fprintf(log_file, "Joint Matrix = %f %f %f %f\n", matrix.mat[0][0], matrix.mat[0][1], matrix.mat[0][2], matrix.mat[0][3]);
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
        fprintf(log_file, "Joint Pos = %f %f %f\n", pos.x, pos.y, pos.z);
        fprintf(log_file, "Joint Rot = %f %f %f\n", rot.x, rot.y, rot.z);
        fprintf(log_file, "Joint Scale = %f %f %f\n", scale.x, scale.y, scale.z);
    }

    SAA_Boolean visible = FALSE;
    SAA_modelGetNodeVisibility(scene, model, &visible);
    new_element->set_visibility(visible != FALSE);
    lfprintf(log_file, "Visibility: %d\n", 1, visible);
    
    // We're a joint!
    new_element->set_as_joint(true);

    // Make sure we set the last node as our previous.
    new_element->prepare_joint_linklist_node();
    JointLinkListNode *node = new_element->get_joint_linklist_node();
    node->set_previous(last_joint);

    // Make sure we increment the amount of next nodes in the previous joint.
    if (last_joint) {
        JointLinkListNode *prev_node = last_joint->get_joint_linklist_node();
        prev_node->set_next_count(prev_node->get_next_count() + 1);
    } else {
        root_joints.push_back(new_element);
    }

    // We are now the new previous joint!
    last_joint = new_element;

    HandleElementChildren(scene, model, new_element, last_joint);

    return new_element;
}

void SetupJointLinkedList(Element *element, Element *last_joint = nullptr) {
    uint32_t children_count = element->get_children_amount();
    JointLinkListNode *node = nullptr;
    if (element->is_joint()) {
        node = element->get_joint_linklist_node();
        node->prepare_next();
        node->prepare_next_indicies();

        last_joint = element;
    } else if (last_joint != nullptr) {
        node = last_joint->get_joint_linklist_node();
    }
    assert(node != nullptr);

    uint32_t j = 0;
    for (uint32_t i = 0; i < children_count; i++) {
        Element *child = element->get_child(i);
        if (!child->is_joint()) { 
            SetupJointLinkedList(child, last_joint);
            continue;
        }

        if (j >= node->get_next_count()) { break; }

        Element **next = node->get_next();
        int64_t *next_indicies = node->get_next_indicies();
        next[j] = child;
        next_indicies[j] = child->get_array_position();

        SetupJointLinkedList(child, last_joint);
        ++j;
    }
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
            element = HandleJoint(scene, model, last_element, last_joint);
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
        if (use_prefix) {
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
    
    // Handle of these seperately from the elements.
    // They can overlap with each other if take Elements into
    // consideration.
    for (int i = 0; i < num_models; i++) {
        CreateConstraints(scene, &models[i]);
    }

    // First setup all of element indicies. This is setup for resolving
    // parents and children when written. 
    for (size_t i = 0; i < elements.size(); i++) {
        Element *element = elements[i];
        assert(element != nullptr);
        element->prepare_children_indicies();
        element->set_array_position(i);
    }

    // Create the arrays for element indicies in constraints.
    for (size_t i = 0; i < constraints.size(); i++) {
        // Get our current constraint.
        Constraint *constraint = constraints[i];
        assert(constraint != nullptr);

        // Set the passive element array index for our constraint.
        Element *passive_element = constraint->get_passive_element();
        assert(passive_element != nullptr);
        constraint->set_passive_element_index(passive_element->get_array_position());

        // Prepare the array for all of the active element array indicies.
        constraint->prepare_active_element_indicies();

        // Get all of our active elements for the constraint.
        Element **active_elements = constraint->get_active_elements();
        assert(active_elements != nullptr);

        // Get the array of active element indicies. 
        SIZE *active_element_indicies = constraint->get_active_element_indicies();
        assert(active_element_indicies != nullptr);

        // Set the active element index from all of our active elements.
        for (uint32_t j = 0; j < constraint->get_active_elements_count(); j++) {
            active_element_indicies[j] = active_elements[j]->get_array_position();
        }
    }

    // Setup the children positions and the parent position for all of the children.
    for (size_t i = 0; i < elements.size(); i++) {
        Element *element = elements[i];
        assert(element != nullptr);
        uint32_t children_count = element->get_children_amount();
        int64_t *chidren_indexes = element->get_children_indicies();
        for (uint32_t j = 0; j < children_count; j++) {
            Element *child = element->get_child(j);
            assert(child != nullptr);
            child->set_parent_position(element->get_array_position());
            chidren_indexes[j] = child->get_array_position();
        }
    }

    // Setup the linked lists of our joint elements.
    for (size_t i = 0; i < root_joints.size(); i++) {
        Element *root_joint = root_joints[i];
        assert(root_joint != nullptr);
        SetupJointLinkedList(root_joint);
    }

    // Open our binary file for writing.
    std::string filename(scene_name);
    {
        std::string file_ext(".siscene");
        filename += file_ext;
    }
    dprintf("Writing scene '%s' to file '%s'.\n", scene_name, filename.c_str());
    CompressedBinaryFile file(filename.c_str());

    // Write the name of our scene.
    file.write_uint64(strlen(scene_name));
    file.write(scene_name);

    // Write out all of our elements.
    file.write_uint64(elements.size());
    for (size_t i = 0; i < elements.size(); i++) {
        Element *element = elements[i];
        assert(element != nullptr);
        element->write(file);
    }

    // Write out all of our constraints.
    file.write_uint64(constraints.size());
    for (size_t i = 0; i < constraints.size(); i++) {
        Constraint *constraint = constraints[i];
        assert(constraint != nullptr);
        constraint->write(file);
    }
    
    // Write out all of our materials.
    file.write_uint64(scene_materials.size());
    for (size_t i = 0; i < scene_materials.size(); i++) {
        Material &material = scene_materials[i];
        material.write(file);
    }

    // Free all of our elements.
    for (size_t i = 0; i < elements.size(); i++) {
        Element *element = elements[i];
        elements[i] = nullptr;
        delete element;
    }
    // Clear the elements.
    elements.clear();

    // Free all of our constraints.
    for (size_t i = 0; i < constraints.size(); i++) {
        Constraint *constraint = constraints[i];
        constraints[i] = nullptr;
        delete constraint;
    }
    // Clear the constraints.
    constraints.clear();
    
    // Clear all of our scene materials.
    scene_materials.clear();

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