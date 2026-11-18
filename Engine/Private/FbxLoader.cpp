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
		return E_FAIL;

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

HRESULT CFbxLoader::Load_Animation(const char* pFilePath)
{
	/* 파일 읽을 준비와 저장할 공간 준비 */

	if (nullptr == m_pManager)
		return E_FAIL;


	if (pFilePath == nullptr || pFilePath[0] == '\0')
		return E_FAIL;


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

	// 파일 내용을 Scene에 실제로 채우기
	if (!pImporter->Import(pScene))
	{
		pImporter->Destroy();
		pScene->Destroy();
		return E_FAIL;
	}

	pImporter->Destroy();

	/* ================================================================================= */
	
	/*
	FbxAnimStack: 애니메이션 클립 하나(예: Idle 하나). 보통 파일 하나에 1개 존재.
	FbxTimeSpan: 클립의 시작 시간과 끝 시간.
	TimeMode: 프레임 속도(30fps, 60fps 등).
	*/

	/* 애니메이션 가져오기 */

	// 1. 애니메이션 클립 개수
	const int32_t iStackCount = pScene->GetSrcObjectCount<FbxAnimStack>();

	// 2. 뼈대 읽기 
	if (FAILED(Read_BoneNodes(pScene)))
		return E_FAIL;

	m_AnimDatas.clear();

	// 3. 클립마다 키프레임 일정한 간격으로 뽑기
	// 일정한 프레임이여야 특정 시간에 뼈가 무슨 자세인지 알 수 있음

	// 키프레임 간격 설정(30fps : 언리얼에서 자주 사용되는 키프레임)
	const f64_t fSampleRate = 30.0;


	for (int32_t iStack = 0; iStack < iStackCount; ++iStack)
	{
		// 클립
		FbxAnimStack* pAnimStack = pScene->GetSrcObject<FbxAnimStack>(iStack);

		// Fbx로컬 행렬을 이 클립을 기준으로 계산하도록 지정
		pScene->SetCurrentAnimationStack(pAnimStack);

		// 클립의 시간
		const FbxTimeSpan Span = pAnimStack->GetLocalTimeSpan();
		const f64_t fStart = Span.GetStart().GetSecondDouble();
		const f64_t fDuration = Span.GetDuration().GetSecondDouble(); // 이 클립의 총(끝) 길이

		// 클립 구조체에 저장
		FBX_ANIM_DATA AnimData{};
		AnimData.strName = pAnimStack->GetName();
		AnimData.fDuration = fDuration;
		AnimData.fSampleRate = fSampleRate;
		// 0초 프레임 + 끝 프레임까지 포함하려고 +1
		AnimData.iFrameCount = static_cast<uint32_t>(fDuration * fSampleRate) + 1;

		// 노드에서 해당 시간의 뼈의 로컬 행렬을 뽑기

		// (1) 뼈 노드를 먼저 찾아 두고, 채널 공간을 준비
		vector<FbxNode*> BoneNodes;

		for (const FBX_NODE_DATA& NodeData : m_NodeDatas)
		{
			if (!NodeData.isBone)
				continue;

			FbxNode* pNode = pScene->FindNodeByName(NodeData.strName.c_str());
			if (nullptr == pNode)
				continue;

			BoneNodes.push_back(pNode);

			// 뼈 하나의 시간별 로컬 행렬 구조체
			FBX_ANIM_CHANNEL Channel{};
			Channel.strBoneName = NodeData.strName;
			Channel.LocalMatrices.resize(AnimData.iFrameCount);
			AnimData.Channels.push_back(Channel);
		}

		// (2) 시간마다 → 모든 뼈 (같은 시간을 연속으로 물어서 SDK 캐시 활용)
		for (uint32_t iFrame = 0; iFrame < AnimData.iFrameCount; ++iFrame)
		{
			// 프레임당 몇초 걸리는지 구하기
			FbxTime Time;
			Time.SetSecondDouble(fStart + static_cast<f64_t>(iFrame) / fSampleRate);


			for (uint64_t iBone = 0; iBone < BoneNodes.size(); ++iBone)
			{
				const FbxAMatrix LocalMatrix = BoneNodes[iBone]->EvaluateLocalTransform(Time);

				float4x4_t& Dst = AnimData.Channels[iBone].LocalMatrices[iFrame];

				for (int32_t iRow = 0; iRow < 4; ++iRow)
				{
					for (int32_t iCol = 0; iCol < 4; ++iCol)
						Dst.m[iRow][iCol] = static_cast<f32_t>(LocalMatrix.Get(iRow, iCol));
				}
			}
		}

		m_AnimDatas.push_back(AnimData);
	}

	uint32_t iBoneCount = 0;
	for (const FBX_NODE_DATA& NodeData : m_NodeDatas)
	{
		if (NodeData.isBone)
			++iBoneCount;
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

	vector<FbxNode*> Nodes;
	Nodes.push_back(pRootNode);

	FBX_NODE_DATA	RootData
	{
		pRootNode->GetUniqueID(),
		pRootNode->GetName(),
		-1,
		pRootNode->GetSkeleton() != nullptr,
	};

	m_NodeDatas.push_back(RootData);


	// 나머지 노드들 데이터 m_vecNodeDatas에 저장
	for (uint64_t iNodeIndex = 0; iNodeIndex < Nodes.size(); ++iNodeIndex)
	{
		FbxNode* pParentNode = Nodes[iNodeIndex];

		for (int32_t iChildIndex = 0; iChildIndex < pParentNode->GetChildCount(); ++iChildIndex)
		{
			FbxNode* pChildNode = pParentNode->GetChild(iChildIndex);

			Nodes.push_back(pChildNode);


			FBX_NODE_DATA	ChildData
			{
				pChildNode->GetUniqueID(),
				pChildNode->GetName(),
				static_cast<int32_t>(iNodeIndex),
				pChildNode->GetSkeleton() != nullptr,
			};

			m_NodeDatas.push_back(ChildData);
		}
	}

	// 노드에 Fbx 로컬, 글로벌(월드) 행렬 저장
	// 뼈 매핑에 사용할 공통 뼈대 저장
	for (uint64_t iNode = 0; iNode < Nodes.size(); ++iNode)
	{
		// EvaluateLocalTransform : Fbx 로컬 행렬 가져오기
		// EvaluateGlobalTransform : Fbx 글로벌(월드) 행렬 가져오기
		FbxAMatrix	LocalMatrix = Nodes[iNode]->EvaluateLocalTransform();
		FbxAMatrix	GlobalMatrix = Nodes[iNode]->EvaluateGlobalTransform();

		// 가져온 Fbx 로컬, 글로벌(월드) 행렬 저장
		for (uint32_t iRow = 0; iRow < 4; ++iRow)
		{
			for (uint32_t iCol = 0; iCol < 4; ++iCol)
			{
				m_NodeDatas[iNode].LocalMatrix[iRow][iCol] = LocalMatrix.Get(iRow, iCol);
				m_NodeDatas[iNode].GlobalMatrix[iRow][iCol] = GlobalMatrix.Get(iRow, iCol);
			}
		}
	}

	return S_OK;
}

HRESULT CFbxLoader::Read_MeshVertexIndex(FbxMesh* pMesh, MESH_DATA& MeshData)
{
	/* 컨트롤 포인트(위치) 저장 */
	const int32_t iPointCount = pMesh->GetControlPointsCount();
	const FbxVector4* pPoints = pMesh->GetControlPoints();

	if (iPointCount <= 0 || nullptr == pPoints)
		return E_FAIL;

	MeshData.Points.resize(static_cast<uint64_t>(iPointCount));

	for (int32_t iPoint = 0; iPoint < iPointCount; ++iPoint)
	{
		MeshData.Points[iPoint] =
		{
			static_cast<f32_t>(pPoints[iPoint][0]),
			static_cast<f32_t>(pPoints[iPoint][1]),
			static_cast<f32_t>(pPoints[iPoint][2])
		};
	}

	/* ======================================================================== */

	/* 폴리곤 꼭짓점 저장 */

	// 첫 번째 UV 세트 이름 (UV를 읽을 때 필요)
	FbxStringList UVSetNames;
	pMesh->GetUVSetNames(UVSetNames);
	const char_t* pUVSetName = (UVSetNames.GetCount() > 0) ? UVSetNames[0].Buffer() : nullptr;

	const int32_t iPolygonCount = pMesh->GetPolygonCount();

	// 삼각형(폴리곤) 하나당 정점이 3개니까 * 3
	MeshData.Verticles.reserve(static_cast<uint64_t>(iPolygonCount) * 3);
	MeshData.PointIndices.reserve(static_cast<uint64_t>(iPolygonCount) * 3);
	MeshData.Indices.reserve(static_cast<uint64_t>(iPolygonCount) * 3);


	// 삼각형 꼭짓점 하나 = GPU 정점 하나
	for (int32_t iPolygon = 0; iPolygon < iPolygonCount; ++iPolygon)
	{
		if (pMesh->GetPolygonSize(iPolygon) != 3)
		{
			MSG_BOX("삼각형이 아닌 면이 존재합니다.");
			return E_FAIL;
		}

		// Corner == 꼭짓점
		for (int32_t iCorner = 0; iCorner < 3; ++iCorner)
		{
			// 이 꼭짓점이 사용하는 컨트롤 포인트 번호
			const int32_t iPointIndex = pMesh->GetPolygonVertex(iPolygon, iCorner);

			if (iPointIndex < 0 || iPointIndex >= iPointCount)
			{
				MSG_BOX("잘못된 정점 번호가 있습니다.");
				return E_FAIL;
			}

			VTXMESH Vertex{};
			// 위치 : 컨트롤 포인트가 위치니까 구조체의 vPosition에 삼각형 정점의 컨트롤 포인트 대입
			Vertex.vPosition = MeshData.Points[iPointIndex]; 

			// 법선 : 이 삼각형의 꼭짓점 기준
			FbxVector4 Normal;
			if (pMesh->GetPolygonVertexNormal(iPolygon, iCorner, Normal))
			{
				Vertex.vNormal = 
				{ 
					static_cast<f32_t>(Normal[0]), 
					static_cast<f32_t>(Normal[1]),
					static_cast<f32_t>(Normal[2]),
				};
			}

			// UV : 이 삼각형의 꼭짓점 기준
			FbxVector2 UV;
			bool_t Unmapped = false;

			if (nullptr != pUVSetName && pMesh->GetPolygonVertexUV(iPolygon, iCorner, pUVSetName, UV, Unmapped))
			{
				Vertex.vTexCoord =
				{
					static_cast<f32_t>(UV[0]),
					1.f - static_cast<f32_t>(UV[1]), // DX에서는 y가 반대(V가 아래로 갈수록 커짐)
				};
			}

			// 새 정점 번호 == 지금까지 쌓인 정점 수
			MeshData.Indices.push_back(static_cast<uint32_t>(MeshData.Verticles.size()));
			// 구조체에 넣어놓은 삼각형 정보(위치, 법선, UV) 저장
			MeshData.Verticles.push_back(Vertex);
			// 정점의 위치 번호 정보 저장
			MeshData.PointIndices.push_back(static_cast<uint32_t>(iPointIndex));
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
					const uint64_t		iBoneID = pCluster->GetLink()->GetUniqueID();
					const char_t*		pBoneName = pCluster->GetLink()->GetName(); // 뼈이름
					const int32_t		iClusterVertexCount = pCluster->GetControlPointIndicesCount(); // 정점 개수
					const int32_t*		pPointIndices = pCluster->GetControlPointIndices(); // 정점 번호 목록

					FBX_CLUSTER_DATA ClusterData{};
					ClusterData.iBoneID = iBoneID;
					ClusterData.strBoneName = pBoneName;

					FbxAMatrix MeshBind;
					FbxAMatrix BoneBind;

					pCluster->GetTransformMatrix(MeshBind);			// 메시 행렬
					pCluster->GetTransformLinkMatrix(BoneBind);		// 뼈 행렬

					// 클러스터의 메시 형렬과 뼈 행렬 저장
					for (uint32_t iRow = 0; iRow < 4; ++iRow)
					{
						for (uint32_t iCol = 0; iCol < 4; ++iCol)
						{
							ClusterData.MeshBindMatrix[iRow][iCol] = MeshBind.Get(iRow, iCol);
							ClusterData.BoneBindMatrix[iRow][iCol] = BoneBind.Get(iRow, iCol);
						}
					}

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