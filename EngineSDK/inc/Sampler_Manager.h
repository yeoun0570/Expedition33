#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class CSamplerState_Manager final
{
private:
	CSamplerState_Manager(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);

public:
	~CSamplerState_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_SamplerState(const wstring_t& strSamplerStateTag, const D3D11_SAMPLER_DESC& SamplerDesc);
	HRESULT Bind_PixelShader(const wstring_t& strSamplerStateTag, uint32_t iSlot);

private:
	ComPtr<ID3D11Device>					m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>				m_pContext = { nullptr };

	map<wstring_t, shared_ptr<class CSamplerState>>		m_SamplerStates;

private:
	shared_ptr<class CSamplerState> Find_SamplerState(const wstring_t& strSamplerStateTag);

public:
	static unique_ptr<CSamplerState_Manager>	Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);
};

NS_END