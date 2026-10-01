#ifndef PBR_COMMON
#define PBR_COMMON
cbuffer FrameData : register(b0)
{
    row_major float4x4 View;
    row_major float4x4 Projection;
    float4 CameraExposure;
    float4 LightDirectionIntensity;
    float4 LightColorAmbient;
    float4 PointPositionIntensity;
    float4 PointColor;
    uint DebugMode;
    float3 FramePadding;
};
cbuffer ObjectData : register(b1)
{
    row_major float4x4 World;
    row_major float4x4 NormalMatrix;
    float4 BaseColor;
    float4 MaterialFactors;
    float2 UVScale;
    uint UseTextures;
    float ObjectPadding;
};
struct MeshToPixel
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float4 tangent : TEXCOORD2;
    float2 uv : TEXCOORD3;
};
#endif
