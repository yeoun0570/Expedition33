#include "SamplerState.h"

CSamplerState::CSamplerState(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
	: m_pDevice { pDevice },
	m_pContext { pContext }
{
}

HRESULT CSamplerState::Initialize(const D3D11_SAMPLER_DESC& SamplerDesc)
{
	m_pDevice->CreateSamplerState(&SamplerDesc, &m_pSamplerState);

	return S_OK;
}

HRESULT CSamplerState::Bind_PixelShader(uint32_t iSlot)
{
	ID3D11SamplerState* pSamplerState[] =
	{
		m_pSamplerState.Get(),
	};

	m_pContext->PSSetSamplers(iSlot, 1, pSamplerState);

	return S_OK;
}


shared_ptr<CSamplerState> CSamplerState::Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext, const D3D11_SAMPLER_DESC& SamplerDesc)
{
	auto    pInstance = shared_ptr<CSamplerState>(new CSamplerState(pDevice, pContext));

	if (FAILED(pInstance->Initialize(SamplerDesc)))
	{
		MSG_BOX("Failed to Created : CSamplerState");
		pInstance.reset();
	}

	return pInstance;
}
