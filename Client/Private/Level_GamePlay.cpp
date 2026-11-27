#include "Level_GamePlay.h"

#include "GameInstance.h"
#include "Verso.h"
#include "Camera_Free.h"

CLevel_GamePlay::CLevel_GamePlay(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext)
    : CLevel { pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;


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
    CVerso::VERSO_DESC VersoDesc{};

    if(FAILED(CGameInstance::Get().Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_Verso"),
                                                  ETOUI(LEVEL::GAMEPLAY), strLayerTag, &VersoDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring_t& strLayerTag)
{
    CCamera_Free::CAMERA_FREE_DESC CameraFreeDesc{};

    CameraFreeDesc.vEye = float3_t(0.f, 20.f, -10.f);
    CameraFreeDesc.vAt = float3_t(0.f, 0.f, 0.f);
    CameraFreeDesc.fFovy = 65.f;
    CameraFreeDesc.fNear = 0.1f;
    CameraFreeDesc.fFar = 1000.f;
    CameraFreeDesc.fMouseSensor = 0.05f;
    CameraFreeDesc.fSpeedPerSec = 20.f;
    CameraFreeDesc.fRotationDegreePerSec = 180.f;

    if (FAILED(CGameInstance::Get().Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Free"),
                                                   ETOUI(LEVEL::GAMEPLAY), strLayerTag, &CameraFreeDesc)))
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
