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

    const wstring_t strShaderPath = L"../Engine/Shader/Shader_VtxCol.hlsl";

    if (FAILED(CGameInstance::Get().Initialize_Engine(EngineDesc, m_pDevice, m_pContext, strShaderPath)))
        return E_FAIL;

    if (FAILED(Start_Level(LEVEL::LOGO)))
        return E_FAIL;


#pragma region 모델 테스트(모델 등록)

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

#pragma region 모델 테스트(월드, 뷰, 투영 행렬 적용)
    TRANSFORM Transform{};

    // 1. 월드 행렬: 원본 크기와 위치 유지
    // XMMatrixIdentity : 단위 행렬 만들기
    XMStoreFloat4x4(
        &Transform.matWorld,
        XMMatrixIdentity());

    // 2. 뷰 행렬: 카메라에서 모델 바라보기
    // XMMatrixLookAtLH : 뷰 행렬 만들기
    XMStoreFloat4x4(
        &Transform.matView,
        XMMatrixLookAtLH(
            XMVectorSet(0.f, -3.f, 1.f, 1.f),   // 카메라 Eye
            XMVectorSet(0.f, 0.f, 1.f, 1.f),    // 카메라 At
            XMVectorSet(0.f, 0.f, 1.f, 0.f)));  // 카메라 Up: +Z

    // 3. 투영 행렬: 원근감 적용
    // XMMatrixPerspectiveFovLH : 투영 행렬 만들기
    XMStoreFloat4x4(
        &Transform.matProj,
        XMMatrixPerspectiveFovLH(
            XMConvertToRadians(60.f),
            static_cast<f32_t>(g_iWinSizeX) /
            static_cast<f32_t>(g_iWinSizeY),
            0.1f,
            10000.f));

    // 4. GPU 상수 버퍼에 전달
    if (FAILED(CGameInstance::Get().Update_Transform(Transform)))
        return E_FAIL;
#pragma endregion


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
