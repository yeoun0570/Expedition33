#include "../../../../EngineSDK/hlsl/Engine_PixelShader_Defines.hlsli"

texture2D g_Texture[2] : register(t0);

struct Pixel_In
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
};

struct Pixel_Out
{
    float4 vColor : SV_TARGET0;
};

Pixel_Out main(Pixel_In In)
{
    Pixel_Out Out;
    
    Out.vColor = g_Texture[1].Sample(g_LinearSampler, In.vTexcoord);
    
    return Out;
}