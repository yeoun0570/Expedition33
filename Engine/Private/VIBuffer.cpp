#include "VIBuffer.h"

CVIBuffer::CVIBuffer(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
	: CComponent { pDevice, pContext }
{
}

HRESULT CVIBuffer::Initialize_Prototype()
{
    return S_OK;

}

HRESULT CVIBuffer::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CVIBuffer::Begin()
{
    ID3D11Buffer* pVertexBuffers[] =
    {
        m_pVB.Get(),
    };

    uint32_t iVertexStrides[] =
    {
        m_iVertexStride,
    };

    uint32_t iOffsets[] =
    {
        0,
    };

    // IASetVertexBuffers() == SetStreamSource()
    m_pContext->IASetVertexBuffers(0,
        m_iNumVertexBuffers,
        pVertexBuffers,
        iVertexStrides,
        iOffsets);

    // 2. 인덱스 버퍼 연결
    // IASetIndexBuffer() == SetIndices()
    m_pContext->IASetIndexBuffer(m_pIB.Get(), m_eIndexFormat, 0);

    // 3. 인덱스 3개마다 삼각형 하나로 해석
    // 그리기 함수에 D3DPT_TRIANGLELIST 전달
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

    return S_OK;
}

HRESULT CVIBuffer::Render()
{
    // 4. 인덱스 개수만큼 그리기
    // DrawIndexedPrimitive() == DrawIndexed()
    m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

    return S_OK;
}

HRESULT CVIBuffer::End()
{
    ID3D11Buffer* pVertexBuffers[] = {
    nullptr
    };

    uint32_t		iVertexStrides[] = {
        0,
    };

    uint32_t		iOffsets[] = {
        0,
    };

    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
    m_pContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    m_pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED);

    return S_OK;
}