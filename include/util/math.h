#ifndef WRE_MATH_H__
#define WRE_MATH_H__
typedef struct
{
    float x, y;
} vec2;

typedef struct
{
    float x, y, z;
} vec3;

typedef struct
{
    float x, y, z, w;
} vec4;

typedef vec4 Quat;

typedef struct
{
    vec3 origin;
    vec2 scale;
    float rotation;
} transform2D;

typedef struct
{
    vec3 origin;
    vec3 scale;
    Quat rotation;
} transform3D;
typedef struct
{
    float identity[16];
} mat4x4;
typedef struct
{
    float identity[12];
} mat3x4;
typedef struct
{
    float identity[8];
} mat2x4;
typedef struct
{
    float identity[9];
} mat3x3;
typedef struct
{
    float identity[6];
} mat2x3;

typedef struct
{
    float identity[4];
} mat2x2;

static const mat4x4 identity4x4 = {{1, 0, 0, 0,
                                    0, 1, 0, 0,
                                    0, 0, 1, 0,
                                    0, 0, 0, 1}};

#endif