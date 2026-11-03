#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class CRenderer final
{
private:
	CRenderer(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);

public:
	~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderGroup(RENDERGROUP eGroup, shared_ptr<class CGameObject> pRenderObject);
	HRESULT Draw();

private:
	ComPtr<ID3D11Device>					m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>				m_pContext = { nullptr };
	list<shared_ptr<class CGameObject>>		m_RenderObjects[ETOUI(RENDERGROUP::END)];

private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_UI();

public:
	static unique_ptr<CRenderer> Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);
};

NS_END