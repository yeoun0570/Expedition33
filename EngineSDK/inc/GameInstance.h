#pragma once

#include "Graphic_Device.h"
#include "Input_Device.h"

#include "Texture.h"
#include "Transform.h"

#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Model_Manager.h"
#include "Prototype_Manager.h"
#include "Object_Manager.h"
#include "Sampler_Manager.h"


#include "Renderer.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Mesh.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "Pipeline.h"

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
	HRESULT Draw(const float4_t* pClearColor);
	HRESULT Draw_End();

	float2_t Get_ViewPortSize() const
	{
		return m_vViewPortSize;
	};

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
	HRESULT	Add_Model(uint32_t iModelLevelIndex, const wstring_t& strModelTag, const char* pFilePath);
	HRESULT Assemble_Model(const shared_ptr<CModel>& pSourceModel, const map<wstring_t, shared_ptr<CModel>>& PartModels, shared_ptr<CModel>& pOut);
	HRESULT Add_Animation(uint32_t iLevelIndex, const wstring_t& strAnimTag, const char* pFilePath);
	HRESULT Set_Animation(uint32_t iLevelIndex, const shared_ptr<CModel>& pModel, const wstring_t& strAnimTag);
	HRESULT Play_Animation(const shared_ptr<CModel>& pModel, f64_t fTimeDelta);
#pragma endregion

#pragma region Prototype_Manager
	HRESULT Add_Prototype(uint32_t iPrototypeLevelIndex, const wstring_t& strPrototypeTag, shared_ptr<class CPrototype> pPrototype);
	shared_ptr<CPrototype> Clone_Prototype(uint32_t iPrototypeLevelIndex, const wstring_t& strPrototypeTag, void* pArg = nullptr);
#pragma endregion

#pragma region Obejct_Manager
	HRESULT Add_GameObject(uint32_t iPrototypeLevelIndex, const wstring_t& strPrototypeTag, uint32_t iLayerLevelIndex, const wstring_t& strLayerTag, void* pArg = nullptr);
#pragma endregion

#pragma region Renderer
	HRESULT Add_RenderGroup(RENDERGROUP eGroup, shared_ptr<class CGameObject> pRenderObject);
#pragma endregion
#pragma region Pipeline
	const float4x4_t* Get_Transform(D3DTS eTransformState) const;
	const float4x4_t* Get_OrthoTransform(D3DTS eTransformState) const;
	const float4x4_t* Get_Transform_Inverse(D3DTS eTransformState) const;
	const float4x4_t* Get_OrthoTransform_Inverse(D3DTS eTransformState) const;
	const float4_t* Get_CamPosition() const;
	void Set_Transform(D3DTS eTransformState, fmatrix_t TransformMatrix);
	void Set_OrthoTransform(D3DTS eTransformState, fmatrix_t TransformMatrix);
#pragma endregion
#pragma region Sampler_Manager
	HRESULT Add_SamplerState(const wstring_t& strSamplerTag, const D3D11_SAMPLER_DESC& SamplerDesc);
	HRESULT Bind_PixelShader_Sampler(const wstring_t& strSamplerStateTag, uint32_t iSlot);
#pragma endregion

#pragma region INPUT_DEVICE
	int8_t	Get_DIKeyState(uint8_t byKeyID);
	int8_t	Get_DIMouseState(DIMK eMouse);
	int32_t Get_DIMouseMove(DIMM eMouseState);
#pragma endregion


private:
	float2_t			m_vViewPortSize;

private:
	unique_ptr<CGraphic_Device>				m_pGraphic_Device = { nullptr };
	unique_ptr<CInput_Device>				m_pInput_Device = { nullptr };

	unique_ptr<CTimer_Manager>				m_pTimer_Manager = { nullptr };
	unique_ptr<CLevel_Manager>				m_pLevel_Manager = { nullptr };
	unique_ptr<CPrototype_Manager>			m_pPrototype_Manager = { nullptr };
	unique_ptr<CObject_Manager>				m_pObject_Manager = { nullptr };
	unique_ptr<CRenderer>					m_pRenderer = { nullptr };
	unique_ptr<CPipeline>					m_pPipeline = { nullptr };

	unique_ptr<CModel_Manager>				m_pModel_Manager = { nullptr };

	unique_ptr<CSamplerState_Manager>		m_pSamplerState_Manager = { nullptr };

public:
	void Release_Engine();
	
};

NS_END