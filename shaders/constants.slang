static const int spriteScaleFactor = 100;
static const int worldScaleFactor = 100;

static const float4 spriteVPositions[6] =
    {
        float4(.1f, .1f, 0.0f, 1.0f),
        float4(.1f, -.1f, 0.0f, 1.0f),
        float4(-.1f, -.1f, 0.0f, 1.0f),
        float4(-.1f, -.1f, 0.0f, 1.0f),
        float4(-.1f, .1f, 0.0f, 1.0f),
        float4(.1f, .1f, 0.0f, 1.0f),
    };
float4 operator *(float4x4 matrix, float4 vec)
{
    return mul(matrix, vec);
}
float4 operator *(float4 vec, float4x4 matrix)
{
    return mul(vec, matrix);
}

float4x4 operator *(float4x4 matrixLeft, float4x4 matrixRight)
{
    return mul(matrixLeft, matrixRight);
}
