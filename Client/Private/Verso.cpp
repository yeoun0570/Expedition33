#include "Verso.h"

#include "GameInstance.h"

CVerso::CVerso(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
    : CCharacter { pDevice, pContext}
{
}

HRESULT CVerso::Initialize_Prototype()
{
#pragma region 몸체 등록

    const char* pFilePath = "./Bin/Resource/FBX/Characters_FBX/Verso/Outfits/Default.fbx";

    if (FAILED(CGameInstance::Get().Add_Model(L"Verso_Body", pFilePath)))
    {
        MSG_BOX("베르소 몸통 모델 등록에 실패했습니다.");
        return E_FAIL;
    }

    MSG_BOX("베르소 몸통 모델 등록에 성공했습니다.");

#pragma endregion

    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CVerso::Initialize(void* pArg)
{
    if(FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CVerso::Priority_Update(const f32_t fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CVerso::Update(const f32_t fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CVerso::Late_Update(const f32_t fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CVerso::Render()
{
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

    if (FAILED(CGameInstance::Get().Render_Model(L"Verso_Body")))
        return E_FAIL;
#pragma endregion

    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

shared_ptr<CVerso> CVerso::Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
{
    auto    pInstance = shared_ptr<CVerso>(new CVerso(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CVerso");
        pInstance.reset();
    }

    return pInstance;
}

shared_ptr<CPrototype> CVerso::Clone(void* pArg)
{
    auto    pInstance = shared_ptr<CVerso>(new CVerso(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CVerso");
        pInstance.reset();
    }

    return pInstance;
}
