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
	HRESULT Initialize_Prototype(const vector<MESH_DATA>& MeshDatas,const vector<FBX_NODE_DATA>& NodeDatas, const map<string, string>& JointMap);
	HRESULT Initialize(void* pArg) override;
	void	Render();
	HRESULT Assemble(const CModel& SourceModel, const map<wstring_t, shared_ptr<CModel>>& PartModels);
	HRESULT Set_Animation(const shared_ptr<FBX_ANIM_DATA>& pAnimData);
	HRESULT Play_Animation(f64_t fTimeDelta);

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
	map<string, string>					m_JointMap;		// 내 뼈 이름 → 공통 뼈대에서 따라갈 뼈 이름 (없으면 같은 이름 사용)

	shared_ptr<FBX_ANIM_DATA>			m_pAnimData;			// 재생할 클립
	vector<int32_t>						m_ChannelNodeIndices;	// 채널 번호 → 모델 노드 번호
	f64_t								m_fAnimTime = 0.0;		// 현재 재생 위치(초)
	vector<float4x4_t>					m_GlobalMatrices;		// 공통 뼈대의 현재 글로벌 자세

private:
	void	Build_BoneMapping(const CModel& SourceModel, vector<int32_t>& SourceNodeIndices) const;
	HRESULT Build_DefaultPose(const CModel& SourceModel, const vector<int32_t>& SourceNodeIndices, vector<float4x4_t>& NodeMatrices) const;
	HRESULT Bind_Source(const CModel& SourceModel);
	HRESULT Calculate_ClusterMatrix(const FBX_CLUSTER_DATA& ClusterData, const float4x4_t& NodeMatrix,float4x4_t& SkinMatrix) const;
	HRESULT Calculate_MeshVertices(uint32_t iMeshIndex, vector<VTXMESH>& Vertices) const;

	HRESULT Calculate_GlobalMatrices(const vector<float4x4_t>& LocalMatrices, vector<float4x4_t>& GlobalMatrices) const;
	void	Get_DefaultLocalMatrices(vector<float4x4_t>& LocalMatrices) const;
	HRESULT	Sample_LocalMatrices(f64_t fTime, vector<float4x4_t>& LocalMatrices) const;
	HRESULT	Apply_SourcePose(const vector<float4x4_t>& SourceGlobalMatrices);
	HRESULT	Update_MeshVertices();

public:
	int32_t	Find_BoneIndex(const string& strBoneName) const;

public:
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
	static shared_ptr<CModel> Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext, const vector<MESH_DATA>& MeshDatas, const vector<FBX_NODE_DATA>& NodeDatas, const map<string, string>& JointMap);

};

NS_END