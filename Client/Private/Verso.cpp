#include "Verso.h"

#include "GameInstance.h"

CVerso::CVerso(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
    : CCharacter { pDevice, pContext}
{
}

HRESULT CVerso::Initialize_Prototype()
{
#pragma region 몸체 등록

    const char* pFilePath = "./Bin/Resources/Models/Characters/Verso/Outfits/Default.fbx";

    if (FAILED(CGameInstance::Get().Add_Model(ETOUI(LEVEL::STATIC), TEXT("Prototype_Verso_Body_Default_Model"), pFilePath)))
    {
#ifdef _DEBUG
        MSG_BOX("베르소 기본 몸통 모델 등록에 실패했습니다.");
#endif
        return E_FAIL;
    }

    pFilePath = "./Bin/Resources/Models/Characters/Verso/Outfits/BikiniB.fbx";

    if (FAILED(CGameInstance::Get().Add_Model(ETOUI(LEVEL::STATIC), TEXT("Prototype_Verso_Body_BikniB_Model"), pFilePath)))
    {
#ifdef _DEBUG
        MSG_BOX("베르소 비키니 몸통 모델 등록에 실패했습니다.");
#endif
        return E_FAIL;
    }


#ifdef _DEBUG
    MSG_BOX("베르소 몸통 모델 등록에 성공했습니다.");
#endif

#pragma endregion

#pragma region 얼굴 등록

    pFilePath = "./Bin/Resources/Models/Characters/Verso/Faces/Default.fbx";

    if (FAILED(CGameInstance::Get().Add_Model(ETOUI(LEVEL::STATIC), TEXT("Prototype_Verso_Face_Model"), pFilePath)))
    {
#ifdef _DEBUG
        MSG_BOX("베르소 얼굴 모델 등록에 실패했습니다.");
#endif
        return E_FAIL;
    }

#ifdef _DEBUG
    MSG_BOX("베르소 얼굴 모델 등록에 성공했습니다.");
#endif

#pragma endregion

#pragma region 애니메이션 등록

    pFilePath = "./Bin/Resources/Models/Characters/Verso/Animations/Verso_Idle_Base.fbx";

    if (FAILED(CGameInstance::Get().Add_Animation(ETOUI(LEVEL::STATIC), TEXT("Verso_Idle_Base"), pFilePath)))
    {
#ifdef _DEBUG
        MSG_BOX("베르소 Idle 애니메이션 등록에 실패했습니다.");
#endif
        return E_FAIL;
    }

    pFilePath = "./Bin/Resources/Models/Characters/Verso/Animations/Verso_Skill_Decimate.fbx";

    if (FAILED(CGameInstance::Get().Add_Animation(ETOUI(LEVEL::STATIC), TEXT("Verso_Skill_Decimate"), pFilePath)))
    {
#ifdef _DEBUG
        MSG_BOX("베르소 Skill_Decimate 애니메이션 등록에 실패했습니다.");
#endif
        return E_FAIL;
    }

#pragma endregion


    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CVerso::Initialize(void* pArg)
{
    if(FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CVerso::Priority_Update(const f32_t fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CVerso::Update(const f32_t fTimeDelta)
{
    #ifdef _DEBUG
    static f32_t	fAccTime = 0.f;
    static uint32_t	iFrameCount = 0;

    fAccTime += fTimeDelta;
    ++iFrameCount;

    if (fAccTime >= 1.f)
    {
        char_t szLog[64]{};
        sprintf_s(szLog, "[FPS] %u\n", iFrameCount);
        OutputDebugStringA(szLog);

        fAccTime = 0.f;
        iFrameCount = 0;
    }
#endif


#pragma region 모델 테스트(월드, 뷰, 투영 행렬 적용)
    // 1. 월드 행렬: 원본 크기와 위치 유지
    // XMMatrixIdentity : 단위 행렬 만들기
    XMStoreFloat4x4(
        &m_Transform.matWorld,
        XMMatrixIdentity());

    // 2. 뷰 행렬: 카메라에서 모델 바라보기
    // XMMatrixLookAtLH : 뷰 행렬 만들기
    XMStoreFloat4x4(
        &m_Transform.matView,
        XMMatrixLookAtLH(
            XMVectorSet(0.f, -3.f, 1.f, 1.f),   // 카메라 Eye
            XMVectorSet(0.f, 0.f, 1.f, 1.f),    // 카메라 At
            XMVectorSet(0.f, 0.f, 1.f, 0.f)));  // 카메라 Up: +Z

    // 3. 투영 행렬: 원근감 적용
    // XMMatrixPerspectiveFovLH : 투영 행렬 만들기
    XMStoreFloat4x4(
        &m_Transform.matProj,
        XMMatrixPerspectiveFovLH(
            XMConvertToRadians(60.f),
            static_cast<f32_t>(g_iWinSizeX) /
            static_cast<f32_t>(g_iWinSizeY),
            0.1f,
            10000.f));
#pragma endregion

#pragma region 몸체 변경
    if (GetKeyState('1') & 0x8000)
        Change_Body(m_pDefaultBodyModelCom);

    else if (GetKeyState('2') & 0x8000)
        Change_Body(m_pBikiniBBodyModelCom);

#pragma endregion

#pragma region 애니메이션
    wstring_t strNextAnimTag = m_strAnimTag;

    if (GetKeyState(VK_F1) & 0x8000)
        strNextAnimTag = TEXT("Verso_Idle_Base");

    else if (GetKeyState(VK_F2) & 0x8000)
        strNextAnimTag = TEXT("Verso_Skill_Decimate");

    // 동작이 실제로 바뀔 때만 한 번 연결 (키를 누르고 있는 동안 매 프레임 다시 걸지 않도록)
    if (strNextAnimTag != m_strAnimTag)
    {
        if (SUCCEEDED(CGameInstance::Get().Set_Animation(ETOUI(LEVEL::STATIC), m_pAssembleModelCom, strNextAnimTag)))
            m_strAnimTag = strNextAnimTag;
    }

    CGameInstance::Get().Play_Animation(m_pAssembleModelCom, fTimeDelta);
#pragma endregion


    __super::Update(fTimeDelta);
}

void CVerso::Late_Update(const f32_t fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    CGameInstance::Get().Add_RenderGroup(RENDERGROUP::NONBLEND, 
                                        dynamic_pointer_cast<CGameObject>(shared_from_this()));
}

HRESULT CVerso::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;
    

    return S_OK;
}

HRESULT CVerso::Ready_Components()
{
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Prototype_Verso_Body_Default_Model"),
        TEXT("Com_Verso_Body_Default_Model"), m_pDefaultBodyModelCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Prototype_Verso_Body_BikniB_Model"),
        TEXT("Com_Verso_Body_BikiniB_Model"), m_pBikiniBBodyModelCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Prototype_Verso_Face_Model"),
        TEXT("Com_Verso_Face_Model"), m_pFaceModelCom)))
        return E_FAIL;

    // 첫 몸체 및 애니메이션 설정
    m_strAnimTag = TEXT("Verso_Idle_Base");

    if (FAILED(Change_Body(m_pDefaultBodyModelCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CVerso::Change_Body(const shared_ptr<CModel>& pBodyModel)
{
    if (nullptr == pBodyModel)
        return E_FAIL;

    // 파츠 이름과 이미 준비한 모델 사본을 연결한다.
    const map<wstring_t, shared_ptr<CModel>> PartModels
    {
        { TEXT("Body"), pBodyModel },
        { TEXT("Face"), m_pFaceModelCom }
    };

    // 몸체에서 기준 노드를 가져오되,
    // 출력할 몸체와 얼굴은 모두 파츠로 전달한다.
    // 생성과 조립 계산은 엔진에서 처리한다.
    if (FAILED(CGameInstance::Get().Assemble_Model(pBodyModel, PartModels, m_pAssembleModelCom)))
        return E_FAIL;

    // 새로 조립된 모델에 보관해 둔 애니메이션 연결
    if (FAILED(CGameInstance::Get().Set_Animation(ETOUI(LEVEL::STATIC), m_pAssembleModelCom, m_strAnimTag)))
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
