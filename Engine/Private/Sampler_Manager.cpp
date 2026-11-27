#include "Sampler_Manager.h"

#include "SamplerState.h"

CSamplerState_Manager::CSamplerState_Manager(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
	: m_pDevice { pDevice },
	m_pContext { pContext }
{
}
HRESULT CSamplerState_Manager::Initialize()
{
	if (FAILED(Add_SamplerState(g_strLinearSamplerTag, g_LinearSampler)))
		return E_FAIL;

	if (FAILED(Add_SamplerState(g_strPointSamplerTag, g_PointSampler)))
		return E_FAIL;

    return S_OK;
}

HRESULT CSamplerState_Manager::Add_SamplerState(const wstring_t& strSamplerStateTag, const D3D11_SAMPLER_DESC& SamplerDesc)
{
	if (nullptr != Find_SamplerState(strSamplerStateTag))
		return E_FAIL;

	auto		pSamplerState = CSamplerState::Create(m_pDevice, m_pContext, SamplerDesc);
	if (nullptr == pSamplerState)
		return E_FAIL;

	m_SamplerStates.emplace(strSamplerStateTag, pSamplerState);

    return S_OK;
}

HRESULT CSamplerState_Manager::Bind_PixelShader(const wstring_t& strSamplerStateTag, uint32_t iSlot)
{
	auto	pSamplerState = Find_SamplerState(strSamplerStateTag);
	if (nullptr == pSamplerState)
		return E_FAIL;

	pSamplerState->Bind_PixelShader(iSlot);

    return S_OK;
}

shared_ptr<class CSamplerState> CSamplerState_Manager::Find_SamplerState(const wstring_t& strSamplerStateTag)
{
	auto	iter = m_SamplerStates.find(strSamplerStateTag);
	if (iter == m_SamplerStates.end())
		return nullptr;

	return iter->second;
}

unique_ptr<CSamplerState_Manager> CSamplerState_Manager::Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
{
	auto    pInstance = unique_ptr<CSamplerState_Manager>(new CSamplerState_Manager(pDevice, pContext));

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CSamplerState_Manager");
		pInstance.reset();
	}

	return pInstance;
}
