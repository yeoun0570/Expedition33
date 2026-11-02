#include "Verso.h"

CVerso::CVerso(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
    : CCharacter { pDevice, pContext}
{
}

HRESULT CVerso::Initialize_Prototype()
{
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
