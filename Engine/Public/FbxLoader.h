#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

// 클라이언트에서도 사용해야하니까 ENGINE_DLL 붙임
class CFbxLoader final
{
private:
	CFbxLoader();
	// 복사 생성, = 오버로딩 사용 금지. 객체 중복 생성 방지용.
	CFbxLoader(const CFbxLoader&) = delete;
	CFbxLoader& operator=(const CFbxLoader&) = delete;

public:
	~CFbxLoader();

public:
	HRESULT Initialize();

public:
	HRESULT Load_Model(const char* pFilePath);

	const vector<MESH_DATA>& Get_MeshDatas() const { return m_vecMeshDatas; }

private:
	FbxManager*				m_pManager	= nullptr;
	vector<FbxScene*>		m_vecScenes;
	vector<MESH_DATA>		m_vecMeshDatas;

public:
	static unique_ptr<CFbxLoader> Create();
};

NS_END