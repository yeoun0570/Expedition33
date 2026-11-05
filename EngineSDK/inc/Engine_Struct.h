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

	typedef struct tagMeshData
	{
		vector<VTXCOL>		vecVerticles;
		vector<uint32_t>	vecIndices;
	}MESH_DATA;

	typedef struct tagFbxNodeData
	{
		string		strName;			// 현재 노드 이름
		int32_t		iParentIndex = -1;	// 부모 노드 번호(부모가 없으면 -1)
		bool_t		isBone = false;		// 노드에 뼈가 존재하는지
	}FBX_NODE_DATA;

	typedef struct tagTransform
	{
		// float4x4_t : 4x4 행렬 자료형
		float4x4_t	matWorld;
		float4x4_t	matView;
		float4x4_t	matProj;
	}TRANSFORM;
}


#endif // Engine_Struct_h__
