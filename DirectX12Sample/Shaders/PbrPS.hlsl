#include "PbrCommon.hlsli"
#include "PbrLighting.hlsli"

Texture2D AlbedoMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D MetallicMap : register(t2);
Texture2D RoughnessMap : register(t3);
Texture2D AOMap : register(t4);
SamplerState MaterialSampler : register(s0);

float4 PbrPS(MeshToPixel input) : SV_TARGET
{
    float3 albedo = BaseColor.rgb;
    float metallic = MaterialFactors.x, roughness = MaterialFactors.y, ao = 1;
    float3 n = normalize(input.normal);
    if (UseTextures)
    {
        albedo *= AlbedoMap.Sample(MaterialSampler, input.uv).rgb;
        metallic *= MetallicMap.Sample(MaterialSampler, input.uv).r;
        roughness *= RoughnessMap.Sample(MaterialSampler, input.uv).r;
        ao = lerp(1, AOMap.Sample(MaterialSampler, input.uv).r, MaterialFactors.w);
        float3 t = normalize(input.tangent.xyz - n * dot(n, input.tangent.xyz));
        float3 b = cross(n, t) * input.tangent.w;
        float3 mapped = NormalMap.Sample(MaterialSampler, input.uv).xyz * 2 - 1;
        mapped.xy *= MaterialFactors.z;
        n = normalize(t * mapped.x + b * mapped.y + n * mapped.z);
    }
    metallic = saturate(metallic);
    roughness = clamp(roughness, 0.045, 1);
    if (DebugMode == 1) return float4(LinearToSRGB(albedo), 1);
    if (DebugMode == 2) return float4(n * 0.5 + 0.5, 1);
    if (DebugMode == 3) return float4(metallic.xxx, 1);
    if (DebugMode == 4) return float4(roughness.xxx, 1);
    if (DebugMode == 5) return float4(ao.xxx, 1);
    float3 v = normalize(CameraExposure.xyz - input.worldPosition);
    float3 l = -LightDirectionIntensity.xyz / max(length(LightDirectionIntensity.xyz), 1e-5);
    float3 color = EvaluateBRDF(n, v, l, albedo, metallic, roughness) *
                   LightColorAmbient.rgb * LightDirectionIntensity.w;
    float3 delta = PointPositionIntensity.xyz - input.worldPosition;
    float distanceSquared = max(dot(delta, delta), 0.01);
    color += EvaluateBRDF(n, v, delta / sqrt(distanceSquared), albedo, metallic, roughness) *
             PointColor.rgb * PointPositionIntensity.w / distanceSquared;
    color += albedo * (1 - metallic) * LightColorAmbient.w * ao;
    color *= CameraExposure.w;
    color = color / (1 + color);
    return float4(LinearToSRGB(color), 1);
}
