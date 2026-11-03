#include "Layer.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(shared_ptr<CGameObject> pGameObject)
{
    if (nullptr == pGameObject)
        return E_FAIL;

    m_GameObjects.push_back(pGameObject);

    return S_OK;
}

void CLayer::Priority_Update(const f32_t fTimeDelta)
{
    for (auto& pObject : m_GameObjects)
    {
        if (nullptr != pObject)
            pObject->Priority_Update(fTimeDelta);
    }
}

void CLayer::Update(const f32_t fTimeDelta)
{
    for (auto& pObject : m_GameObjects)
    {
        if (nullptr != pObject)
            pObject->Update(fTimeDelta);
    }
}

void CLayer::Late_Update(const f32_t fTimeDelta)
{
    for (auto& pObject : m_GameObjects)
    {
        if (nullptr != pObject)
            pObject->Late_Update(fTimeDelta);
    }
}

shared_ptr<CLayer> CLayer::Create()
{
    return shared_ptr<CLayer>(new CLayer);
}

