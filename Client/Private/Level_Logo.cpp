#include "Level_Logo.h"

#include "GameInstance.h"
#include "BackGround.h"
#include "Level_Loading.h"

CLevel_Logo::CLevel_Logo(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext)
    : CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize()
{
    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Update(f32_t)
{
    if (GetKeyState(VK_RETURN) & 0x8000)
    {
        if (FAILED(CGameInstance::Get().Change_Level(ETOUI(LEVEL::LOADING),
            CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::GAMEPLAY))))
            return;
    }          
}

HRESULT CLevel_Logo::Render()
{
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const wstring_t& strLayerTag)
{
    if (FAILED(CGameInstance::Get().Add_GameObject(ETOUI(LEVEL::LOGO), TEXT("Prototype_GameObject_BackGround"), ETOUI(LEVEL::LOGO), strLayerTag)))
        return E_FAIL;

    return S_OK;
}

shared_ptr<CLevel_Logo> CLevel_Logo::Create(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext)
{
    auto    pInstance = shared_ptr<CLevel_Logo>(new CLevel_Logo(pDevice, pContext));

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_Logo");
        pInstance.reset();
    }

    return pInstance;
}
