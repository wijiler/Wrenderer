#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/math.h>

#ifdef _MSC_VER
#include <intrin.h>
#endif
#include <immintrin.h>

#ifdef _MSC_VER

#define CHECKSIMDSUPPORT                                                   \
    bool SSE_SUPPORTED = false;                                            \
    bool AVX_SUPPORTED = false;                                            \
    bool AVX512_SUPPORTED = false;                                         \
    {                                                                      \
        int regs[4] = {0};                                                 \
        __cpuidex(regs, 7, 0);                                             \
        bool avx2Support = (regs[1] & (1 << 5)) != 0;                      \
        bool avx512Support = (regs[1] & (1 << 16)) != 0;                   \
        __cpuid(regs, 1);                                                  \
        SSE_SUPPORTED = (regs[2] & (1 << 20)) != 0;                        \
        bool avxSupport = (regs[2] & (1 << 28)) != 0;                      \
        bool XSAVE = (regs[2] & (1 << 27)) != 0;                           \
        if (avx2Support && XSAVE && avxSupport)                            \
        {                                                                  \
            uint64_t xcrFeatureMask = _xgetbv(_XCR_XFEATURE_ENABLED_MASK); \
            AVX_SUPPORTED = (xcrFeatureMask & 0x6) == 0x6;                 \
            if (avx512Support)                                             \
                AVX512_SUPPORTED = (xcrFeatureMask & 0xE6) == 0xE6;        \
        }                                                                  \
    }

#endif

void mat4x4Translate(vec3 translation, mat4x4 *matrix)
{
    matrix->_14 = translation.x;
    matrix->_24 = translation.y;
    matrix->_34 = translation.z;
    matrix->_44 = 1;
}

void mat4x4Scale(vec3 scale, mat4x4 *matrix)
{
    matrix->_11 = scale.x;
    matrix->_22 = scale.y;
    matrix->_33 = scale.z;
    matrix->_44 = 1;
}

mat4x4 mat4x4RotateQuat(Quat rotation)
{
    /* TODO SIMD? */
    float realSquared = rotation.x * rotation.x;
    float xSquared = rotation.y * rotation.y;
    float ySquared = rotation.z * rotation.z;
    float zSquared = rotation.w * rotation.w;

    float q0 = rotation.x;
    float q1 = rotation.y;
    float q2 = rotation.z;
    float q3 = rotation.w;

    mat4x4 rotMatrix = {2 * (realSquared + xSquared) - 1, 2 * (q1 * q2 - q0 * q3), 2 * (q1 * q3 + q0 * q2), 0,
                        2 * (q1 * q2 + q0 * q3), 2 * (realSquared + ySquared) - 1, 2 * (q2 * q3 - q0 * q1), 0,
                        2 * (q1 * q3 - q0 * q2), 2 * (q2 * q3 + q0 * q1), 2 * (realSquared + zSquared) - 1, 0,
                        0, 0, 0, 1};
    return rotMatrix;
}

Quat eulerToQuaternion(vec3 angles)
{
    float roll = angles.x;
    float pitch = angles.y;
    float yaw = angles.z;

    float sinRoll = sinf(roll / 2);
    float cosRoll = cosf(roll / 2);

    float sinPitch = sinf(pitch / 2);
    float cosPitch = cosf(pitch / 2);

    float sinYaw = sinf(yaw / 2);
    float cosYaw = cosf(yaw / 2);

    Quat quat = {
        sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw,
        cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw,
        cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw,
        cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw,
    };
    return quat;
}

