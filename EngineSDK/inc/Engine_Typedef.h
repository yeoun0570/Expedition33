#ifndef Engine_Typedef_h__
#define Engine_Typedef_h__

namespace Engine
{
	typedef		bool						bool_t;
	typedef		char						char_t;	
	typedef		wstring						wstring_t;

	typedef		float						f32_t;
	typedef		double						f64_t;

	// XM : DirectX 접두사
	// 해석하면 DX float형 2개,3개,4개, 즉 x,y,z,w 중 어디까지 갖고 있느냐에 따른 구조체
	typedef		XMFLOAT2					float2_t;
	typedef		XMFLOAT3					float3_t;
	typedef		XMFLOAT4					float4_t;

	typedef		XMFLOAT4X4					float4x4_t;	
}

#endif // Engine_Typedef_h__
