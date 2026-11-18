#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HWND			hWnd;
		WINMODE			eWinMode;
		uint32_t		iWinSizeX;
		uint32_t		iWinSizeY;
		uint32_t		iNumLevels;
	}ENGINE_DESC;


	typedef struct tagVertexColor
	{
		float3_t vPosition;
		float4_t vColor;
	} VTXCOL;

	typedef struct tagVertexPositionTexcoord
	{
		float3_t			vPosition;
		float2_t			vTexcoord;
	}VTXPOSTEX;

	typedef struct tagVertexMesh
	{
		float3_t		vPosition;		// 위치
		float2_t		vTexCoord;		// UV
		float3_t		vNormal;		// 면이 바라보는 방향
		float3_t		vTangent;		// 노멀맵
	}VTXMESH;
	
	typedef struct tagVertexAnimationMesh
	{
		float3_t		vPosition;			// 위치
		float2_t		vTexCoord;			// UV
		float3_t		vNormal;			// 면이 바라보는 방향
		float3_t		vTangent;			// 노멀맵
		uint32_t		vBlendIndices[4];	// 이 정점을 움직이는 뼈 번호(최대 4개)
		float4_t		vBlendWeights;		// 뼈별 가중치
	}VTXANIMMESH;

	typedef struct tagTransform
	{
		// float4x4_t : 4x4 행렬 자료형
		float4x4_t	matWorld;
		float4x4_t	matView;
		float4x4_t	matProj;
	}TRANSFORM;




	/* FBX를 위한 구조체 */

	// 클러스터 데이터들
	typedef struct tagFbxPointWeight
	{
		int32_t iPointIndex;
		f64_t fWeight;
	} FBX_POINT_WEIGHT;



	typedef struct tagFbxClusterData
	{
		string	strBoneName;
		vector<FBX_POINT_WEIGHT> FbxPointWeights;

		uint64_t		iBoneID;
		f64_t			MeshBindMatrix[4][4]{}; // 바인드 메시
		f64_t			BoneBindMatrix[4][4]{}; // 바인드 뼈

		int32_t			iNodeIndex = -1;	// 노드 연결용 인덱스
	} FBX_CLUSTER_DATA;



	typedef struct tagFbxSkinData
	{
		vector<FBX_CLUSTER_DATA> FbxClusters;
	} FBX_SKIN_DATA;



	// 정점 + 인덱스 + 클러스터합친 Fbx데이터
	typedef struct tagMeshData
	{
		vector<VTXMESH>				Verticles;
		vector<uint32_t>			Indices;
		vector<FBX_SKIN_DATA>		FbxSkins;
		vector<float3_t>			Points;			// 컨트롤 포인트 원래 위치 (스키닝 계산용)
		vector<uint32_t>			PointIndices;	// 정점 i → 원래 컨트롤 포인트(위치) 번호
	}MESH_DATA;


	// 뼈대 가져오기용
	typedef struct tagFbxNodeData
	{
		uint64_t	iNodeID;
		string		strName;				// 현재 노드 이름
		int32_t		iParentIndex = -1;		// 부모 노드 번호(부모가 없으면 -1)
		bool_t		isBone = false;			// 노드에 뼈가 존재하는지
		f64_t		LocalMatrix[4][4]{};	// FBX 기본 자세 행렬
		f64_t		GlobalMatrix[4][4]{};	// FBX 파일 기준 월드 행렬(게임에 반영되는 월드 행렬이 아니라 FBX 파일 안의 월드 행렬임)
	}FBX_NODE_DATA;


	/* FBX 애니메이션을 위한 구조체 */

	// 뼈 하나의 시간별 로컬 행렬
	typedef struct tagFbxAnimChannel
	{
		string				strBoneName;	// 뼈 이름 (몸체 뼈와 이름으로 연결)
		vector<float4x4_t>	LocalMatrices;	// 프레임마다 로컬 행렬 (부모 기준 자세)
	}FBX_ANIM_CHANNEL;


	// 애니메이션 클립 하나
	typedef struct tagFbxAnimData
	{
		string						strName;			// 클립 이름
		f64_t						fDuration = 0.0;	// 길이(초)
		f64_t						fSampleRate = 0.0;	// 샘플링 간격(초당 몇 번 찍었는지)
		uint32_t					iFrameCount = 0;	// 찍은 프레임 수
		vector<FBX_ANIM_CHANNEL>	Channels;			// 뼈별 데이터
	}FBX_ANIM_DATA;

}


#endif // Engine_Struct_h__
