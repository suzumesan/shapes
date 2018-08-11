
struct VS_INPUT
{
	float3 Pos : POSITION;
};

cbuffer ObjectData : register(b0)
{
	float4x4 WVP;
};


float4 main( VS_INPUT input ) : SV_POSITION
{
	float4 pos = mul(float4(input.Pos, 1), WVP);
	return pos;
}