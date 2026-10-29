#include "Level_Loading.h"

#include "GameInstance.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"

CLevel_Loading::CLevel_Loading(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext)
    : CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
    m_eNextLevelID = eNextLevelID;

    m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
    if (nullptr == m_pLoader)
        return E_FAIL;

    return S_OK;
}

void CLevel_Loading::Update(f32_t)
{
    if (m_pLoader->IsFinished()
        && GetKeyState(VK_SPACE))
    {
        shared_ptr<CLevel> pNewLevel = nullptr;

        switch (m_eNextLevelID)
        {
        case LEVEL::LOGO:
            pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
            break;
        case LEVEL::GAMEPLAY:
            pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
            break;
        default:
            break;
        }

        if (nullptr == pNewLevel)
            return;

        if(FAILED(CGameInstance::Get().Change_Level((ETOUI(m_eNextLevelID)), pNewLevel)))
            return;

        return;
    }
}

HRESULT CLevel_Loading::Render()
{
	return S_OK;
}

shared_ptr<CLevel_Loading> CLevel_Loading::Create(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext, LEVEL eNextLevelID)
{
    auto    pInstance = shared_ptr<CLevel_Loading>(new CLevel_Loading(pDevice, pContext));

    if (FAILED(pInstance->Initialize(eNextLevelID)))
    {
        MSG_BOX("Failed to Created : CLevel_Loading");
        pInstance.reset();
    }

    return pInstance;
}
