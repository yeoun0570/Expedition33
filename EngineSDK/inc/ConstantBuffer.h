#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL CConstantBuffer final
{
private:
	CConstantBuffer(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext);

public:
	virtual ~CConstantBuffer() = default;

public:
	HRESULT Initialize(const void* pData, uint32_t iLength);
	HRESULT	Update(const void* pData, uint32_t iLength);
	HRESULT Bind_VertexShader(uint32_t iSlot);
	HRESULT Bind_PixelShader(uint32_t iSlot);


private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

	ComPtr<ID3D11Buffer>			m_pCB = { nullptr };

public:
	static shared_ptr<CConstantBuffer> Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext, const void* pData, uint32_t iLength);
};

NS_END