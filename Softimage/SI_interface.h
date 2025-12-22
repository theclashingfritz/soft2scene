#ifndef SI_INTERFACE
#define SI_INTERFACE

#include <stdint.h>
#include <SAA.h>

#include "..\binaryFile.h"
#include "..\types.h"

/* Functions to read the Softimage data into structures */

/*
 Processes the SAA element into the SI Element structure.
 
 Returns SI_ERR_BAD_ARGUMENT if the arguments are invalid.
 Returns SI_ERR_BAD_ELEMENT is the SAA element is invalid.
 Returns SI_SUCCESS on success.
 */
SI_Error SI_GetElement(SAA_Scene *scene, SAA_Elem *elem, SI_Element &si_elem);
SI_Error SI_GetTexture2d(SAA_Scene *scene, SAA_Elem *elem, SI_Texture2d &tex);
SI_Error SI_GetTexture3d(SAA_Scene *scene, SAA_Elem *elem, SI_Texture3d &tex);
SI_Error SI_GetMaterial(SAA_Scene *scene, SAA_Elem *elem, SI_Material &mat);

SI_Error SI_GetPositionConstraint(SAA_Scene *scene, SAA_Elem *elem, SI_Constraint &cns);
SI_Error SI_GetOrientationConstraint(SAA_Scene *scene, SAA_Elem *elem, SI_Constraint &cns);
SI_Error SI_GetScaleConstraint(SAA_Scene *scene, SAA_Elem *elem, SI_Constraint &cns);
SI_Error SI_GetPositionLimitsConstraint(SAA_Scene *scene, SAA_Elem *elem, SI_PositionLimitConstraint &cns);
SI_Error SI_GetRotationLimitsConstraint(SAA_Scene *scene, SAA_Elem *elem, SI_RotationLimitConstraint &cns);
SI_Error SI_GetUpVctConstraint(SAA_Scene *scene, SAA_Elem *elem, SI_Constraint &cns);

SI_Error SI_GetFCurve(SAA_Scene *scene, SAA_Elem *fcurve, SI_FCurve &curv);

SI_Error SI_GetScene(SAA_Scene *scene, SI_Scene &siscene);

/* Functions to write the structures to a file. */

SI_Error SI_WriteElement(SI_Element &elem, BinaryFile *file);
SI_Error SI_WriteTexture2d(SI_Texture2d &tex, BinaryFile *file);
SI_Error SI_WriteTexture3d(SI_Texture3d &tex, BinaryFile *file);

SI_Error SI_WriteFCurve(SI_FCurve &curv, BinaryFile *file);

SI_Error SI_WriteScene(SI_Scene &siscene, BinaryFile *file);

/* Functions to clear the structures. */

/*
 Frees the data from a processed element.
 
 Returns SI_SUCCESS on success.
 */
SI_Error SI_CleanupElement(SI_Element &elem);

/*
 Frees the data from a processed texture 2d element.
 
 Returns SI_SUCCESS on success.
 */
SI_Error SI_CleanupTexture2d(SI_Texture2d &tex);

#endif // SI_INTERFACE