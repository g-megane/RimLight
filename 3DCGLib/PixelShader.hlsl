struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 PosW : POSITION0;
    float4 NorW : NORMAL0;
};



cbuffer ConstantBuffer : register(b0)
{
    float4 eyePos;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float rim = 1.0 - clamp(dot(input.NorW, eyePos), 0.0, 1.0);

    return float4(1.0, 1.0, 1.0, 1.0) * rim;
}
