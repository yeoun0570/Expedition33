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

    return S_OK;
}

void CGameInstance::Update_Engine(f32_t fTimeDelta)
{
    m_pLevel_Manager->Update(fTimeDelta);
}

HRESULT CGameInstance::Clear_Resources(uint32_t iClearLevelIndex)
{
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

HRESULT CGameInstance::Draw(const float4_t* pClearColor, const wstring_t& strModelTag)
{
    // 1. 화면 지우기
    if (FAILED(Draw_Begin(pClearColor)))
        return E_FAIL;

    if (FAILED(m_pLevel_Manager->Render()))
        return E_FAIL;

    // 2. 셰이더와 입력 레이아웃 연결
    m_pShader->Bind_Shader();

    // 3. 정점·인덱스 버퍼 연결 후 그리기
    //m_pTriCol->Render_Buffer();
    if(FAILED(m_pModel_Manager->Render_Model(strModelTag)))
        return E_FAIL;

    // 4. 완성한 화면 표시
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


void CGameInstance::Release_Engine()
{
    m_pTimer_Manager.reset();
    m_pLevel_Manager.reset();
    m_pModel_Manager.reset();
    
    m_pShader.reset();
    m_pTriCol.reset();

    m_pGraphic_Device.reset();
}
