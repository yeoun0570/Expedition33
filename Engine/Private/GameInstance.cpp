#include "GameInstance.h"

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, _Out_ ComPtr<ID3D11Device>& pDevice, _Out_ ComPtr<ID3D11DeviceContext>& pContext)
{
    m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWinMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, pDevice, pContext);
    if (nullptr == m_pGraphic_Device)
        return E_FAIL;

    m_pTimer_Manager = CTimer_Manager::Create();
    if (nullptr == m_pTimer_Manager)
        return E_FAIL;

    return S_OK;
}

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
    if (FAILED(Draw_Begin(pClearColor)))
        return E_FAIL;

    /* ±×¸°´Ù. */
    
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

f32_t CGameInstance::Compute_TimeDelta(const wstring_t& strTimerTag)
{
    return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);    
}

HRESULT CGameInstance::Add_Timer(const wstring_t& strTimerTag)
{
    return m_pTimer_Manager->Add_Timer(strTimerTag);
}

void CGameInstance::Release_Engine()
{
    
    m_pTimer_Manager.reset();

    m_pGraphic_Device.reset();
}
