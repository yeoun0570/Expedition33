#include "Component.h"

CComponent::CComponent(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
	: CPrototype { pDevice, pContext }
{
}

void CComponent::Update_Component(const f32_t fTimeDelta)
{
}

void CComponent::LateUpdate_Component(const f32_t fTimeDelta)
{
}

shared_ptr<CPrototype> CComponent::Clone(void* pArg)
{
	return shared_ptr<CPrototype>();
}
