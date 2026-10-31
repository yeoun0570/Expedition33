#pragma once

#include "Graphic_Device.h"

#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Model_Manager.h"

#include "Shader.h"
#include "TriCol.h"

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
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, _Out_ ComPtr<ID3D11Device>& pDevice, _Out_ ComPtr<ID3D11DeviceContext>& pContext, const wstring_t& strFilePath);
	void	Update_Engine(f32_t fTimeDelta);

	HRESULT Draw_Begin(const float4_t* pClearColor);
	HRESULT Draw(const float4_t* pClearColor, const wstring_t& strModelTag);
	HRESULT Draw_End();

	HRESULT Clear_Resources(uint32_t iClearLevelIndex);

public:
#pragma region Timer_Manager
	// Timer
	f32_t Compute_TimeDelta(const wstring_t& strTimerTag);
	HRESULT	Add_Timer(const wstring_t& strTimerTag);
#pragma endregion

#pragma region Level_Manager
	// Level
	HRESULT Change_Level(uint32_t iLevelIndex, shared_ptr<class CLevel> pNewLevel);
#pragma endregion

#pragma region Model_Manager
	HRESULT	Add_Model(const wstring_t& strModelTag, const char* pFilePath);
	HRESULT	Render_Model(const wstring_t& strModelTag);
#pragma endregion

#pragma region Shader
	HRESULT		Update_Transform(const TRANSFORM& Transform);
#pragma endregion


private:
	unique_ptr<CTimer_Manager>				m_pTimer_Manager = { nullptr };
	unique_ptr<CLevel_Manager>				m_pLevel_Manager = { nullptr };
	unique_ptr<CGraphic_Device>				m_pGraphic_Device = { nullptr };

	unique_ptr<CShader>						m_pShader = { nullptr };
	unique_ptr<CTriCol>						m_pTriCol = { nullptr };
	unique_ptr<CModel_Manager>				m_pModel_Manager = { nullptr };

public:
	void Release_Engine();
	
};

NS_END