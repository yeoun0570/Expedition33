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

shared_ptr<CLayer> CLayer::Create()
{
    return shared_ptr<CLayer>(new CLayer);
}

