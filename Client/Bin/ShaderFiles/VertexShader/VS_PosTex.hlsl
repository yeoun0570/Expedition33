#include "../../../../EngineSDK/hlsl/Engine_VertexShader_Defines.hlsli"

struct Vertex_In
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct Vertex_Out
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
};


Vertex_Out main(Vertex_In In)
{
    Vertex_Out Out;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

Vertex_Out main_ortho(Vertex_In In)
{
    Vertex_Out Out;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_OrthoViewMatrix);
    matWVP = mul(matWV, g_OrthoProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}