mat4x4 mat4x4Mul(mat4x4 a, mat4x4 b)
{
    CHECKSIMDSUPPORT

    if (SSE_SUPPORTED)
    {
        float resultMat[16] = {0};

        float a1[4] = {a._11, a._21, a._31, a._41};
        float a2[4] = {a._12, a._22, a._32, a._42};
        float a3[4] = {a._13, a._23, a._33, a._43};
        float a4[4] = {a._14, a._24, a._34, a._44};
        __m128 acol1 = _mm_loadu_ps(a1);
        __m128 acol2 = _mm_loadu_ps(a2);
        __m128 acol3 = _mm_loadu_ps(a3);
        __m128 acol4 = _mm_loadu_ps(a4);

        float b1[4] = {b._11, b._12, b._13, b._14};
        float b2[4] = {b._21, b._22, b._23, b._24};
        float b3[4] = {b._31, b._32, b._33, b._34};
        float b4[4] = {b._41, b._42, b._43, b._44};
        __m128 brow1 = _mm_loadu_ps(b1);
        __m128 brow2 = _mm_loadu_ps(b2);
        __m128 brow3 = _mm_loadu_ps(b3);
        __m128 brow4 = _mm_loadu_ps(b4);

        // I know this is ugly
        {
            __m128 resultLineOne = _mm_mul_ps(_mm_shuffle_ps(acol1, acol1, 0x00), brow1);
            resultLineOne = _mm_add_ps(resultLineOne, _mm_mul_ps(_mm_shuffle_ps(acol1, acol1, 0x55), brow2));
            resultLineOne = _mm_add_ps(resultLineOne, _mm_mul_ps(_mm_shuffle_ps(acol1, acol1, 0xaa), brow3));
            resultLineOne = _mm_add_ps(resultLineOne, _mm_mul_ps(_mm_shuffle_ps(acol1, acol1, 0xff), brow4));

            _mm_storeu_ps(&resultMat[0], resultLineOne);
        }
        {
            __m128 resultLineTwo = _mm_mul_ps(_mm_shuffle_ps(acol2, acol2, 0x00), brow1);
            resultLineTwo = _mm_add_ps(resultLineTwo, _mm_mul_ps(_mm_shuffle_ps(acol2, acol2, 0x55), brow2));
            resultLineTwo = _mm_add_ps(resultLineTwo, _mm_mul_ps(_mm_shuffle_ps(acol2, acol2, 0xaa), brow3));
            resultLineTwo = _mm_add_ps(resultLineTwo, _mm_mul_ps(_mm_shuffle_ps(acol2, acol2, 0xff), brow4));

            _mm_storeu_ps(&resultMat[4], resultLineTwo);
        }
        {
            __m128 resultLineThree = _mm_mul_ps(_mm_shuffle_ps(acol3, acol3, 0x00), brow1);
            resultLineThree = _mm_add_ps(resultLineThree, _mm_mul_ps(_mm_shuffle_ps(acol3, acol3, 0x55), brow2));
            resultLineThree = _mm_add_ps(resultLineThree, _mm_mul_ps(_mm_shuffle_ps(acol3, acol3, 0xaa), brow3));
            resultLineThree = _mm_add_ps(resultLineThree, _mm_mul_ps(_mm_shuffle_ps(acol3, acol3, 0xff), brow4));

            _mm_storeu_ps(&resultMat[8], resultLineThree);
        }
        {
            __m128 resultLineFour = _mm_mul_ps(_mm_shuffle_ps(acol4, acol4, 0x00), brow1);
            resultLineFour = _mm_add_ps(resultLineFour, _mm_mul_ps(_mm_shuffle_ps(acol4, acol4, 0x55), brow2));
            resultLineFour = _mm_add_ps(resultLineFour, _mm_mul_ps(_mm_shuffle_ps(acol4, acol4, 0xaa), brow3));
            resultLineFour = _mm_add_ps(resultLineFour, _mm_mul_ps(_mm_shuffle_ps(acol4, acol4, 0xff), brow4));

            _mm_storeu_ps(&resultMat[12], resultLineFour);
        }
        return (mat4x4){
            resultMat[0], resultMat[1], resultMat[2], resultMat[3],
            resultMat[4], resultMat[5], resultMat[6], resultMat[7],
            resultMat[8], resultMat[9], resultMat[10], resultMat[11],
            resultMat[12], resultMat[13], resultMat[14], resultMat[15]};
    }
    else
    {
        mat4x4 result = {0};

        result._11 = a._11 * b._11 + a._12 * b._21 + a._13 * b._31;
        result._21 = a._21 * b._11 + a._22 * b._21 + a._23 * b._31;
        result._31 = a._31 * b._11 + a._32 * b._21 + a._33 * b._31;
        result._41 = a._41 * b._11 + a._42 * b._21 + a._43 * b._31;

        result._12 = a._11 * b._12 + a._12 * b._22 + a._13 * b._32;
        result._22 = a._21 * b._12 + a._22 * b._22 + a._23 * b._32;
        result._32 = a._31 * b._12 + a._32 * b._22 + a._33 * b._32;
        result._42 = a._41 * b._12 + a._42 * b._22 + a._43 * b._32;

        result._13 = a._11 * b._13 + a._12 * b._23 + a._13 * b._33;
        result._23 = a._21 * b._13 + a._22 * b._23 + a._23 * b._33;
        result._33 = a._31 * b._13 + a._32 * b._23 + a._33 * b._33;
        result._43 = a._41 * b._13 + a._42 * b._23 + a._43 * b._33;

        result._14 = a._14;
        result._24 = a._24;
        result._34 = a._34;
        result._44 = a._44;
        return result;
    }
}

