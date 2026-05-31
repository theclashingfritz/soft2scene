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
SI_Error Element_SAA2SI(SAA_Scene *scene, SAA_Elem *elem, SI_Element &si_elem);
SI_Error Texture2d_SAA2SI(SAA_Scene *scene, SAA_Elem *elem, SI_Texture2d &tex);
SI_Error Texture3d_SAA2SI(SAA_Scene *scene, SAA_Elem *elem, SI_Texture3d &tex);
SI_Error Material_SAA2SI(SAA_Scene *scene, SAA_Elem *elem, SI_Material &mat);
SI_Error Model_SAA2SI(SI_Scene &scene, SAA_Elem *elem, SI_Model &mdl);

SI_Error PosCons_SAA2SI(SAA_Scene *scene, SAA_Elem *elem, SI_Constraint &cns);
SI_Error OriCons_SAA2SI(SAA_Scene *scene, SAA_Elem *elem, SI_Constraint &cns);
SI_Error SclCons_SAA2SI(SAA_Scene *scene, SAA_Elem *elem, SI_Constraint &cns);
SI_Error PosLimCons_SAA2SI(SAA_Scene *scene, SAA_Elem *elem, SI_PositionLimitConstraint &cns);
SI_Error RotLimCons_SAA2SI(SAA_Scene *scene, SAA_Elem *elem, SI_RotationLimitConstraint &cns);
SI_Error VctCons_SAA2SI(SAA_Scene *scene, SAA_Elem *elem, SI_Constraint &cns);

SI_Error FCurve_SAA2SI(SAA_Scene *scene, SAA_Elem *fcurve, SI_FCurve &curv);
SI_Error Expression_SAA2SI(SAA_Scene *scene, SAA_Elem *elem, SI_Expression &express);

SI_Error Scene_SAA2SI(SI_Scene &scene);

/* Functions to write the structures to a file. */

SI_Error SI_WriteElement(SI_Element &elem, BinaryFile *file);
SI_Error SI_WriteTexture2d(SI_Texture2d &tex, BinaryFile *file);
SI_Error SI_WriteTexture3d(SI_Texture3d &tex, BinaryFile *file);

SI_Error SI_WriteFCurve(SI_FCurve &curv, BinaryFile *file);
SI_Error SI_WriteExpression(SI_Expression &expression, BinaryFile *file);

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