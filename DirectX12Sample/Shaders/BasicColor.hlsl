cbuffer FrameConstants : register(b0)
{
    row_major float4x4 View;
    row_major float4x4 Projection;
};

cbuffer DrawConstants : register(b1)
{
    row_major float4x4 World;
    float4 Tint;
    uint Shading;
};

struct VertexInput
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD;
};

struct PixelInput
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : TEXCOORD0;
};

PixelInput VSMain(VertexInput input)
{
    PixelInput output;
    const float4 worldPosition = mul(input.Position, World);
    output.WorldPosition = worldPosition.xyz;
    const float4 viewPosition = mul(worldPosition, View);
    output.Position = mul(viewPosition, Projection);
    return output;
}

float4 PSMain(PixelInput input) : SV_TARGET
{
    // 선택적인 평면 음영: 도형 종류나 정점 포맷에 종속되지 않는 월드 공간 면 법선.
    if (Shading == 1)
    {
        const float3 normal = normalize(cross(ddx(input.WorldPosition), ddy(input.WorldPosition)));
        const float3 lightDirection = normalize(float3(-0.5, 0.8, -0.6));
        const float brightness = 0.3 + 0.7 * saturate(dot(normal, lightDirection));
        return float4(Tint.rgb * brightness, Tint.a);
    }
    return Tint;
}
