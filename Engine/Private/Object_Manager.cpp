#include "Object_Manager.h"

#include "GameInstance.h"
#include "Layer.h"

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Initialize(uint32_t iNumLevels)
{
    m_iNumLevels = iNumLevels;

    m_pLayers = make_shared<LAYERS[]>(iNumLevels);

    return S_OK;
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

void CObject_Manager::Priority_Update(const f32_t fTimeDelta)
{
    for (uint32_t i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& Pair : m_pLayers[i])
        {
            Pair.second->Priority_Update(fTimeDelta);
        }
    }
}

void CObject_Manager::Update(const f32_t fTimeDelta)
{
    for (uint32_t i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& Pair : m_pLayers[i])
        {
            Pair.second->Update(fTimeDelta);
        }
    }
}

void CObject_Manager::Late_Update(const f32_t fTimeDelta)
{
    for (uint32_t i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& Pair : m_pLayers[i])
        {
            Pair.second->Late_Update(fTimeDelta);
        }
    }
}

void CObject_Manager::Clear(uint32_t iClearLevelIndex)
{
    for (auto& Pair : m_pLayers[iClearLevelIndex])
    {
        Pair.second.reset();
    }

    m_pLayers[iClearLevelIndex].clear();
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