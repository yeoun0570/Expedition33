#include "Component.h"

CComponent::CComponent(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext)
    : CPrototype { pDevice, pContext }
{
}

HRESULT CComponent::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CComponent::Initialize(void* pArg)
{
    return S_OK;
}
