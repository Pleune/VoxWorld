#ifndef CUSTOMMATH_H
#define CUSTOMMATH_H

typedef struct {
	int x;
	int y;
	int z;
} int3_t;

typedef struct {
	long x;
	long y;
	long z;
} long3_t;

typedef struct {
	float x;
	float y;
} vec2_t;

typedef struct {
	float x;
	float y;
	float z;
} vec3_t;

typedef struct {
       float x;
       float y;
       float z;
       float w;
} vec4_t;

typedef struct mat3_s {
       float mat[9];
} mat3_t;

typedef struct mat4_s {
	float mat[16];
} mat4_t;

long double *distlong3(long double *out, const long3_t *a, const long3_t *b);

//vector stuff
vec3_t *normalvec3(vec3_t *out, const vec3_t *in);

vec3_t *crossvec3(vec3_t *out, const vec3_t *a, const vec3_t *b);

//dot products
mat4_t *dotmat4mat4(mat4_t *out, const mat4_t *a, const mat4_t *b);

//more specific transformation functions for use with opengl (mat4 / vec4 only)
mat4_t gettranslatematrix(float x, float y, float z);

mat4_t getrotmatrix(float rx, float ry, float rz);
mat4_t getprojectionmatrix(float fov, float aspect, float far, float near);
mat4_t getviewmatrix(vec3_t eye, vec3_t target, vec3_t up);//lookat right-handed

#endif
