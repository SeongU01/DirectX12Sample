cbuffer FrameConstants : register(b0)
{
    row_major float4x4 View;
    row_major float4x4 Projection;
};

cbuffer DrawConstants : register(b1)
{
    row_major float4x4 World;
    float4 Tint;
};

struct VertexInput
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD;
};

struct PixelInput
{
    float4 Position : SV_POSITION;
};

PixelInput VSMain(VertexInput input)
{
    PixelInput output;
    const float4 worldPosition = mul(input.Position, World);
    const float4 viewPosition = mul(worldPosition, View);
    output.Position = mul(viewPosition, Projection);
    return output;
}

float4 PSMain(PixelInput input) : SV_TARGET
{
    return Tint;
}
