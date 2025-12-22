#ifndef TYPES
#define TYPES

#include <stdint.h>

#include <SAA.h>

#include "Softimage\SI_types.h"

#define SIZE uint64_t

union FourByteUnion {
  int32_t ival;
  uint32_t uval;
  float fval;
};

union EightByteUnion {
  int64_t ival;
  uint64_t uval;
  double fval;
};

#define Vector3f SI_Vector3f
#define Vector3d SI_Vector3d
#define Vector4f SI_Vector4f
#define Vector4d SI_Vector4d
#define Matrix4f SI_Matrix4f
#define Matrix4d SI_Matrix4d
#define ColorRGB SAA_ColorRGB
#define ColorRGBA SAA_ColorRGBA

#endif // TYPES