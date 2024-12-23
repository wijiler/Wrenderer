#ifndef WRE_MATH_H__
#define WRE_MATH_H__

static const float PI32 = 3.14159265358979323846f;
static const double PI64 = 3.1415926535897932384626433832795028841971693993751058209749445923078164062L;
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
    float _11, _12, _13, _14, _21, _22, _23, _24, _31, _32, _33, _34, _41, _42, _43, _44;
} mat4x4;
typedef struct
{
    float _11, _12, _13, _14, _21, _22, _23, _24, _31, _32, _33, _34;
} mat3x4;
typedef struct
{
    float _11, _12, _13, _14, _21, _22, _23, _24;
} mat2x4;
typedef struct
{
    float _11, _12, _13, _21, _22, _23, _31, _32, _33;
} mat3x3;
typedef struct
{
    float _11, _12, _13, _21, _22, _23;
} mat2x3;

typedef struct
{
    float _11, _12, _21, _22;
} mat2x2;

static const mat4x4 identity4x4 = {1, 0, 0, 0,
                                   0, 1, 0, 0,
                                   0, 0, 1, 0,
                                   0, 0, 0, 1};
//
mat4x4 mat4x4Translate(vec3 translation, mat4x4 matrix);
mat4x4 mat4x4Scale(vec3 scale, mat4x4 matrix);
mat4x4 mat4x4RotateQuat(Quat rotation);
mat4x4 transposeMat4x4(mat4x4 in);
Quat eulerToQuaternion(vec3 angles);
mat4x4 mat4x4Mul(mat4x4 a, mat4x4 b);
mat4x4 mat4x4MulScalar(mat4x4 a, float b);
mat4x4 lookAtMatrix(vec3 pos, vec3 target, vec3 up);
mat4x4 fpsViewMatrix(vec3 pos, float pitch, float yaw);
mat4x4 orthoProjMatrix(float near, float far, float top, float bottom, float left, float right);
mat4x4 perspProjMatrix(float vertical_fov, float aspect_ratio, float n, float f);
//
vec3 divVec3Scalar(vec3 a, float b);
vec3 divV3V3(vec3 a, vec3 b);
vec3 subtractVec3(vec3 a, vec3 b);
float vec3Len(vec3 in);
vec3 normalizeVec3(vec3 in);
vec3 vec3Cross(vec3 a, vec3 b);
float vec3Dot(vec3 a, vec3 b);
vec3 Vec3Add(vec3 a, vec3 b);
//

#endif