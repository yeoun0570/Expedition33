#include "Model.h"

CModel::CModel(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
    : m_pDevice { pDevice },
    m_pContext { pContext }
{
}

HRESULT CModel::Initialize(const vector<MESH_DATA>& MeshDatas)
{
    if (m_pDevice == nullptr || m_pContext == nullptr)
        return E_FAIL;

    if (MeshDatas.empty())
        return E_INVALIDARG;

    // 이미 초기화된 객체의 중복 생성을 방지
    if (!m_vecMeshes.empty())
        return E_FAIL;

    for (const MESH_DATA& MeshData : MeshDatas)
    {
        m_vecMeshes.push_back(CMeshCol::Create(m_pDevice, m_pContext, MeshData));

        if (m_vecMeshes.back() == nullptr)
        {
            m_vecMeshes.pop_back();
            return E_FAIL;
        }
    }

    return S_OK;
}

void CModel::Render()
{
    for (const auto& pMesh : m_vecMeshes)
    {
        pMesh->Render_Buffer();
    }
}

shared_ptr<CModel> CModel::Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext, const vector<MESH_DATA>& MeshDatas)
{
    auto pInstance = shared_ptr<CModel>(new CModel(pDevice, pContext));

    if (FAILED(pInstance->Initialize(MeshDatas)))
    {
        MSG_BOX("Failed to Created : CModel");
        pInstance.reset();
    }

    return pInstance;
}
