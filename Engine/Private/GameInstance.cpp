#include "GameInstance.h"

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, _Out_ ComPtr<ID3D11Device>& pDevice, _Out_ ComPtr<ID3D11DeviceContext>& pContext, const wstring_t& strFilePath)
{
    m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWinMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, pDevice, pContext);
    if (nullptr == m_pGraphic_Device)
        return E_FAIL;

    m_pTimer_Manager = CTimer_Manager::Create();
    if (nullptr == m_pTimer_Manager)
        return E_FAIL;

    m_pLevel_Manager = CLevel_Manager::Create();
    if (nullptr == m_pLevel_Manager)
        return E_FAIL;

    m_pShader = CShader::Create(pDevice, pContext, strFilePath);
    if (nullptr == m_pShader)
        return E_FAIL;

    m_pTriCol = CTriCol::Create(pDevice, pContext);
    if (nullptr == m_pTriCol)
        return E_FAIL;

    m_pModel_Manager = CModel_Manager::Create(pDevice, pContext);
    if (m_pModel_Manager == nullptr)
        return E_FAIL;

    m_pPrototype_Manager = CPrototype_Manager::Create(EngineDesc.iNumLevels);
    if (m_pPrototype_Manager == nullptr)
        return E_FAIL;

    m_pObject_Manager = CObject_Manager::Create(EngineDesc.iNumLevels);
    if (m_pObject_Manager == nullptr)
        return E_FAIL;

    m_pRenderer = CRenderer::Create(pDevice, pContext);
    if (m_pRenderer == nullptr)
        return E_FAIL;

    return S_OK;
}

void CGameInstance::Update_Engine(f32_t fTimeDelta)
{
    m_pObject_Manager->Priority_Update(fTimeDelta);
    m_pObject_Manager->Update(fTimeDelta);
    m_pObject_Manager->Late_Update(fTimeDelta);

    m_pLevel_Manager->Update(fTimeDelta);
}

HRESULT CGameInstance::Clear_Resources(uint32_t iClearLevelIndex)
{
    m_pObject_Manager->Clear(iClearLevelIndex);

    m_pPrototype_Manager->Clear(iClearLevelIndex);

    return S_OK;
}

#pragma region DRAW

HRESULT CGameInstance::Draw_Begin(const float4_t* pClearColor)
{
    if (FAILED(m_pGraphic_Device->Clear_BackBuffer_View(pClearColor)))
        return E_FAIL;
    if (FAILED(m_pGraphic_Device->Clear_DepthStencil_View()))
        return E_FAIL;

    return S_OK;
}

HRESULT CGameInstance::Draw(const float4_t* pClearColor)
{
    // 1. 화면 지우기
    if (FAILED(Draw_Begin(pClearColor)))
        return E_FAIL;

    if (FAILED(m_pRenderer->Draw()))
        return E_FAIL;

    if (FAILED(m_pLevel_Manager->Render()))
        return E_FAIL;

    if (FAILED(Draw_End()))
        return E_FAIL;

    return S_OK;
}

HRESULT CGameInstance::Draw_End()
{
    if (FAILED(m_pGraphic_Device->Present()))
        return E_FAIL;

    return S_OK;
}

#pragma endregion

#pragma region Timer_Manager

f32_t CGameInstance::Compute_TimeDelta(const wstring_t& strTimerTag)
{
    if (m_pTimer_Manager == nullptr)
        return 0.f;

    return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const wstring_t& strTimerTag)
{
    if (m_pTimer_Manager == nullptr)
        return E_FAIL;

    return m_pTimer_Manager->Add_Timer(strTimerTag);
}

#pragma endregion

#pragma region Level_Manager

HRESULT CGameInstance::Change_Level(uint32_t iLevelIndex, shared_ptr<class CLevel> pNewLevel)
{
    return m_pLevel_Manager->Change_Level(iLevelIndex, pNewLevel);
}

#pragma endregion

#pragma region Model_Manager
HRESULT CGameInstance::Add_Model(const wstring_t& strModelTag, const char* pFilePath)
{
    if (nullptr == m_pModel_Manager)
        return E_FAIL;

    return m_pModel_Manager->Add_Model(strModelTag, pFilePath);
}

HRESULT CGameInstance::Render_Model(const wstring_t& strModelTag)
{
    if (nullptr == m_pModel_Manager)
        return E_FAIL;

    // 셰이더와 입력 레이아웃 연결
    m_pShader->Bind_Shader();

    return m_pModel_Manager->Render_Model(strModelTag);
}
#pragma endregion

#pragma region Shader
HRESULT CGameInstance::Update_Transform(const TRANSFORM& Transform)
{
    if (nullptr == m_pShader)
        return E_FAIL;

    return m_pShader->Update_Transform(Transform);
}
#pragma endregion


#pragma region PROTOTYPE_MANAGER
HRESULT CGameInstance::Add_Prototype(uint32_t iPrototypeLevelIndex, const wstring_t& strPrototypeTag, shared_ptr<class CPrototype> pPrototype)
{
    return m_pPrototype_Manager->Add_Prototype(iPrototypeLevelIndex, strPrototypeTag, pPrototype);
}

shared_ptr<CPrototype> CGameInstance::Clone_Prototype(uint32_t iPrototypeLevelIndex, const wstring_t& strPrototypeTag, void* pArg)
{
    return m_pPrototype_Manager->Clone_Prototype(iPrototypeLevelIndex, strPrototypeTag, pArg);
}
#pragma endregion
#pragma region OBJECT_MANAGER
HRESULT CGameInstance::Add_GameObject(uint32_t iPrototypeLevelIndex, const wstring_t& strPrototypeTag, uint32_t iLayerLevelIndex, const wstring_t& strLayerTag, void* pArg)
{
    return m_pObject_Manager->Add_GameObject(iPrototypeLevelIndex, strPrototypeTag, iLayerLevelIndex, strLayerTag, pArg);
}

#pragma endregion

#pragma region Renderer
HRESULT CGameInstance::Add_RenderGroup(RENDERGROUP eGroup, shared_ptr<class CGameObject> pRenderObject)
{
    return m_pRenderer->Add_RenderGroup(eGroup, pRenderObject);
}
#pragma endregion

void CGameInstance::Release_Engine()
{
    m_pTimer_Manager.reset();
    m_pLevel_Manager.reset();
    m_pModel_Manager.reset();
    m_pPrototype_Manager.reset();
    m_pObject_Manager.reset();
    m_pRenderer.reset();
    
    m_pShader.reset();
    m_pTriCol.reset();

    m_pGraphic_Device.reset();
}
