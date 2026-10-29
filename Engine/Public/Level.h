#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL CLevel abstract
{
protected:
	CLevel(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext);

public:
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Update(f32_t fTimeDelta);
	virtual HRESULT Render();

protected:
	ComPtr<ID3D11Device>				m_pDevice;
	ComPtr<ID3D11DeviceContext>			m_pContext;
};

NS_END