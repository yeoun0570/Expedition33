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
}


#endif // Engine_Struct_h__
