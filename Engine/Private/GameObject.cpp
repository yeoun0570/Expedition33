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
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

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
