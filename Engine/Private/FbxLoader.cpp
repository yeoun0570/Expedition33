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

	m_vecScenes.clear();
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

	m_vecScenes.push_back(pScene);

	// Scene의 면을 삼각형으로 변환
	FbxGeometryConverter Converter(m_pManager);

	if (!Converter.Triangulate(pScene, true))
	{
		MSG_BOX("FBX의 삼각형 변환에 실패했습니다.");
		return E_FAIL;
	}

	// ===============================================================================//
	// ===============================================================================//
	// ===============================================================================//

	// Fbx에서메시 정보 추출해서 저장하기
	

	// 변환된 메시를 대상으로 개수 확인과 데이터 추출
	const int iMeshCount = pScene->GetSrcObjectCount<FbxMesh>();

	if (iMeshCount == 0)
	{
		MSG_BOX("FBX 파일에 메시가 없습니다.");
	}

	// Fbx의 메시 정점, 인덱스 정보 저장
	for (int i = 0; i < iMeshCount; ++i)
	{
		FbxMesh* pMesh = pScene->GetSrcObject<FbxMesh>(i);

		// 정점 정보 저장

		MESH_DATA MeshData{};

		// FBX에 저장된 점 개수와 좌표 배열
		const int32_t iPointCount = pMesh->GetControlPointsCount();
		const FbxVector4* pPoints = pMesh->GetControlPoints();

		if (iPointCount <= 0 || pPoints == nullptr)
			return E_FAIL;

		// 점 개수만큼 정점 공간 확보
		MeshData.vecVerticles.resize(static_cast<size_t>(iPointCount));

		for (int32_t iPoint = 0; iPoint < iPointCount; ++iPoint)
		{
			const FbxVector4& Position = pPoints[iPoint];

			MeshData.vecVerticles[iPoint].vPosition =
			{
				static_cast<f32_t>(Position[0]),
				static_cast<f32_t>(Position[1]),
				static_cast<f32_t>(Position[2])
			};

			// 첫 표시는 텍스처 없이 초록색으로 확인
			MeshData.vecVerticles[iPoint].vColor =
			{
				0.f, 1.f, 0.f, 1.f
			};
		}


		// ===============================================================================//

		// 인덱스 정보 저장

		const int32_t iPolygonCount = pMesh->GetPolygonCount();

		// 삼각형 하나당 인덱스 3개
		MeshData.vecIndices.reserve(static_cast<size_t>(iPolygonCount) * 3);

		for (int32_t iPolygon = 0; iPolygon < iPolygonCount; ++iPolygon)
		{
			if (pMesh->GetPolygonSize(iPolygon) != 3)
			{
				MSG_BOX("삼각형이 아닌 면이 있습니다.");
				return E_FAIL;
			}

			for (int32_t iCorner = 0; iCorner < 3; ++iCorner)
			{
				const int32_t iPointIndex = pMesh->GetPolygonVertex(iPolygon, iCorner);

				if (iPointIndex < 0 || iPointIndex >= iPointCount)
				{
					MSG_BOX("잘못된 정점 번호가 있습니다.");
					return E_FAIL;
				}

				MeshData.vecIndices.push_back(static_cast<uint32_t>(iPointIndex));
			}
		}

		// 정점과 인덱스를 모두 채운 뒤 보관
		m_vecMeshDatas.push_back(MeshData);


		// ===============================================================================//

		// 처리 메시지

		char szMessage[512] = {};

		sprintf_s(
			szMessage,
			"Mesh: %s\nVertices: %zu\nIndices: %zu\nTriangles: %d",
			pMesh->GetName(),
			MeshData.vecVerticles.size(),
			MeshData.vecIndices.size(),
			iPolygonCount);

		MessageBoxA(
			nullptr,
			szMessage,
			"FBX 메시 정보",
			MB_OK);
	}

	return S_OK;
}

unique_ptr<CFbxLoader> CFbxLoader::Create()
{
	auto pInstance = unique_ptr<CFbxLoader>(new CFbxLoader);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CFbxLoader");
		pInstance.reset();
	}

	return pInstance;
}