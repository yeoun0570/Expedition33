#include "Prototype_Manager.h"

CPrototype_Manager::CPrototype_Manager()
{
}

HRESULT CPrototype_Manager::Initialize(uint32_t iNumLevels)
{
    m_iNumLevels = iNumLevels;

    m_Prototypes = make_shared<PROTOTYPES[]>(iNumLevels);

	return S_OK;
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
