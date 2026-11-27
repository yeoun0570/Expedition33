#include "Camera.h"

CCamera::CCamera(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
    : CGameObject{ pDevice, pContext }
{
}

HRESULT CCamera::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (nullptr != pArg)
    {
        auto		pDesc = static_cast<CAMERA_DESC*>(pArg);

        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vEye), 1.f));

        m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&pDesc->vAt), 1.f));


        m_fFovy = pDesc->fFovy;
        m_fNear = pDesc->fNear;
        m_fFar = pDesc->fFar;
    }

    Update_PipeLine();

    return S_OK;
}

void CCamera::Priority_Update(const f32_t fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CCamera::Update(const f32_t fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CCamera::Late_Update(const f32_t fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

void CCamera::Update_PipeLine()
{
    CGameInstance::Get().Set_Transform(D3DTS::VIEW, 
        XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr())));

    float2_t vViewPortSize = CGameInstance::Get().Get_ViewPortSize();

    CGameInstance::Get().Set_Transform(D3DTS::PROJ,
        XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fFovy), vViewPortSize.x / vViewPortSize.y, m_fNear, m_fFar));
}
