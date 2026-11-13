#include <fbxsdk.h>
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

	/* 파일 읽을 준비와 저장할 공간 준비 */

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

	// Scene의 면을 삼각형으로 변환
	FbxGeometryConverter Converter(m_pManager);

	if (!Converter.Triangulate(pScene, true))
	{
		MSG_BOX("FBX의 삼각형 변환에 실패했습니다.");
		return E_FAIL;
	}


	if (FAILED(Read_BoneNodes(pScene)))
		return E_FAIL;

	/* Fbx에서 메시 정보 추출해서 저장하기 */
	/* Fbx의 메시 정점, 인덱스, 스킨/클러스터 정보 저장 */

	// 변환된 메시를 대상으로 개수 확인과 데이터 추출
	const int iMeshCount = pScene->GetSrcObjectCount<FbxMesh>();

	if (iMeshCount == 0)
	{
		MSG_BOX("FBX 파일에 메시가 없습니다.");
	}

	for (int i = 0; i < iMeshCount; ++i)
	{
		FbxMesh* pMesh = pScene->GetSrcObject<FbxMesh>(i);

		MESH_DATA MeshData{};

		if (FAILED(Read_MeshVertexIndex(pMesh, MeshData)))
			return E_FAIL;

		Read_MeshCluster(pMesh, MeshData);

		// 정점, 인덱스, 클러스터를 모두 채운 뒤 보관
 		m_MeshDatas.push_back(MeshData);
	}

	return S_OK;
}

HRESULT CFbxLoader::Read_BoneNodes(FbxScene* pScene)
{
	/* Fbx 노드 데이터 읽어와서 뼈대 가져오기 */
	/* 뼈가 존재하는 노드들만 m_vecNodeDatas에 넣기 */

	m_NodeDatas.clear(); // 새로 로드 되었을 때 비워놓고 다시 저장하기 위한 용도

	// 최상위 노드 데이터 m_vecNodeDatas에 저장
	FbxNode* pRootNode = pScene->GetRootNode();
	if (nullptr == pRootNode)
		return E_FAIL;

	vector<FbxNode*> vecNodes;
	vecNodes.push_back(pRootNode);

	FBX_NODE_DATA	RootData
	{
		pRootNode->GetName(),
		-1,
		pRootNode->GetSkeleton() != nullptr,
	};

	m_NodeDatas.push_back(RootData);


	// 나머지 노드들 데이터 m_vecNodeDatas에 저장
	for (uint64_t iNodeIndex = 0; iNodeIndex < vecNodes.size(); ++iNodeIndex)
	{
		FbxNode* pParentNode = vecNodes[iNodeIndex];

		for (int32_t iChildIndex = 0; iChildIndex < pParentNode->GetChildCount(); ++iChildIndex)
		{
			FbxNode* pChildNode = pParentNode->GetChild(iChildIndex);

			vecNodes.push_back(pChildNode);


			FBX_NODE_DATA	ChildData
			{
				pChildNode->GetName(),
				static_cast<int32_t>(iNodeIndex),
				pChildNode->GetSkeleton() != nullptr,
			};

			m_NodeDatas.push_back(ChildData);
		}
	}

	return S_OK;
}

HRESULT CFbxLoader::Read_MeshVertexIndex(FbxMesh* pMesh, MESH_DATA& MeshData)
{
	/* 정점 정보 저장 */

	// FBX에 저장된 점 개수와 좌표 배열
	const int32_t iPointCount = pMesh->GetControlPointsCount();
	const FbxVector4* pPoints = pMesh->GetControlPoints();

	if (iPointCount <= 0 || pPoints == nullptr)
		return E_FAIL;

	// 점 개수만큼 정점 공간 확보
	MeshData.Verticles.resize(static_cast<size_t>(iPointCount));

	for (int32_t iPoint = 0; iPoint < iPointCount; ++iPoint)
	{
		const FbxVector4& Position = pPoints[iPoint];

		MeshData.Verticles[iPoint].vPosition =
		{
			static_cast<f32_t>(Position[0]),
			static_cast<f32_t>(Position[1]),
			static_cast<f32_t>(Position[2])
		};

		// 첫 표시는 텍스처 없이 초록색으로 확인
		MeshData.Verticles[iPoint].vColor =
		{
			0.f, 1.f, 0.f, 1.f
		};
		
	}

	// ===============================================================================//

	/* 인덱스 정보 저장 */

	const int32_t iPolygonCount = pMesh->GetPolygonCount();

	// 삼각형 하나당 인덱스 3개
	MeshData.Indices.reserve(static_cast<size_t>(iPolygonCount) * 3);

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

			MeshData.Indices.push_back(static_cast<uint32_t>(iPointIndex));
		}
	}

	return S_OK;
}

void CFbxLoader::Read_MeshCluster(FbxMesh* pMesh, MESH_DATA& MeshData)
{
	/* 스킨/클러스터 정보 저장*/
	
	// FbxDeformer : FbxSkin, FbxBlendShape(표정 같이 미리 만든것)의 부모 타입
		
	// 메시에서 스킨 꺼내기
	const int32_t iSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);

	for (int32_t iSkin = 0; iSkin < iSkinCount; ++iSkin)
	{
		auto* pSkin = static_cast<FbxSkin*>(pMesh->GetDeformer(iSkin, FbxDeformer::eSkin));

		if (nullptr != pSkin && pSkin->GetClusterCount() > 0)
		{
			// 메시의 클러스터 개수
			int32_t pClusterCount = pSkin->GetClusterCount();

			// 스킨에 대한 정보 저장용 생성
			FBX_SKIN_DATA SkinData{};

			for (int32_t iCluster = 0; iCluster < pClusterCount; ++iCluster)
			{
				// 스킨의 클러스터 꺼내기
				FbxCluster* pCluster = pSkin->GetCluster(iCluster);

				// 클러스터의 정보 꺼내서 저장하기
				// GetLink : 클러스터에 연결된 뼈
				// GetControlPointIndicesCount : 클러스터의 뼈가 영향을 주는 정점의 개수
				if (nullptr != pCluster &&
					nullptr != pCluster->GetLink() &&
					pCluster->GetControlPointIndicesCount() > 0)
				{
					const char_t*		pBoneName = pCluster->GetLink()->GetName(); // 뼈이름
					const int32_t		iClusterVertexCount = pCluster->GetControlPointIndicesCount(); // 정점 개수
					const int32_t*		pPointIndices = pCluster->GetControlPointIndices(); // 정점 번호 목록

					FBX_CLUSTER_DATA ClusterData{};
					ClusterData.strBoneName = pBoneName;

					const f64_t* pWeights = pCluster->GetControlPointWeights();

					for (int32_t iClusterVertex = 0; iClusterVertex < iClusterVertexCount; ++iClusterVertex)
					{
						ClusterData.FbxPointWeights.push_back({pPointIndices[iClusterVertex], pWeights[iClusterVertex]});
					}

					// 클러스터 정보 저장
					SkinData.FbxClusters.push_back(ClusterData);

					// 노드에서 공통 뼈대 읽어오기
					FbxNode* pBoneNode = pCluster->GetLink();
				}
			}

			// 스킨 데이터 저장하기
			MeshData.FbxSkins.push_back(SkinData);
		}
	}
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