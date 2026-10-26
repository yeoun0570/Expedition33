#pragma once

#include "Timer_Manager.h"
#include "Graphic_Device.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameInstance final 
{
	DECLARE_SINGLETON(CGameInstance);

private:
	CGameInstance();
public:
	~CGameInstance() = default;

public:
	/* 엔진을 사용하기 위해 초기화과정을 수행한다. */
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, _Out_ ComPtr<ID3D11Device>& pDevice, _Out_ ComPtr<ID3D11DeviceContext>& pContext);
	HRESULT Draw_Begin(const float4_t* pClearColor);
	HRESULT Draw(const float4_t* pClearColor);
	HRESULT Draw_End();


public:
	f32_t Compute_TimeDelta(const wstring_t& strTimerTag);
	HRESULT	Add_Timer(const wstring_t& strTimerTag);

private:
	unique_ptr<CTimer_Manager>				m_pTimer_Manager = { nullptr };
	unique_ptr<CGraphic_Device>				m_pGraphic_Device = { nullptr };

public:
	void Release_Engine();
	
};

NS_END