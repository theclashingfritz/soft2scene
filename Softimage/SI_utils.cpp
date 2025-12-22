#include "SI_utils.h"

#include <assert.h>
#include <cstring>

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
    SI_Error error = SI_GetElement(scene, elem, a);
    if (error != SI_SUCCESS) { return -1; }
    
    int32_t result = SI_CompareElements(a, b);
    
    error = SI_CleanupElement(a);
    if (error != SI_SUCCESS) { return -1; }
    
    return result;
}