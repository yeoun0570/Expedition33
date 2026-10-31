#include "GameObject_Manager.h"

CGameObject_Manager::CGameObject_Manager()
{
}

HRESULT CGameObject_Manager::Initialize(uint32_t iNumLevels)
{
    m_Objects = make_shared<OBJECTS[]>(iNumLevels);

    return S_OK;
}

void CGameObject_Manager::Update_GameObject(uint32_t iNumLevels, const f32_t fTimeDelta)
{
    for (auto& pObject : m_Objects.get()[iNumLevels])
        pObject.second->Update_GameObject(fTimeDelta);
}

void CGameObject_Manager::LateUpdate_GameObject(uint32_t iNumLevels, const f32_t fTimeDelta)
{
    for (auto& pObject : m_Objects.get()[iNumLevels])
        pObject.second->LateUpdate_GameObject(fTimeDelta);
}

void CGameObject_Manager::Render_GameObject(uint32_t iNumLevels)
{
    for (auto& pObject : m_Objects.get()[iNumLevels])
        pObject.second->Render_GameObject();
}

unique_ptr<CGameObject_Manager> CGameObject_Manager::Create(uint32_t iNumLevels)
{
    auto    pInstance = unique_ptr<CGameObject_Manager>(new CGameObject_Manager());

    if (FAILED(pInstance->Initialize(iNumLevels)))
    {
        MSG_BOX("Failed to Created : CGameObject_Manager");
        pInstance.reset();
    }

    return pInstance;
}