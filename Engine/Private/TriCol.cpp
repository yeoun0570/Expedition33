#include "TriCol.h"

CTriCol::CTriCol(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CVIBuffer(pDevice, pContext)
{
}

HRESULT CTriCol::Initialize_Buffer()
{
    m_iVtxSize = static_cast<uint32_t>(sizeof(VTXCOL));
    m_iVtxCnt = 3;
    m_iIdxCnt = 3;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// 1. 부모 클래스에서 GPU 버퍼 공간 생성
	if (FAILED(CVIBuffer::Initialize_Buffer()))
		return E_FAIL;

	// 2. CPU 메모리에 정점 데이터 작성
    vector<VTXCOL> Vertices(m_iVtxCnt);

	Vertices[0].vPosition = { 0.f, 1.f, 0.f };
	Vertices[0].vColor = { 0.f, 1.f, 0.f, 1.f };

	Vertices[1].vPosition = { 1.f, -1.f, 0.f };
	Vertices[1].vColor = { 0.f, 1.f, 0.f, 1.f };

	Vertices[2].vPosition = { -1.f, -1.f, 0.f };
	Vertices[2].vColor = { 0.f, 1.f, 0.f, 1.f };

    // 3. 점을 연결할 번호 작성
    vector<uint32_t> Indices(m_iIdxCnt);

    for (uint32_t i = 0; i < m_iIdxCnt; ++i)
    {
        Indices[i] = i;
    }

    // 4. GPU의 정점 버퍼에 복사
    // UpdateSubresource()는 CPU의 데이터를 GPU 리소스로 복사하는 함수
    // Vertices, Indices를 GPU에 복사. CPU에 있는 이녀석들은 지역변수니까 함수 끝나면 자동으로 사라짐.

    m_pContext->UpdateSubresource(m_pVB.Get(),      // 데이터를 받을 GPU 버퍼(복사할 목적지)!!!
                                0,                  // 대상 하위 리소스 번호. 버퍼는 0 사용
                                nullptr,            // 일부 영역을 지정하지 않고 전체 갱신
                                Vertices.data(),    // 복사할 원본(복사할 시작 주소)!!!
                                0,                  // 버퍼에는 필요하지 않은 행·깊이 간격
                                0);

    // 5. GPU의 인덱스 버퍼에 복사
    m_pContext->UpdateSubresource(m_pIB.Get(),
                                0,
                                nullptr,
                                Indices.data(),
                                0,
                                0);

	return S_OK;
}

void CTriCol::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

unique_ptr<CTriCol> CTriCol::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    auto    pInstance = unique_ptr<CTriCol>(new CTriCol(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Buffer()))
    {
        MSG_BOX("Failed to Created : CTriCol");
        pInstance.reset();
    }

    return pInstance;
}