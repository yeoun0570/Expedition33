#include "MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"

CMainApp::CMainApp()
{
}

CMainApp::~CMainApp()
{
    CGameInstance::Get().Release_Engine();
}

HRESULT CMainApp::Initialize()
{
    ENGINE_DESC     EngineDesc 
    {
        .hWnd = g_hWnd, 
        .eWinMode = WINMODE::WIN, 
        .iWinSizeX = g_iWinSizeX, 
        .iWinSizeY = g_iWinSizeY,
        .iNumLevels = ETOUI(LEVEL::END),
    };

    const wstring_t strShaderPath = L"../EngineSDK/Shader/Shader_VtxCol.hlsl";

    if (FAILED(CGameInstance::Get().Initialize_Engine(EngineDesc, m_pDevice, m_pContext, strShaderPath)))
        return E_FAIL;

    if (FAILED(Start_Level(LEVEL::LOGO)))
        return E_FAIL;

    return S_OK;
}

void CMainApp::Update(f32_t fTimeDelta)
{
    CGameInstance::Get().Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
    float4_t        vClearColor = float4_t(0.f, 0.f, 1.f, 1.f);

    if (FAILED(CGameInstance::Get().Draw(&vClearColor)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eStartLevel)
{
    if(FAILED(CGameInstance::Get().Change_Level(ETOUI(LEVEL::LOADING),
                                                CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevel)
    )))
        return E_FAIL;


    return S_OK;
}

unique_ptr<CMainApp> CMainApp::Create()
{
    // uto        pInstance = make_unique<CMainApp>();
    auto    pInstance = unique_ptr<CMainApp>(new CMainApp());

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CMainApp");
        pInstance.reset();
    }

    return pInstance;
}
