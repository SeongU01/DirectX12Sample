#include "PbrCommon.hlsli"

struct StaticMeshInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : TEXCOORD;
};

MeshToPixel StaticMeshVS(StaticMeshInput input)
{
    MeshToPixel output;
    float4 worldPosition = mul(float4(input.position, 1), World);
    output.worldPosition = worldPosition.xyz;
    output.position = mul(mul(worldPosition, View), Projection);
    output.normal = normalize(mul(input.normal, (float3x3)NormalMatrix));
    output.tangent.xyz = normalize(mul(input.tangent.xyz, (float3x3)World));
    output.tangent.w = input.tangent.w * (determinant((float3x3)World) < 0 ? -1 : 1);
    output.uv = input.uv * UVScale;
    return output;
}
