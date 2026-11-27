
cbuffer Freq_Object : register(b0)
{
    row_major matrix g_WorldMatrix;
};

cbuffer Freq_Frame : register(b1)
{
    row_major matrix g_OrthoViewMatrix, g_OrthoProjMatrix; // 직교 뷰/투영 (UI)
    row_major matrix g_OrthoViewInverse, g_OrthoProjInverse;
    row_major matrix g_ViewMatrix, g_ProjMatrix; // 원근 뷰/투영 (3D)
    row_major matrix g_ViewInverse, g_ProjInverse;
    float4 g_vCamPosition;
};