#include "Level_Logo.h"

#include "GameInstance.h"
#include "BackGround.h"

CLevel_Logo::CLevel_Logo(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext)
    : CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize()
{
	return S_OK;
}

void CLevel_Logo::Update(f32_t)
{
}

HRESULT CLevel_Logo::Render()
{
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const wstring_t& strLayerTag)
{
    // 사본 생성(레이어로 관리)

    CBackGround::BACKGROUND_DESC    Desc{};

    /* 복제본에게 필요한 데이터 CBackGround::BACKGROUND_DESC를 셋팅해서 넘긴다. */
    //BACKGROUND_DESC		Desc{};
    //Desc.vColor = ;
    //Desc.fX = ;
    //Desc.fY = ;
    //Desc.fSizeX = ;
    //Desc.fSizeY = ;
    //Desc.iData = ;

    CGameInstance::Get().Add_GameObject(ETOUI(LEVEL::LOGO), TEXT("Prototype_GameObject_BackGround"), ETOUI(LEVEL::LOGO), strLayerTag, &Desc);

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
