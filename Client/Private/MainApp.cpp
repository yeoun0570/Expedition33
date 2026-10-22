#include "MainApp.h"

CMainApp::CMainApp()
{
}

HRESULT CMainApp::Initialize()
{


    return S_OK;
}

void CMainApp::Update(f32_t fTimeDelta)
{
  
}

HRESULT CMainApp::Render()
{
    return S_OK;
}

unique_ptr<CMainApp> CMainApp::Create()
{
    // uto        pInstance = make_unique<CMainApp>();
    auto    pInstance = unique_ptr<CMainApp>(new CMainApp());

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CMainApp");
        pInstance.reset();
    }

    return pInstance;
}
