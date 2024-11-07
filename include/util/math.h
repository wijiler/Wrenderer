#ifndef WRE_MATH_H__
#define WRE_MATH_H__
typedef struct
{
    float x, y;
} Vector2;

typedef struct
{
    float x, y, z;
} Vector3;

typedef struct
{
    float x, y, z, w;
} Vector4;

typedef Vector4 Quat;

typedef struct
{
    Vector3 origin;
    Vector2 scale;
    float rotation;
} transform2D;

typedef struct
{
    Vector3 origin;
    Vector3 scale;
    Quat rotation;
} transform3D;

typedef float mat4x4[4][4];
typedef float mat3x4[3][4];
typedef float mat2x4[2][4];
typedef float mat4x3[4][3];
typedef float mat3x3[3][3];
typedef float mat2x3[2][3];
typedef float mat4x2[4][2];
typedef float mat3x2[3][2];
typedef float mat2x2[2][2];

#endif