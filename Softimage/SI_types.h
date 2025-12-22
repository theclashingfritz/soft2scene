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

typedef struct
{
   float r, g, b;
}  SAA_ColorRGB;

typedef struct
{
   float r, g, b, a;
}  SAA_ColorRGBA;

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

enum SI_Texture2dFlags: uint8_t {
  None         = 0,
  Interpolate  = 1 << 0,
  Displacement = 1 << 1,
  UVSwap       = 1 << 2,
  UVWrap       = 1 << 3,
  AltH         = 1 << 4,
  AltV         = 1 << 5
};

typedef struct
{
  char *prefix;
  char *name;
  uint32_t prefix_len;
  uint32_t name_len;
  int32_t revision;
  int32_t wireframecol;
  SAA_WireType wiretype;
  SAA_ChapterType chapter;
  
  // Debugging only variables.
  int id;
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
  uint32_t trackname_len;
  uint32_t num_keys;
  SAA_FcurveInterpType interp;
  SAA_FcurveExtrapType preextrap;
  SAA_FcurveExtrapType postextrap;
  bool active;
} SI_FCurve;

typedef struct : SI_Element
{
  char *filepath;
  uint32_t filepath_len;
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
  bool interpolate;
  bool displacement; // For Mental Ray only.
  bool uv_swap;
  bool uv_wrap;
  bool alth;
  bool altv;
} SI_Texture2d;

typedef struct : SI_Element
{
  SAA_ColorRGBA color0;
  SAA_ColorRGBA color1;
  SAA_ColorRGBA color2;
  SAA_ColorRGBA color3;
  SAA_ColorRGBA color4;
  SI_Vector3f translation;
  SI_Vector3f rotation;
  SI_Vector3f scaling;
  SI_Vector3f factor;
  float ambient;
  float blending;
  float diffuse;
  float power;
  float reflection;
  float roughness;
  float strength;
  float spacing;
  float tapering;
  float transparency;
  int32_t iteration;
  SAA_MaskingType masking;
} SI_Texture3d;

/* 
  Material

  Materials only store local textures. If you want textures from the global pool.
  That is stored with the model instead.
 */
typedef struct : SI_Element
{
  SI_Texture2d *active_tex2d;
  SI_Texture2d *passive_tex2d;
  SI_Texture3d *active_tex3d;
  SI_Texture3d *passive_tex3d;
  uint32_t num_active_tex2d;
  uint32_t num_passive_tex2d;
  uint32_t num_active_tex3d;
  uint32_t num_passive_tex3d;
  SAA_ColorRGB ambieance;
  SAA_ColorRGB diffuse;
  SAA_ColorRGB specular;
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
  SI_Element *elements;
  SI_Texture2d *textures2d;
  SI_Texture3d *textures3d;
  SI_Material *materials;
  SI_FCurve *fcurves;
  SI_Constraint *constraints;
  uint32_t prefix_len;
  uint32_t name_len;
  uint32_t num_elements;
  uint32_t num_textures2d;
  uint32_t num_textures3d;
  uint32_t num_materials;
  uint32_t num_fcurves;
  uint32_t num_constraints;
  SAA_ScalingType scaling;
} SI_Scene;

#endif // SI_TYPES