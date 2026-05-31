#include "SI_utils.h"

#include <assert.h>
#include <cstring>

int32_t SI_Chapter2ElementList(SI_Scene &scene, SAA_ChapterType chapter, SI_Element **list, uint32_t *num_elements) {
    if (!list || !num_elements) { return -1; }
    
    // Make sure these are initalized.
    // Undefined behavior is no fun!
    *list = NULL;
    *num_elements = 0;
    
    switch (chapter) {
        case SAA_CHP_MODEL:
            *list = scene.models.data();
            *num_elements = scene.models.size();
            break;
        case SAA_CHP_MATERIAL:
            *list = scene.materials.data();
            *num_elements = scene.materials.size();
            break;
        case SAA_CHP_TEXTURE_2D:
            *list = scene.textures2d.data();
            *num_elements = scene.textures2d.size();
            break;
        case SAA_CHP_TEXTURE_3D:
            *list = scene.textures3d.data();
            *num_elements = scene.textures3d.size();
            break;
        case SAA_CHP_EXPRESSION:
            *list = scene.expressions.data();
            *num_elements = scene.expressions.size();
            break;
        case SAA_CHP_CUSTOM_FCURVE:
        case SAA_CHP_MATERIAL_MRSHADER_FCURVE:
        case SAA_CHP_SHADOW_MRSHADER_FCURVE:
        case SAA_CHP_VOLUME_MRSHADER_FCURVE:
        case SAA_CHP_TEXTURE_2D_MRSHADER_FCURVE:
        case SAA_CHP_TEXTURE_3D_MRSHADER_FCURVE:
        case SAA_CHP_LIGHT_MRSHADER_FCURVE:
        case SAA_CHP_LENS_MRSHADER_FCURVE:
        case SAA_CHP_OUTPUT_MRSHADER_FCURVE:
        case SAA_CHP_ENVIRONMENT_MRSHADER_FCURVE:
        case SAA_CHP_MATERIAL_PHOTON_MRSHADER_FCURVE:
        case SAA_CHP_LIGHT_PHOTON_MRSHADER_FCURVE:
        case SAA_CHP_ACTION_FCURVE:
            *list = scene.fcurves.data();
            *num_elements = scene.fcurves.size();
            break;
        case SAA_CHP_SCENE:
        case SAA_CHP_ANIMATION:
        case SAA_CHP_CAMERA:
        case SAA_CHP_CLUSTER:
        case SAA_CHP_PICTURE:
        case SAA_CHP_LATTICE:
        case SAA_CHP_LIGHT:
        case SAA_CHP_NAMED_SELECTION:
        case SAA_CHP_PALETTE:
        case SAA_CHP_RENDERED_PICTURE:
        case SAA_CHP_SETUP_SOFT:
        case SAA_CHP_SETUP_CHANNEL:
        case SAA_CHP_SETUP_FLOCK:
        case SAA_CHP_SETUP_SHAPE:
        case SAA_CHP_STRUCT_KEY:
        case SAA_CHP_WAVE:
        case SAA_CHP_MATERIAL_MRSHADER:
        case SAA_CHP_SHADOW_MRSHADER:
        case SAA_CHP_VOLUME_MRSHADER:
        case SAA_CHP_TEXTURE_2D_MRSHADER:
        case SAA_CHP_TEXTURE_3D_MRSHADER:
        case SAA_CHP_LIGHT_MRSHADER:
        case SAA_CHP_LENS_MRSHADER:
        case SAA_CHP_OUTPUT_MRSHADER:
        case SAA_CHP_ENVIRONMENT_MRSHADER:
        case SAA_CHP_MATERIAL_PHOTON_MRSHADER:
        case SAA_CHP_LIGHT_PHOTON_MRSHADER:
        case SAA_CHP_CHANNEL_SETUP:
        case SAA_CHP_AUDIOTRACK:
        default:
            return -1;
    }
    
    return 0;
}

int32_t SI_CompareElements(SI_Element &a, SI_Element &b) {
    if (a.name == nullptr || a.prefix == nullptr) { return -1; }
    if (b.name == nullptr || b.prefix == nullptr) { return -1; }
    
    if (a.chapter != b.chapter) { return 1; }
    if (strcmp(a.name, b.name) != 0) { return 2; }
    if (strcmp(a.prefix, b.prefix) != 0) { return 3; }
    if (a.revision != b.revision) { return 4; }
    
    // Matching!
    return 0;
}

int32_t SI_CompareElements(SAA_Scene *scene, SAA_Elem *elem, SI_Element &b) {
    SI_Element a;
    SI_Error error = Element_SAA2SI(scene, elem, a);
    if (error != SI_SUCCESS) { return -1; }
    
    int32_t result = SI_CompareElements(a, b);
    
    error = SI_CleanupElement(a);
    if (error != SI_SUCCESS) { return -1; }
    
    return result;
}

int32_t SI_HasSAAElement(SI_Scene &scene, SAA_Elem *saa_elem, SI_Element **elem) {
    if (!saa_elem || !elem) { return -1; }
    
    SI_Element temp_elem;
    SI_Error error = Element_SAA2SI(scene.saa_scene, saa_elem, temp_elem);
    if (error != SI_SUCCESS) { return -1; }
    
    // Make sure this is initalized.
    // Undefined behavior is no fun!
    *elem = NULL;
    
    SI_Element *list = NULL;
    uint32_t num_elements = 0;
    if (SI_Chapter2ElementList(scene, temp_elem.chapter, &list, &num_elements) != 0) {
        printf("ERROR: Element %s in chapter %d is not supported in conversion tool! This WILL cause errors!\n", temp_elem.name, temp_elem.chapter);
        SI_CleanupElement(temp_elem); 
        return 0; 
    }
    
    int32_t found = 0;
    for (uint32_t i = 0; i < num_elements; i++) {
        if (SI_CompareElements(temp_elem, list[i]) != 0) { continue; }
        
        *elem = &list[i];
        found = 1;
        break;
    }

    if (found != 1) {
        printf("WARNING: Failed to find element %s in our scene! This may cause the saved scene to have errors!\n", temp_elem.name);
    }
    
    SI_CleanupElement(temp_elem);
    
    return found;
}