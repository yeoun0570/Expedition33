#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class CSamplerState final
{
private:
	CSamplerState(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);

public:
	~CSamplerState() = default;

public:
	HRESULT Initialize(const D3D11_SAMPLER_DESC& SamplerDesc);
	HRESULT Bind_PixelShader(uint32_t iSlot);

private:
	ComPtr<ID3D11Device>					m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>				m_pContext = { nullptr };

	ComPtr<ID3D11SamplerState>				m_pSamplerState = { nullptr };

public:
	static shared_ptr<CSamplerState>	Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext, const D3D11_SAMPLER_DESC& SamplerDesc);
};

NS_END