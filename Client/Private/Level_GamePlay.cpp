#include "Level_GamePlay.h"

#include "GameInstance.h"
#include "Verso.h"

CLevel_GamePlay::CLevel_GamePlay(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext)
    : CLevel { pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
    if(FAILED(Ready_Layer_Characters(TEXT("Layer_Characters"))))
        return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Update(f32_t)
{
}

HRESULT CLevel_GamePlay::Render()
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Characters(const wstring_t& strLayerTag)
{
    CVerso::VERSO_DESC  Desc{};

    /* 海福家 单捞磐 技泼 */
    //Desc.iData = ;

    if(FAILED(CGameInstance::Get().Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Verso"),
                                        ETOUI(LEVEL::GAMEPLAY), strLayerTag, 
                                        &Desc)))
        return E_FAIL;

    return S_OK;
}

shared_ptr<CLevel_GamePlay> CLevel_GamePlay::Create(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext)
{
    auto    pInstance = shared_ptr<CLevel_GamePlay>(new CLevel_GamePlay(pDevice, pContext));

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_GamePlay");
        pInstance.reset();
    }

    return pInstance;
}
