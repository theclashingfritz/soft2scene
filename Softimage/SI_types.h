#ifndef SI_TYPES
#define SI_TYPES

typedef struct
{
   float x, y, z;
}  SAA_Vector3;

typedef struct
{
   double x, y, z;
}  SAA_DVector3;

#define SI_Vector3f SAA_Vector3
#define SI_Vector3d SAA_DVector3
#define SI_Vector4f SAA_Vector
#define SI_Vector4d SAA_DVector
typedef float SI_Matrix4f[4][4];
typedef double SI_Matrix4d[4][4];

// It's reccomended to read the SI SDK docs to understand function curves.
// SI 4.0 includes a copy of its own SDK.
typedef struct
{
  float value;
  float time;
  float lslope;
  float rslope;
  SAA_FKeySlopeType slopetype;
  SAA_FcurveInterpType interp;
  SAA_FKeyType type;
} SI_FCurveKey;

enum SI_ConstraintType: uint8_t {
  INVALID,
  POSITION, // Constrains the position of the passive elements to the center of the active element.
  ORIENTATION, // Constrains the rotation of the passive elements to the rotation of the active element.
  SCALE,
  POSITION_LIMIT,
  ROTATION_LIMIT,
  UP_VCT,
};

typedef struct
{
  char *prefix;
  char *name;
  int32_t revision;
  int32_t wireframecol;
  SAA_WireType wiretype;
  SAA_ChapterType chapter;
} SI_Element;

typedef struct : SI_Element
{
  SAA_Elem passive_elem;
  SAA_Elem *active_elems;
  uint32_t num_active_elems;
  SI_ConstraintType type;
  bool active;
} SI_Constraint;

typedef struct : SI_Constraint
{
  SI_Vector3f min_pos;
  SI_Vector3f max_pos;
  float damping_width;
  float damping_strength;
  float radius;
  bool min_pos_active[3];
  bool max_pos_active[3];
  SAA_CnsPosLimType shape;
  SAA_CoordSys sys;
} SI_PositionLimitConstraint;

typedef struct : SI_Constraint
{
  SI_Vector3f min_hpr;
  SI_Vector3f max_hpr;
  float damping_width;
  float damping_strength;
  bool min_hpr_active[3];
  bool max_hpr_active[3];
} SI_RotationLimitConstraint;

typedef struct : SI_Element
{
  char *trackname;
  SI_FCurveKey *keys;
  uint32_t num_keys;
  SAA_FcurveInterpType interp;
  SAA_FcurveExtrapType preextrap;
  SAA_FcurveExtrapType postextrap;
  bool active;
} SI_FCurve;

typedef struct : SI_Element
{
  char *filepath;
  float transparency;
  float ambieance;
  float roughness;
  float rotation;
  float specular;
  float diffuse;
  float blend;
  float reflection; // Reflectivity
  float reflection_int; // Reflection Intensity
  float u_scale;
  float v_scale;
  float u_offset;
  float v_offset;
  int32_t u_repeat;
  int32_t v_repeat;
  int32_t u_crop_min;
  int32_t v_crop_min;
  int32_t u_crop_max;
  int32_t v_crop_max;
  SAA_AnimationType anim_type;
  SAA_MappingType mapping;
  SAA_MapComponent map_component;
  SAA_MaskingType masking;
  bool uv_swap;
  bool uv_wrap;
  bool interpolate;
  bool alth;
  bool altv;
  bool displacement; // For Mental Ray only.
} SI_Texture2d;

typedef struct : SI_Element
{
  SI_Texture2d *textures;
  uint32_t num_textures;
  SI_Vector3f ambieance;
  SI_Vector3f diffuse;
  SI_Vector3f specular;
  float specular_decay;
  float reflection;
  float refractive_index;
  float transparency;
  float blur_decay;
  float blur_width;
  SAA_ShadingModelType smodel;
  SAA_DiffuseSrcType diffuse_src;
  SAA_TransparencySrcType transparency_src;
  bool static_blur;
} SI_Material;

typedef struct
{
  char *prefix;
  char *name;
  SI_FCurve *fcurves;
  uint32_t num_fcurves;
} SI_Scene;

#endif // SI_TYPES