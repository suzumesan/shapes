#include "shared.hlsl"

struct VS_INPUT
{
    float3 Pos : POSITION;
};

cbuffer ObjectData : register(b1)
{
    float4x4 World;
};

float4 VSMain(VS_INPUT input) : SV_POSITION
{
    float4 pos = mul(float4(input.Pos, 1), mul(World, VP));
    return pos;
}

float4 PSMain(float4 pos : SV_POSITION) : SV_TARGET
{
    return float4(1, 0, 0, 1);
}