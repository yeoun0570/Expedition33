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
	}

	m_pScenes.clear();
}

HRESULT CFbxLoader::Initialize()
{
	// FbxManager : SDK 객체의 생성과 메모리를 관리하는 관리자
		// SDK 객체 : FBX 파일의 내용을 C++ 코드에서 다룰 수 있도록 SDK가 제공하는 객체
	// FbxIOSettings : 파일을 읽고 저장할 때 사용할 설정

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

	return S_OK;
}

HRESULT CFbxLoader::Load_Model(const char* pFilePath) 
{
	// FbxImporter : 파일의 데이터를 SDK 객체로 가져오는 도구
	// FbxScene : 파일에서 읽어 온 모델·뼈대 등을 담는 공간

	if (m_pManager == nullptr)
		return E_FAIL;

	if (pFilePath == nullptr || pFilePath[0] == '\0')
		return E_INVALIDARG;

	FbxImporter* pImporter = FbxImporter::Create(m_pManager, "");

	if (pImporter == nullptr)
		return E_FAIL;

	// 파일을 읽을 준비
	if (!pImporter->Initialize(pFilePath, -1, m_pManager->GetIOSettings()))
	{
		pImporter->Destroy();
		return E_FAIL;
	}

	// 모델 데이터를 담을 공간 생성
	FbxScene* pScene = FbxScene::Create(m_pManager, pFilePath);

	if (pScene == nullptr)
	{
		pImporter->Destroy();
		return E_FAIL;
	}


	// FBX파일을 해당 공간(FbxScene)에다가 저장해둠(몸통.fbx → 몸통용 FbxScene, 머리.fbx → 머리용 FbxScene)
	if (!pImporter->Import(pScene))
	{
		pImporter->Destroy();
		pScene->Destroy();
		return E_FAIL;
	}

	// 읽은 데이터는 Scene에 남으므로 Importer는 정리
	pImporter->Destroy();

	m_pScenes.push_back(pScene);

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