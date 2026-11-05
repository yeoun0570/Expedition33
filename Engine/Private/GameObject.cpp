#include "GameObject.h"

CGameObject::CGameObject(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
    : CPrototype { pDevice, pContext }
{
}

HRESULT CGameObject::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        auto		pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
        m_iData = pDesc->iData;
    }

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == m_pTransformCom)
        return E_FAIL;

    if (FAILED(m_pTransformCom->Initialize(pArg)))
        return E_FAIL;

    m_pComponents.emplace(g_strTransformComTag, m_pTransformCom);

    return S_OK;
}

void CGameObject::Priority_Update(const f32_t fTimeDelta)
{
}

void CGameObject::Update(const f32_t fTimeDelta)
{
}

void CGameObject::Late_Update(const f32_t fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
    return S_OK;
}
