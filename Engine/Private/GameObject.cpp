#include "GameObject.h"

CGameObject::CGameObject(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
	: CPrototype { pDevice, pContext }
{
}

shared_ptr<CComponent> CGameObject::Get_Component(COMPONENTID eID, const wstring_t& strComponentTag)
{
	const uint32_t iIndex = ETOUI(eID);

	if (iIndex >= ETOUI(COMPONENTID::END))
		return nullptr;

	auto	pComponent = Find_Component(eID, strComponentTag);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

HRESULT CGameObject::Initialize_GameObject()
{
	return S_OK;
}

void CGameObject::Update_GameObject(const f32_t fTimeDelta)
{
	for (auto& pComponent : m_Components[ETOUI(COMPONENTID::DYNAMIC)])
		pComponent.second->Update_Component(fTimeDelta);
}

void CGameObject::LateUpdate_GameObject(const f32_t fTimeDelta)
{
	for (auto& pComponent : m_Components[ETOUI(COMPONENTID::DYNAMIC)])
		pComponent.second->LateUpdate_Component(fTimeDelta);
}

shared_ptr<CComponent> CGameObject::Find_Component(COMPONENTID eID, const wstring_t& strComponentTag)
{
	auto	iter = m_Components[ETOUI(eID)].find(strComponentTag);

	if (iter == m_Components[ETOUI(eID)].end())
		return nullptr;

	return iter->second;
}

shared_ptr<CPrototype> CGameObject::Clone(void* pArg)
{
	return shared_ptr<CPrototype>();
}
