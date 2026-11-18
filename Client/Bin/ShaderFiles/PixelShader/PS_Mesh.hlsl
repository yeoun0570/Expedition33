texture2D g_Texture : register(t0);
sampler g_LinearSampler : register(s0);

struct Pixel_In
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
    float3 vNormal : NORMAL;
};

struct Pixel_Out
{
    float4 vColor : SV_TARGET0;
};

Pixel_Out main(Pixel_In In)
{
    Pixel_Out Out;
    
    // 확인용: 법선(-1~1)을 색(0~1)으로 바꿔서 출력
    float3 vNormal = normalize(In.vNormal);
    Out.vColor = float4(vNormal * 0.5f + 0.5f, 1.f);

    // 텍스처 연결 후 사용
    // Out.vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord);
    
    return Out;
}