#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPipeline final
{
private:
	CPipeline(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);

public:
	virtual ~CPipeline() = default;

public:
	const float4x4_t* Get_Transform(D3DTS eTransformState) const
	{
		return &m_PipelineDesc.TransformMatrix[ETOUI(eTransformState)];
	}

	const float4x4_t* Get_OrthoTransform(D3DTS eTransformState) const
	{
		return &m_PipelineDesc.OrthoTransformMatrix[ETOUI(eTransformState)];
	}

	const float4x4_t* Get_Transform_Inverse(D3DTS eTransformState) const
	{
		return &m_PipelineDesc.TransformMatrix_Inverse[ETOUI(eTransformState)];
	}

	const float4x4_t* Get_OrthoTransform_Inverse(D3DTS eTransformState) const
	{
		return &m_PipelineDesc.OrthoTransformMatrix_Inverse[ETOUI(eTransformState)];
	}

	const float4_t* Get_CamPosition() const
	{
		return &m_PipelineDesc.vCamPostion;
	}

	void Set_Transform(D3DTS eTransformState, fmatrix_t TransformMatrix)
	{
		XMStoreFloat4x4(&m_PipelineDesc.TransformMatrix[ETOUI(eTransformState)], TransformMatrix);
	}

	void Set_OrthoTransform(D3DTS eTransformState, fmatrix_t TransformMatrix)
	{
		XMStoreFloat4x4(&m_PipelineDesc.OrthoTransformMatrix[ETOUI(eTransformState)], TransformMatrix);
	}

public:
	HRESULT Initialize();
	void Update();
	HRESULT Bind_VertexShader(uint32_t iSlot);
	HRESULT Bind_PixelShader(uint32_t iSlot);


private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

	PIPELINE_DESC						m_PipelineDesc{};
	shared_ptr<class CConstantBuffer>	m_pCB = { nullptr };

public:
	static unique_ptr<CPipeline> Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);
};

NS_END