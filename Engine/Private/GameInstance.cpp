#include "GameInstance.h"

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, _Out_ ComPtr<ID3D11Device>& pDevice, _Out_ ComPtr<ID3D11DeviceContext>& pContext, const wstring_t& strFilePath)
{
    m_vViewPortSize = float2_t(EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);

    m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWinMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, pDevice, pContext);
    if (nullptr == m_pGraphic_Device)
        return E_FAIL;

    m_pInput_Device = CInput_Device::Create(EngineDesc.hInstance, EngineDesc.hWnd);
    if (nullptr == m_pInput_Device)
        return E_FAIL;

    m_pTimer_Manager = CTimer_Manager::Create();
    if (nullptr == m_pTimer_Manager)
        return E_FAIL;

    m_pLevel_Manager = CLevel_Manager::Create();
    if (nullptr == m_pLevel_Manager)
        return E_FAIL;

    m_pModel_Manager = CModel_Manager::Create(pDevice, pContext, EngineDesc.iNumLevels);
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

    m_pPipeline = CPipeline::Create(pDevice, pContext);
    if (m_pPipeline == nullptr)
        return E_FAIL;

    m_pSamplerState_Manager = CSamplerState_Manager::Create(pDevice, pContext);
    if (m_pSamplerState_Manager == nullptr)
        return E_FAIL;

    return S_OK;
}

void CGameInstance::Update_Engine(f32_t fTimeDelta)
{
    m_pInput_Device->Update();

    m_pObject_Manager->Priority_Update(fTimeDelta);
    m_pObject_Manager->Update(fTimeDelta);
    m_pObject_Manager->Late_Update(fTimeDelta);

    m_pLevel_Manager->Update(fTimeDelta);
}

HRESULT CGameInstance::Clear_Resources(uint32_t iClearLevelIndex)
{
    m_pObject_Manager->Clear(iClearLevelIndex);

    m_pPrototype_Manager->Clear(iClearLevelIndex);

    m_pModel_Manager->Clear(iClearLevelIndex);

    return S_OK;
}

#pragma region DRAW

HRESULT CGameInstance::Draw_Begin(const float4_t* pClearColor)
{
    if (FAILED(m_pGraphic_Device->Clear_BackBuffer_View(pClearColor)))
        return E_FAIL;
    if (FAILED(m_pGraphic_Device->Clear_DepthStencil_View()))
        return E_FAIL;

    if (FAILED(m_pPipeline->Bind_VertexShader(ETOUI(V_SLOT::FRAME))))
        return E_FAIL;

    if (FAILED(m_pSamplerState_Manager->Bind_PixelShader(g_strLinearSamplerTag, ETOUI(P_SLOT::LINEAR_SAMPLER))))
        return E_FAIL;

    if (FAILED(m_pSamplerState_Manager->Bind_PixelShader(g_strPointSamplerTag, ETOUI(P_SLOT::POINT_SAMPLER))))
        return E_FAIL;

    return S_OK;
}

HRESULT CGameInstance::Draw(const float4_t* pClearColor)
{
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
HRESULT CGameInstance::Add_Model(uint32_t iModelLevelIndex, const wstring_t& strModelTag, const char* pFilePath)
{
    if (nullptr == m_pModel_Manager)
        return E_FAIL;

    return m_pModel_Manager->Add_Model(strModelTag, pFilePath, iModelLevelIndex);
}

HRESULT CGameInstance::Assemble_Model(const shared_ptr<CModel>& pSourceModel, const map<wstring_t, shared_ptr<CModel>>& PartModels, shared_ptr<CModel>& pOut)
{
    if (nullptr == m_pModel_Manager)
        return E_FAIL;

    return m_pModel_Manager->Assemble_Model(pSourceModel, PartModels, pOut);
}

HRESULT CGameInstance::Add_Animation(uint32_t iLevelIndex, const wstring_t& strAnimTag, const char* pFilePath)
{
    if (nullptr == m_pModel_Manager)
        return E_FAIL;

    return m_pModel_Manager->Add_Animation(strAnimTag, pFilePath, iLevelIndex);
}

HRESULT CGameInstance::Set_Animation(uint32_t iLevelIndex, const shared_ptr<CModel>& pModel, const wstring_t& strAnimTag)
{
    if (nullptr == m_pModel_Manager)
        return E_FAIL;

    return m_pModel_Manager->Set_Animation(pModel, strAnimTag, iLevelIndex);
}

HRESULT CGameInstance::Play_Animation(const shared_ptr<CModel>& pModel, f64_t fTimeDelta)
{
    if (nullptr == pModel)
        return E_FAIL;

    return pModel->Play_Animation(fTimeDelta);
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

#pragma region PIPELINE
const float4x4_t* CGameInstance::Get_Transform(D3DTS eTransformState) const
{
    return m_pPipeline->Get_Transform(eTransformState);
}

const float4x4_t* CGameInstance::Get_OrthoTransform(D3DTS eTransformState) const
{
    return m_pPipeline->Get_OrthoTransform(eTransformState);
}

const float4x4_t* CGameInstance::Get_Transform_Inverse(D3DTS eTransformState) const
{
    return m_pPipeline->Get_Transform_Inverse(eTransformState);
}

const float4x4_t* CGameInstance::Get_OrthoTransform_Inverse(D3DTS eTransformState) const
{
    return m_pPipeline->Get_OrthoTransform_Inverse(eTransformState);
}

const float4_t* CGameInstance::Get_CamPosition() const
{
    return m_pPipeline->Get_CamPosition();
}

void CGameInstance::Set_Transform(D3DTS eTransformState, fmatrix_t TransformMatrix)
{
    m_pPipeline->Set_Transform(eTransformState, TransformMatrix);
}

void CGameInstance::Set_OrthoTransform(D3DTS eTransformState, fmatrix_t TransformMatrix)
{
    m_pPipeline->Set_OrthoTransform(eTransformState, TransformMatrix);
}

#pragma endregion

#pragma region SAMPLER_MANAGER

HRESULT CGameInstance::Add_SamplerState(const wstring_t& strSamplerTag, const D3D11_SAMPLER_DESC& SamplerDesc)
{
    return m_pSamplerState_Manager->Add_SamplerState(strSamplerTag, SamplerDesc);
}

HRESULT CGameInstance::Bind_PixelShader_Sampler(const wstring_t& strSamplerTag, uint32_t iSlot)
{
    return m_pSamplerState_Manager->Bind_PixelShader(strSamplerTag, iSlot);
}
#pragma endregion

#pragma region INPUT_DEVICE

int8_t CGameInstance::Get_DIKeyState(uint8_t byKeyID)
{
    return m_pInput_Device->Get_DIKeyState(byKeyID);
}

int8_t CGameInstance::Get_DIMouseState(DIMK eMouse)
{
    return m_pInput_Device->Get_DIMouseState(eMouse);
}

int32_t CGameInstance::Get_DIMouseMove(DIMM eMouseState)
{
    return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

#pragma endregion

void CGameInstance::Release_Engine()
{
    m_pSamplerState_Manager.reset();
    m_pTimer_Manager.reset();
    m_pLevel_Manager.reset();
    m_pModel_Manager.reset();
    m_pPrototype_Manager.reset();
    m_pObject_Manager.reset();
    m_pRenderer.reset();
    m_pPipeline.reset();

    m_pGraphic_Device.reset();
    m_pInput_Device.reset();
}
