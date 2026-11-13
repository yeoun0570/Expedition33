#include "VIBuffer_Mesh.h"

CVIBuffer_Mesh::CVIBuffer_Mesh(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
	: CVIBuffer(pDevice, pContext)
{
}

HRESULT CVIBuffer_Mesh::Initialize_Prototype(const MESH_DATA& MeshData)
{
    if (MeshData.Verticles.empty() || MeshData.Indices.empty())
        return E_FAIL;

    // 개수를 uint32_t로 바꾸기 전에 범위 검사
    if (MeshData.Verticles.size() > UINT32_MAX ||
        MeshData.Indices.size() > UINT32_MAX)
        return E_FAIL;

    // 삼각형 목록이므로 인덱스는 3개씩 있어야 함
    if (MeshData.Indices.size() % 3 != 0)
        return E_FAIL;

    // 각 인덱스가 실제 정점을 가리키는지 검사
    for (uint32_t iIndex : MeshData.Indices)
    {
        if (iIndex >= MeshData.Verticles.size())
            return E_FAIL;
    }

#pragma region Public

    m_iNumVertexBuffers = 1;
    m_iVertexStride = static_cast<uint32_t>(sizeof(VTXCOL));
    m_iIndexStride = 4;
    m_iNumVertices = static_cast<uint32_t>(MeshData.Verticles.size());
    m_iNumIndices = static_cast<uint32_t>(MeshData.Indices.size());
    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma endregion


#pragma region Vertex Buffer

    D3D11_BUFFER_DESC VertexBufferDesc{};
    VertexBufferDesc.ByteWidth              = m_iVertexStride * m_iNumVertices;
    VertexBufferDesc.Usage                  = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags              = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags         = 0;
    VertexBufferDesc.MiscFlags              = 0;
    VertexBufferDesc.StructureByteStride    = m_iVertexStride;

    D3D11_SUBRESOURCE_DATA          VertexInitialData{
    .pSysMem = MeshData.Verticles.data(),
    };

    if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

#pragma endregion


#pragma region Index Buffer

    D3D11_BUFFER_DESC       IndexBufferDesc{};
    IndexBufferDesc.ByteWidth           = m_iIndexStride * m_iNumIndices;
    IndexBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
    IndexBufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.CPUAccessFlags      = 0;
    IndexBufferDesc.MiscFlags           = 0;
    IndexBufferDesc.StructureByteStride = m_iIndexStride;

    D3D11_SUBRESOURCE_DATA          IndexInitialData{
    .pSysMem = MeshData.Indices.data(),
    };

    if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
        return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Mesh::Initialize(void* pArg)
{
    return S_OK;
}

shared_ptr<CVIBuffer_Mesh> CVIBuffer_Mesh::Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext, const MESH_DATA& MeshData)
{
    auto pInstance = shared_ptr<CVIBuffer_Mesh>(new CVIBuffer_Mesh(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype(MeshData)))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Mesh");
        pInstance.reset();
    }

    return pInstance;
}

shared_ptr<CPrototype> CVIBuffer_Mesh::Clone(void* pArg)
{
    auto    pInstance = shared_ptr<CVIBuffer_Mesh>(new CVIBuffer_Mesh(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CVIBuffer_Mesh");
        pInstance.reset();
    }

    return pInstance;
}