#include "Character.h"

CCharacter::CCharacter(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
    : CGameObject { pDevice, pContext }
{
}

HRESULT CCharacter::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCharacter::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CCharacter::Priority_Update(const f32_t fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CCharacter::Update(const f32_t fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CCharacter::Late_Update(const f32_t fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CCharacter::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}
