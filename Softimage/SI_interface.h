#ifndef SI_INTERFACE
#define SI_INTERFACE

#include <stdint.h>
#include <SAA.h>
#include "..\types.h"

SI_Error SI_GetElement(SAA_Scene *scene, SAA_Elem *elem, SI_Element &si_elem);

SI_Error SI_GetPositionConstraint(SAA_Scene *scene, SAA_Elem *elem, SI_Constraint &cns);
SI_Error SI_GetOrientationConstraint(SAA_Scene *scene, SAA_Elem *elem, SI_Constraint &cns);
SI_Error SI_GetScaleConstraint(SAA_Scene *scene, SAA_Elem *elem, SI_Constraint &cns);
SI_Error SI_GetPositionLimitsConstraint(SAA_Scene *scene, SAA_Elem *elem, SI_PositionLimitConstraint &cns);
SI_Error SI_GetRotationLimitsConstraint(SAA_Scene *scene, SAA_Elem *elem, SI_RotationLimitConstraint &cns);
SI_Error SI_GetUpVctConstraint(SAA_Scene *scene, SAA_Elem *elem, SI_Constraint &cns);

SI_Error SI_GetFCurve(SAA_Scene *scene, SAA_Elem *fcurve, SI_FCurve &curv);

#endif // SI_INTERFACE