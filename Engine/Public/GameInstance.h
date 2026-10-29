#pragma once

#include "Timer_Manager.h"
#include "Graphic_Device.h"
#include "FbxLoader.h"
#include "Shader.h"
#include "TriCol.h"
#include "MeshCol.h"

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
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, _Out_ ComPtr<ID3D11Device>& pDevice, _Out_ ComPtr<ID3D11DeviceContext>& pContext, const wstring_t& strFilePath, const MESH_DATA& MeshData);
	HRESULT Draw_Begin(const float4_t* pClearColor);
	HRESULT Draw(const float4_t* pClearColor);
	HRESULT Draw_End();


public:
	// Timer
	f32_t Compute_TimeDelta(const wstring_t& strTimerTag);
	HRESULT	Add_Timer(const wstring_t& strTimerTag);

	// Fbx Loader
	HRESULT Load_Model(const char* pFilePath);

private:
	unique_ptr<CTimer_Manager>				m_pTimer_Manager = { nullptr };
	unique_ptr<CGraphic_Device>				m_pGraphic_Device = { nullptr };
	unique_ptr<CFbxLoader>					m_pFbx_Loader = { nullptr };
	unique_ptr<CShader>						m_pShader = { nullptr };
	unique_ptr<CTriCol>						m_pTriCol = { nullptr };
	unique_ptr<CMeshCol>					m_pMeshCol = { nullptr };

public:
	void Release_Engine();
	
};

NS_END