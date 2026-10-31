#include "VIBuffer.h"

CVIBuffer::CVIBuffer(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
	: //CComponent { pDevice, pContext }
    m_pDevice(pDevice)
	, m_pContext(pContext)
{
}

HRESULT CVIBuffer::Initialize_Buffer()
{
    if (m_pDevice == nullptr || m_pContext == nullptr)
        return E_FAIL;

    if (m_iVtxSize == 0 || m_iVtxCnt == 0 || m_iIdxCnt == 0)
        return E_FAIL;

    // 버퍼 크기 계산 시 정수 범위를 넘지 않도록 검사
    if (m_iVtxCnt > UINT32_MAX / m_iVtxSize ||
        m_iIdxCnt > UINT32_MAX / sizeof(uint32_t))
        return E_FAIL;


    // 1. 정점 버퍼 생성
    D3D11_BUFFER_DESC VertexDesc{};

    VertexDesc.ByteWidth = m_iVtxSize * m_iVtxCnt;      // 전체 버퍼 크기
    VertexDesc.Usage = D3D11_USAGE_DEFAULT;             // GPU에서 사용하는 일반적인 버퍼
    VertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;    // 정점용인지 인덱스용인지 지정

    if (FAILED(m_pDevice->CreateBuffer(&VertexDesc, 
                                        nullptr,                            // 생성할 때 초기 데이터를 전달하지 않음
                                        m_pVB.ReleaseAndGetAddressOf())))   // 기존 버퍼 참조가 있으면 해제하고, 새 버퍼를 받을 주소를 제공
        return E_FAIL;

    // 2. 인덱스 버퍼 생성(정점 버퍼랑 거의 흡사)
    D3D11_BUFFER_DESC IndexDesc{};

    IndexDesc.ByteWidth = m_iIdxCnt * static_cast<uint32_t>(sizeof(uint32_t));
    IndexDesc.Usage = D3D11_USAGE_DEFAULT;
    IndexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    if (FAILED(m_pDevice->CreateBuffer(&IndexDesc, nullptr, m_pIB.ReleaseAndGetAddressOf())))
        return E_FAIL;

    return S_OK;

}

void CVIBuffer::Render_Buffer()
{
    if (m_pContext == nullptr ||
        m_pVB == nullptr || m_pIB == nullptr)
        return;

    const uint32_t iOffset = 0;

    // 1. 정점 버퍼 연결
    // DX11 함수 == DX9 함수 식으로 비교
    ID3D11Buffer* pVertexBuffer = m_pVB.Get();

    // IASetVertexBuffers() == SetStreamSource()
    m_pContext->IASetVertexBuffers( 0,
                                    1,
                                    &pVertexBuffer,
                                    &m_iVtxSize,
                                    &iOffset);

    // 2. 인덱스 버퍼 연결
    // IASetIndexBuffer() == SetIndices()
    m_pContext->IASetIndexBuffer(m_pIB.Get(), m_eIndexFormat, 0);

    // 3. 인덱스 3개마다 삼각형 하나로 해석
    // 그리기 함수에 D3DPT_TRIANGLELIST 전달
    m_pContext->IASetPrimitiveTopology(m_eTopology);

    // 4. 인덱스 개수만큼 그리기
    // DrawIndexedPrimitive() == DrawIndexed()
    m_pContext->DrawIndexed(m_iIdxCnt, 0, 0);
}