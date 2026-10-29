#include "Loader.h"

CLoader::CLoader(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext)
	: m_pDevice { pDevice },
	m_pContext { pContext }
{
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
    m_eNextLevelID = eNextLevelID;

    /* 스레드를 생성한다. */

    return S_OK;
}

shared_ptr<CLoader> CLoader::Create(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext, LEVEL eNextLevelID)
{
    auto    pInstance = shared_ptr<CLoader>(new CLoader(pDevice, pContext));

    if (FAILED(pInstance->Initialize(eNextLevelID)))
    {
        MSG_BOX("Failed to Created : CLoader");
        pInstance.reset();
    }

    return pInstance;
}