vec3 divV3V3(vec3 a, vec3 b)
{
    CHECKSIMDSUPPORT

    if (SSE_SUPPORTED)
    {
        vec4 a4 = {a.x, a.y, a.z, 0};
        __m128 vecA = _mm_loadu_ps((float *)&a4);

        vec4 b4 = {b.x, b.y, b.z, 0};
        __m128 vecB = _mm_loadu_ps((float *)&b4);

        vec4 result = {0};
        __m128 res = _mm_div_ps(vecA, vecB);
        _mm_storeu_ps((float *)&result, res);
        return (vec3){result.x, result.y, result.z};
    }
    else
    {
        return (vec3){a.x / b.x, a.y / b.y, a.z / b.z};
    }
}

vec3 divVec3Scalar(vec3 a, float b)
{
    CHECKSIMDSUPPORT

    if (SSE_SUPPORTED)
    {
        vec4 a4 = {a.x, a.y, a.z, 0};
        __m128 vecA = _mm_loadu_ps((float *)&a4);

        vec4 b4 = {b, b, b, 0};
        __m128 vecB = _mm_loadu_ps((float *)&b4);

        vec4 result = {0};
        __m128 res = _mm_div_ps(vecA, vecB);
        _mm_storeu_ps((float *)&result, res);
        return (vec3){result.x, result.y, result.z};
    }
    else
    {
        return (vec3){a.x / b, a.y / b, a.z / b};
    }
}

float vec3Len(vec3 in)
{
    CHECKSIMDSUPPORT

    if (SSE_SUPPORTED)
    {
        vec4 in4 = {in.x, in.y, in.z, 0};
        __m128 inVec = _mm_loadu_ps((float *)&in4);
        __m128 insqre = _mm_mul_ps(inVec, inVec);
        vec4 squared = {0};

        _mm_storeu_ps((float *)&squared, insqre);
        float scalar = sqrtf(squared.x + squared.y + squared.z);
        return scalar;
    }
    else
    {
        return sqrtf(in.x * in.x + in.y * in.y + in.z * in.z);
    }
}

vec3 subtractVec3(vec3 a, vec3 b)
{
    CHECKSIMDSUPPORT
    if (SSE_SUPPORTED)
    {
        vec4 a4 = {a.x, a.y, a.z, 0};
        __m128 vecA = _mm_loadu_ps((float *)&a4);

        vec4 b4 = {b.x, b.y, b.z, 0};
        __m128 vecB = _mm_loadu_ps((float *)&b4);
        vec4 result = {0};
        _mm_storeu_ps((float *)&result, _mm_sub_ps(vecA, vecB));
        return (vec3){result.x, result.y, result.z};
    }
    else
    {
        return (vec3){a.x - b.x, a.y - b.y, a.z - b.z};
    }
}

vec3 vec3Cross(vec3 a, vec3 b)
{
    CHECKSIMDSUPPORT

    if (AVX_SUPPORTED)
    {
        float acol1[8] = {a.y, a.z, a.x, a.z, a.x, a.y, 0, 0};
        float bcol1[8] = {b.z, b.x, b.y, b.y, b.z, b.x, 0, 0};
        __m256 a1 = _mm256_loadu_ps(acol1);
        __m256 b1 = _mm256_loadu_ps(bcol1);

        float mul[8] = {0};
        _mm256_storeu_ps(mul, _mm256_mul_ps(a1, b1));

        __m128 col1 = _mm_loadu_ps(&mul[0]);
        __m128 col2 = _mm_loadu_ps(&mul[3]);

        vec4 result = {0};
        _mm_storeu_ps((float *)&result, _mm_sub_ps(col1, col2));
        return (vec3){result.x, result.y, result.z};
    }
    if (SSE_SUPPORTED)
    {
        float acol1[4] = {a.y, a.z, a.x, 0}; // could we use the memory more efficiently here?
        float acol2[4] = {a.z, a.x, a.y, 0};
        float bcol1[4] = {b.z, b.x, b.y, 0};
        float bcol2[4] = {b.y, b.z, b.x, 0};
        __m128 a1 = _mm_loadu_ps(acol1);
        __m128 a2 = _mm_loadu_ps(acol2);
        __m128 b1 = _mm_loadu_ps(bcol1);
        __m128 b2 = _mm_loadu_ps(bcol2);

        __m128 res1 = _mm_mul_ps(a1, b1);
        __m128 res2 = _mm_mul_ps(a2, b2);

        vec4 result = {0};
        _mm_storeu_ps((float *)&result, _mm_sub_ps(res1, res2));
        return (vec3){result.x, result.y, result.z};
    }
    return (vec3){
        (a.y * b.z) - (a.z * b.y),
        (a.z * b.x) - (a.x * b.z),
        (a.x * b.y) - (a.y * b.x),
    };
}

