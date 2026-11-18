#include "Pipeline.h"

#include "ConstantBuffer.h"

CPipeline::CPipeline(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice { pDevice },
	m_pContext { pContext }
{
}

HRESULT CPipeline::Initialize()
{
	for (uint32_t i = 0; i < ETOUI(D3DTS::END); ++i)
	{
		XMStoreFloat4x4(&m_PipelineDesc.TransformMatrix[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_PipelineDesc.TransformMatrix_Inverse[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_PipelineDesc.OrthoTransformMatrix[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_PipelineDesc.OrthoTransformMatrix_Inverse[i], XMMatrixIdentity());
	}

	m_PipelineDesc.vCamPostion = float4_t(0.f, 0.f, 0.f, 1.f);

	m_pCB = CConstantBuffer::Create(m_pDevice, m_pContext, &m_PipelineDesc, sizeof m_PipelineDesc);
	if (nullptr == m_pCB)
		return E_FAIL;

	return S_OK;
}

void CPipeline::Update()
{
	for (uint32_t i = 0; i < ETOUI(D3DTS::END); ++i)
	{
		XMStoreFloat4x4(&m_PipelineDesc.OrthoTransformMatrix_Inverse[i],
			XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_PipelineDesc.OrthoTransformMatrix[i])));


		XMStoreFloat4x4(&m_PipelineDesc.TransformMatrix_Inverse[i],
			XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_PipelineDesc.TransformMatrix[i])));
	}

	memcpy(&m_PipelineDesc.vCamPostion, &m_PipelineDesc.TransformMatrix_Inverse[ETOUI(D3DTS::VIEW)].m[3], sizeof float4_t);
}

HRESULT CPipeline::Bind_VertexShader(uint32_t iSlot)
{
	if(FAILED(m_pCB->Update(&m_PipelineDesc, sizeof m_PipelineDesc)))
		return E_FAIL;

	m_pCB->Bind_VertexShader(iSlot);

	return S_OK;
}

HRESULT CPipeline::Bind_PixelShader(uint32_t iSlot)
{
	if (FAILED(m_pCB->Update(&m_PipelineDesc, sizeof m_PipelineDesc)))
		return E_FAIL;

	m_pCB->Bind_PixelShader(iSlot);

	return S_OK;
}

unique_ptr<CPipeline> CPipeline::Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext)
{
	auto    pInstance = unique_ptr<CPipeline>(new CPipeline(pDevice, pContext));

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPipeline");
		pInstance.reset();
	}

	return pInstance;
}
