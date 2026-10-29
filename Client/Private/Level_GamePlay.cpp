#include "Level_GamePlay.h"

CLevel_GamePlay::CLevel_GamePlay(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext)
    : CLevel { pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	return S_OK;
}

void CLevel_GamePlay::Update(f32_t)
{
}

HRESULT CLevel_GamePlay::Render()
{
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