vec3 normalizeVec3(vec3 in)
{
    float len = vec3Len(in);
    return divVec3Scalar(in, len);
}

// https://stackoverflow.com/questions/6996764/fastest-way-to-do-horizontal-sse-vector-sum-or-other-reduction ;)
float horizontalSum(__m128 v)
{
    __m128 shuf = _mm_movehdup_ps(v);
    __m128 sums = _mm_add_ps(v, shuf);
    shuf = _mm_movehl_ps(shuf, sums);
    sums = _mm_add_ss(sums, shuf);
    return _mm_cvtss_f32(sums);
}

float vec3Dot(vec3 a, vec3 b)
{
    CHECKSIMDSUPPORT

    if (SSE_SUPPORTED)
    {
        vec4 a4 = {a.x, a.y, a.z, 0};
        vec4 b4 = {b.x, b.y, b.z, 0};

        __m128 avec = _mm_loadu_ps((float *)&a4);
        __m128 bvec = _mm_loadu_ps((float *)&b4);

        return horizontalSum(_mm_mul_ps(avec, bvec));
    }
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

mat4x4 lookAtMatrix(vec3 pos, vec3 target, vec3 up)
{
    vec3 forward = normalizeVec3(subtractVec3(pos, target));

    vec3 right = normalizeVec3(vec3Cross(up, pos));

    vec3 newUp = vec3Cross(forward, right);

    return (mat4x4){
        right.x, newUp.x, forward.x, 0,
        right.y, newUp.y, forward.y, 0,
        right.z, newUp.z, forward.z, 0,
        -vec3Dot(right, pos), -vec3Dot(newUp, pos), -vec3Dot(forward, pos), 1};
}

mat4x4 fpsViewMatrix(vec3 pos, float pitch, float yaw)
{
    mat4x4 tmat = identity4x4;
    mat4x4Translate(pos, &tmat);
    mat4x4 rot = mat4x4RotateQuat(eulerToQuaternion((vec3){yaw, 0, pitch + PI32}));

    return mat4x4Mul(transposeMat4x4(tmat), rot);
}

mat4x4 orthoProjMatrix(float near, float far, float top, float bottom, float left, float right)
{
    return (mat4x4){2 / (right - left), 0, 0, 0,
                    0, 2 / (top - bottom), 0, 0,
                    0, 0, -1 / (far - near), 0,
                    -(right + left) / (right - left), -(top + bottom) / (top - bottom), -far / (far - near), 1};
}

mat4x4 transposeMat4x4(mat4x4 in)
{
    return (mat4x4){
        in._11, in._21, in._31, in._41,
        in._12, in._22, in._32, in._42,
        in._13, in._23, in._33, in._43,
        in._14, in._24, in._34, in._44};
}

// modified from https://www.vincentparizet.com/blog/posts/vulkan_perspective_matrix/
mat4x4 perspProjMatrix(float vertical_fov, float aspect_ratio, float n, float f)
{
    float fov_rad = vertical_fov * 2.0f * PI32 / 360.0f;
    float focal_length = 1.0f / tanf(fov_rad / 2.0f);

    float x = focal_length / aspect_ratio;
    float y = -focal_length;
    float A = n / (f - n);
    float B = f * A;

    return (mat4x4){
        x, 0.0f, 0.0f, 0.0f,
        0.0f, y, 0.0f, 0.0f,
        0.0f, 0.0f, A, B,
        0.0f, 0.0f, -1.0f, 0.0f};
}
