#include "Model.h"

CModel::CModel(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
    : m_pDevice { pDevice },
    m_pContext { pContext }
{
}

HRESULT CModel::Initialize(const vector<MESH_DATA>& MeshDatas, const vector<FBX_NODE_DATA>& NodeDatas)
{
    if (m_pDevice == nullptr || m_pContext == nullptr)
        return E_FAIL;

    if (MeshDatas.empty())
        return E_INVALIDARG;

    // 이미 초기화된 객체의 중복 생성을 방지
    if (!m_Meshes.empty())
        return E_FAIL;

    for (const MESH_DATA& MeshData : MeshDatas)
    {
        m_Meshes.push_back(CVIBuffer_Mesh::Create(m_pDevice, m_pContext, MeshData));

        if (m_Meshes.back() == nullptr)
        {
            m_Meshes.pop_back();
            return E_FAIL;
        }
    }

    m_NodeDatas = NodeDatas;

    return S_OK;
}

void CModel::Render()
{
    for (const auto& pMesh : m_Meshes)
    {
        pMesh->Begin();
        pMesh->Render();
        pMesh->End();
    }
}

shared_ptr<CModel> CModel::Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext, const vector<MESH_DATA>& MeshDatas, const vector<FBX_NODE_DATA>& NodeDatas)
{
    auto pInstance = shared_ptr<CModel>(new CModel(pDevice, pContext));

    if (FAILED(pInstance->Initialize(MeshDatas, NodeDatas)))
    {
        MSG_BOX("Failed to Created : CModel");
        pInstance.reset();
    }

    return pInstance;
}
