#include "MeshCol.h"

CMeshCol::CMeshCol(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
	: CVIBuffer(pDevice, pContext)
{
}

HRESULT CMeshCol::Initialize_Buffer(const MESH_DATA& MeshData)
{
    if (MeshData.vecVerticles.empty() || MeshData.vecIndices.empty())
        return E_FAIL;

    // 개수를 uint32_t로 바꾸기 전에 범위 검사
    if (MeshData.vecVerticles.size() > UINT32_MAX ||
        MeshData.vecIndices.size() > UINT32_MAX)
        return E_FAIL;

    // 삼각형 목록이므로 인덱스는 3개씩 있어야 함
    if (MeshData.vecIndices.size() % 3 != 0)
        return E_FAIL;

    // 각 인덱스가 실제 정점을 가리키는지 검사
    for (uint32_t iIndex : MeshData.vecIndices)
    {
        if (iIndex >= MeshData.vecVerticles.size())
            return E_FAIL;
    }

    m_iVtxSize = static_cast<uint32_t>(sizeof(VTXCOL));
    m_iVtxCnt = static_cast<uint32_t>(MeshData.vecVerticles.size());
    m_iIdxCnt = static_cast<uint32_t>(MeshData.vecIndices.size());

    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // 기존 함수로 GPU 버퍼 공간 생성
    if (FAILED(CVIBuffer::Initialize_Buffer()))
        return E_FAIL;

    // 추출한 정점·인덱스를 GPU 버퍼에 복사
    m_pContext->UpdateSubresource(m_pVB.Get(), 
                                0, 
                                nullptr,
                                MeshData.vecVerticles.data(), 
                                0, 
                                0);

    m_pContext->UpdateSubresource(m_pIB.Get(), 
                                0, 
                                nullptr,
                                MeshData.vecIndices.data(), 
                                0, 
                                0);

	return S_OK;
}

void CMeshCol::Render_Buffer()
{
    CVIBuffer::Render_Buffer();
}

unique_ptr<CMeshCol> CMeshCol::Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext, const MESH_DATA& MeshData)
{
    auto pInstance = unique_ptr<CMeshCol>(new CMeshCol(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Buffer(MeshData)))
    {
        MSG_BOX("Failed to Created : CMeshCol");
        pInstance.reset();
    }

    return pInstance;
}
