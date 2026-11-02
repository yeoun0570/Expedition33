#include "Prototype_Manager.h"
#include "Prototype.h"

CPrototype_Manager::CPrototype_Manager()
{
}

HRESULT CPrototype_Manager::Initialize(uint32_t iNumLevels)
{
    m_iNumLevels = iNumLevels;

    m_pPrototypes = make_shared<PROTOTYPES[]>(iNumLevels);

	return S_OK;
}

HRESULT CPrototype_Manager::Add_Prototype(uint32_t iPrototypeLevelIndex, const wstring_t& strPrototypeTag, shared_ptr<class CPrototype> pPrototype)
{
    if (nullptr == m_pPrototypes ||
        iPrototypeLevelIndex >= m_iNumLevels ||
        nullptr != Find_Prototype(iPrototypeLevelIndex, strPrototypeTag)
        )
        return E_FAIL;

    m_pPrototypes[iPrototypeLevelIndex].emplace(strPrototypeTag, pPrototype);

    return S_OK;
}

shared_ptr<CPrototype> CPrototype_Manager::Clone_Prototype(uint32_t iPrototypeLevelIndex, const wstring_t& strPrototypeTag, void* pArg)
{
    auto    pPrototype = Find_Prototype(iPrototypeLevelIndex, strPrototypeTag);
    if (nullptr == pPrototype)
        return nullptr;

    return pPrototype->Clone(pArg);
}

shared_ptr<class CPrototype> CPrototype_Manager::Find_Prototype(uint32_t iPrototypeLevelIndex, const wstring_t& strPrototypeTag)
{
    auto    iter = m_pPrototypes[iPrototypeLevelIndex].find(strPrototypeTag);
    if (iter == m_pPrototypes[iPrototypeLevelIndex].end())
        return nullptr;

    return iter->second;
}

unique_ptr<CPrototype_Manager> CPrototype_Manager::Create(uint32_t iNumLevels)
{
    auto    pInstance = unique_ptr<CPrototype_Manager>(new CPrototype_Manager());

    if (FAILED(pInstance->Initialize(iNumLevels)))
    {
        MSG_BOX("Failed to Created : CPrototype_Manager");
        pInstance.reset();
    }

    return pInstance;
}
