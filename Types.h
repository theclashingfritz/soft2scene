#pragma once

#include <stdint.h>

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

enum CoordinateSystem : uint8_t {
	LOCAL,
	GLOBAL
};

struct Matrix4f {
	float mat[4][4];
};

struct Matrix4d {
	double mat[4][4];
};

struct Vector3b {
	bool x, y, z;
};

struct Vector3f {
	float x, y, z;
};

struct Vector3d {
	double x, y, z;
};

struct Vector4b {
	bool x, y, z, w;
};

struct Vector4f {
	float x, y, z, w;
};

struct Vector4d {
	double x, y, z, w;
};