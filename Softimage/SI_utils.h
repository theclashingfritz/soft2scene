#ifndef SI_UTILS
#define SI_UTILS

#include <stdint.h>
#include <SAA.h>

#include "SI_interface.h"
#include "..\types.h"

/* 
 Compares two SI elements to one another.
 
 Checks happen in incremental order. So if you get 4 as a result. You
 can assume everything but the revision matches.
 But if you get a result of 2. Only the chapters match.
 
 Returns -1 if there is a error or if an argument is invalid.
 Returns 0 if they are matching.
 Returns 1 if the chapters do not match.
 Returns 2 if the names do not match.
 Returns 3 if the prefix doesn't match.
 Returns 4 if the revision doesn't match.
 */
int32_t SI_CompareElements(SI_Element &a, SI_Element &b);

/* 
 Compares a SAA element and a SI element to one another.
 
 Checks happen in incremental order. So if you get 4 as a result. You
 can assume everything but the revision matches.
 But if you get a result of 2. Only the chapters match.
 
 Returns -1 if there is a error or if an argument is invalid.
 Returns 0 if they are matching.
 Returns 1 if the chapters do not match.
 Returns 2 if the names do not match.
 Returns 3 if the prefix doesn't match.
 Returns 4 if the revision doesn't match.
 */
int32_t SI_CompareElements(SAA_Scene *scene, SAA_Elem *elem, SI_Element &b);

#endif // SI_UTILS