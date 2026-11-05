#pragma once

#include "Engine_Defines.h"
#include "MeshCol.h"

NS_BEGIN(Engine)

class CModel final
{
private:
	CModel(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);
	CModel(const CModel&) = delete;
	CModel& operator=(const CModel&) = delete;

public:
	~CModel() = default;

public:
	HRESULT Initialize(const vector<MESH_DATA>& MeshDatas, const vector<FBX_NODE_DATA>& NodeDatas);
	void	Render();

private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

	vector<shared_ptr<CMeshCol>>	m_vecMeshes;
	vector<FBX_NODE_DATA>			m_vecNodeDatas;

public:
	static shared_ptr<CModel> Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext, const vector<MESH_DATA>& MeshDatas, const vector<FBX_NODE_DATA>& NodeDatas);

};

NS_END