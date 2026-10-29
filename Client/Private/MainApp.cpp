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
    };

    const wstring_t strShaderPath = L"../Engine/Shader/Shader_VtxCol.hlsl";

    if (FAILED(CGameInstance::Get().Initialize_Engine(EngineDesc, m_pDevice, m_pContext, strShaderPath)))
        return E_FAIL;

    if (FAILED(Start_Level(LEVEL::LOGO)))
        return E_FAIL;


#pragma region 모델 테스트

    const char* pFilePath = "./Bin/Resource/FBX/Characters_FBX/Verso/Outfits/Default.fbx";

    if (FAILED(CGameInstance::Get().Add_Model(L"Verso_Body", pFilePath)))
    {
        MSG_BOX("베르소 몸통 모델 등록에 실패했습니다.");
        return E_FAIL;
    }

    MSG_BOX("베르소 몸통 모델 등록에 성공했습니다.");

#pragma endregion


    return S_OK;
}

void CMainApp::Update(f32_t fTimeDelta)
{
    CGameInstance::Get().Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
    float4_t        vClearColor = float4_t(0.f, 0.f, 1.f, 1.f);

    if (FAILED(CGameInstance::Get().Draw(&vClearColor, L"Verso_Body")))
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
