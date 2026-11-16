cbuffer Freq_Object : register(b0)
{
    row_major matrix g_WorldMatrix;
};

cbuffer Freq_Frame : register(b1)
{
    row_major matrix g_ViewMatrix, g_ProjMatrix;
};


struct Vertex_In
{
    float3 vPosition : POSTION;
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