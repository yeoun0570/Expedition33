#pragma once

#include "Component.h"

#include "Engine_Defines.h"
#include "VIBuffer_Mesh.h"

NS_BEGIN(Engine)

class CModel final : public CComponent
{
private:
	CModel(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);

public:
	~CModel() = default;

public:
	HRESULT Initialize_Prototype(const vector<MESH_DATA>& MeshDatas,const vector<FBX_NODE_DATA>& NodeDatas);
	HRESULT Initialize(void* pArg) override;
	void	Render();
	HRESULT Assemble(const CModel& SourceModel, const map<wstring_t, shared_ptr<CModel>>& PartModels);

	const vector<FBX_NODE_DATA>& Get_NodeDatas() const
	{
		return m_NodeDatas;
	}

private:
	vector<shared_ptr<CVIBuffer_Mesh>>	m_Meshes;
	vector<FBX_NODE_DATA>				m_NodeDatas;
	vector<MESH_DATA>					m_MeshDatas;
	map<string, int32_t>				m_BoneIndicies;
	vector<int32_t>						m_SourceNodeIndices;
	vector<float4x4_t>					m_NodeMatrices;
	map<wstring_t, shared_ptr<CModel>>	m_Parts;

private:
	void	Build_BoneMapping(const CModel& SourceModel, vector<int32_t>& SourceNodeIndices) const;
	HRESULT Build_DefaultPose(const CModel& SourceModel, const vector<int32_t>& SourceNodeIndices, vector<float4x4_t>& NodeMatrices) const;
	HRESULT Bind_Source(const CModel& SourceModel);
	HRESULT Calculate_ClusterMatrix(const FBX_CLUSTER_DATA& ClusterData, const float4x4_t& NodeMatrix,float4x4_t& SkinMatrix) const;
	HRESULT Calculate_MeshVertices(uint32_t iMeshIndex, vector<VTXCOL>& Vertices) const;

public:
	int32_t	Find_BoneIndex(const string& strBoneName) const;

public:
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
	static shared_ptr<CModel> Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext, const vector<MESH_DATA>& MeshDatas, const vector<FBX_NODE_DATA>& NodeDatas);

};

NS_END