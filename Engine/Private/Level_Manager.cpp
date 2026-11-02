#include "Level_Manager.h"

#include "GameInstance.h"
#include "Level.h"

CLevel_Manager::CLevel_Manager()
{
}

HRESULT CLevel_Manager::Change_Level(uint32_t iLevelIndex, shared_ptr<class CLevel> pNewLevel)
{
	if (nullptr != m_pCurrentLevel)
	{
		/* 기존레벨에서 이용한 자원들을 정리한다. */
		if (FAILED(CGameInstance::Get().Clear_Resources(m_iCurrentLevelIndex)))
			return E_FAIL;

		m_pCurrentLevel.reset();
	}

	m_pCurrentLevel = pNewLevel;

	m_iCurrentLevelIndex = iLevelIndex;

	return S_OK;
}

void CLevel_Manager::Update(f32_t fTimeDelta)
{
	m_pCurrentLevel->Update(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	return m_pCurrentLevel->Render();
}

unique_ptr<CLevel_Manager> CLevel_Manager::Create()
{
	return unique_ptr<CLevel_Manager>(new CLevel_Manager());
}