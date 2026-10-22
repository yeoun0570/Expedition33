#include "FbxLoader.h"

CFbxLoader::CFbxLoader()
{
}

CFbxLoader::~CFbxLoader()
{
	// 멤버 변수들이 포인터 타입으로 다른 곳에 생성된 애들이니까 이 클래스가 사라질 때 null로 전부 초기화 시켜놔야함.
	if (m_pManager != nullptr)
	{
		m_pManager->Destroy();

		m_pManager = nullptr;
		m_pScene = nullptr;
	}
}

HRESULT CFbxLoader::Initialize()
{
	// FbxManager : SDK 객체의 생성과 메모리를 관리하는 관리자
	// FbxIOSettings : 파일을 읽고 저장할 때 사용할 설정
	// FbxScene : 파일에서 읽어 온 모델·뼈대 등을 담는 공간

	if (m_pManager)
		return E_FAIL;


	// 1. SDK 관리자 생성
	m_pManager = FbxManager::Create();

	if (m_pManager == nullptr)
		return E_FAIL;

	// 2. 파일 입출력 설정 생성 및 등록
	FbxIOSettings* pIOSettings = FbxIOSettings::Create(m_pManager, IOSROOT);

	if (pIOSettings == nullptr)
	{
		m_pManager->Destroy();
		m_pManager = nullptr;
		return E_FAIL;
	}

	m_pManager->SetIOSettings(pIOSettings);

	// 3. 모델 데이터를 담을 공간 생성
	m_pScene = FbxScene::Create(m_pManager, "ModelScene");

	if (m_pScene == nullptr)
	{
		m_pManager->Destroy();
		m_pManager = nullptr;
		return E_FAIL;
	}

	return S_OK;
}

shared_ptr<CFbxLoader> CFbxLoader::Create()
{
	auto pInstance = shared_ptr<CFbxLoader>(new CFbxLoader);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CFbxLoader");
		pInstance.reset();
	}

	return pInstance;
}