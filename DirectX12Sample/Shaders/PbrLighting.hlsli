#ifndef PBR_LIGHTING
#define PBR_LIGHTING
static const float PI = 3.14159265359;

float3 FresnelSchlick(float cosine, float3 f0)
{
    return f0 + (1 - f0) * pow(1 - saturate(cosine), 5);
}

float DistributionGGX(float nDotH, float roughness)
{
    float a2 = pow(roughness, 4);
    float denominator = nDotH * nDotH * (a2 - 1) + 1;
    return a2 / max(PI * denominator * denominator, 1e-12);
}

float GeometrySchlick(float nDotX, float roughness)
{
    float k = (roughness + 1) * (roughness + 1) / 8;
    return nDotX / max(nDotX * (1 - k) + k, 1e-5);
}

float3 EvaluateBRDF(float3 n, float3 v, float3 l, float3 albedo, float metallic, float roughness)
{
    float3 h = (v + l) / max(length(v + l), 1e-5);
    float nl = saturate(dot(n, l)), nv = saturate(dot(n, v));
    float3 f = FresnelSchlick(dot(v, h), lerp(0.04.xxx, albedo, metallic));
    float d = DistributionGGX(saturate(dot(n, h)), roughness);
    float g = GeometrySchlick(nv, roughness) * GeometrySchlick(nl, roughness);
    float3 specular = d * g * f / max(4 * nv * nl, 1e-5);
    return ((1 - f) * (1 - metallic) * albedo / PI + specular) * nl;
}

float3 LinearToSRGB(float3 value)
{
    return lerp(1.055 * pow(max(value, 0), 1.0 / 2.4) - 0.055, value * 12.92, value <= 0.0031308);
}
#endif
