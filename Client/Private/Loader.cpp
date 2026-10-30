#include "Loader.h"

CLoader::CLoader(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext)
	: m_pDevice { pDevice },
	m_pContext { pContext }
{
}

CLoader::~CLoader()
{
    WaitForSingleObject(m_hThread, INFINITE);

    DeleteObject(m_hThread);

    CloseHandle(m_hThread);

    DeleteCriticalSection(&m_CrticalSection);
}

uint32_t APIENTRY ThreadMain(void* pArg)
{
    auto    pLoaoder = static_cast<CLoader*>(pArg);

    if (FAILED(pLoaoder->Loading()))
        return 1;

    return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
    m_eNextLevelID = eNextLevelID;

    InitializeCriticalSection(&m_CrticalSection);

    /* 스레드를 생성한다. */
    m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadMain, this, 0, nullptr);
    if (nullptr == m_hThread)
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::Loading()
{
    EnterCriticalSection(&m_CrticalSection);

    CoInitializeEx(nullptr, 0);

    HRESULT hr = {};

    switch (m_eNextLevelID)
    {
    case LEVEL::LOGO:
        hr = Loading_For_LogoLV();
        break;
    case LEVEL::GAMEPLAY:
        hr = Loading_For_GamePlayLV();
        break;
    default:
        break;
    }

    if (FAILED(hr))
        return E_FAIL;

    CoUninitialize();

    LeaveCriticalSection(&m_CrticalSection);

    return S_OK;
}

HRESULT CLoader::Loading_For_LogoLV()
{
    return S_OK;
}

HRESULT CLoader::Loading_For_GamePlayLV()
{
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