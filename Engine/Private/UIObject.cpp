#include "UIObject.h"

CUIObject::CUIObject(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
    : CGameObject { pDevice, pContext }
{
}

HRESULT CUIObject::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUIObject::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        auto    pDesc = static_cast<UIOBJECT_DESC*>(pArg);

        m_fX = pDesc->fX;
        m_fY = pDesc->fY;
        m_fSizeX = pDesc->fSizeX;
        m_fSizeY = pDesc->fSizeY;
    }


    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CUIObject::Priority_Update(const f32_t fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CUIObject::Update(const f32_t fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CUIObject::Late_Update(const f32_t fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CUIObject::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}
