#pragma once

#include <stdint.h>


struct Matrix4f {
	float mat[4][4];
};

struct Vector3f {
	float x, y, z;
};

struct Vector3d {
	double x, y, z;
};

struct Vector4f {
	float x, y, z, w;
};

struct Vector4d {
	double x, y, z, w;
};