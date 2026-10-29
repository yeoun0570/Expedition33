#include "Level_Manager.h"

#include "Level.h"

CLevel_Manager::CLevel_Manager()
{
}

HRESULT CLevel_Manager::Change_Level(uint32_t iLevelIndex, shared_ptr<class CLevel> pNewLevel)
{
    return S_OK;
}

void CLevel_Manager::Update(f32_t fTimeDelta)
{
}

HRESULT CLevel_Manager::Render()
{
    return S_OK;
}

unique_ptr<CLevel_Manager> CLevel_Manager::Create()
{
    return unique_ptr<CLevel_Manager>(new CLevel_Manager());
}