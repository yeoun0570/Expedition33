#include "Object_Manager.h"

#include "GameInstance.h"
#include "Layer.h"

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Initialize(uint32_t iNumLevels)
{

}

HRESULT CObject_Manager::Add_GameObject(uint32_t iPrototypeLevelIndex, const wstring_t& strPrototypeTag, uint32_t iLayerLevelIndex, const wstring_t& strLayerTag, void* pArg)
{
    auto    pGameObject = dynamic_pointer_cast<CGameObject>(CGameInstance::Get().Clone_Prototype(iPrototypeLevelIndex, strPrototypeTag, pArg));
    if (nullptr == pGameObject)
        return E_FAIL;


    auto    pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
    if (nullptr == pLayer)
    {
        pLayer = CLayer::Create();
        pLayer->Add_GameObject(pGameObject);
        m_pLayers[iLayerLevelIndex].emplace(strLayerTag, pLayer);
    }
    else
        pLayer->Add_GameObject(pGameObject);

    return S_OK;
}

shared_ptr<class CLayer> CObject_Manager::Find_Layer(uint32_t iLayerLevelIndex, const wstring_t& strLayerTag)
{
    auto    iter = m_pLayers[iLayerLevelIndex].find(strLayerTag);
    if (iter == m_pLayers[iLayerLevelIndex].end())
        return nullptr;

    return iter->second;
}


unique_ptr<CObject_Manager> CObject_Manager::Create(uint32_t iNumLevels)
{
    auto    pInstance = unique_ptr<CObject_Manager>(new CObject_Manager());

    if (FAILED(pInstance->Initialize(iNumLevels)))
    {
        MSG_BOX("Failed to Created : CObject_Manager");
        pInstance.reset();
    }

    return pInstance;
}