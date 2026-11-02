#include "BackGround.h"

CBackGround::CBackGround(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
    : CUIObject { pDevice, pContext }
{
}

HRESULT CBackGround::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBackGround::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CBackGround::Priority_Update(const f32_t fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CBackGround::Update(const f32_t fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CBackGround::Late_Update(const f32_t fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CBackGround::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

shared_ptr<CPrototype> CBackGround::Clone(void* pArg)
{
    return shared_ptr<CPrototype>();
}

shared_ptr<CBackGround> CBackGround::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto    pInstance = shared_ptr<CBackGround>(new CBackGround(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackGround");
		pInstance.reset();
	}

	return pInstance;
}


shared_ptr<CPrototype> CBackGround::Clone(void* pArg)
{
	auto    pInstance = shared_ptr<CBackGround>(new CBackGround(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBackGround");
		pInstance.reset();
	}

	return pInstance;
}