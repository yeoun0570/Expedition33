#pragma once

#include "Engine_Defines.h"
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer //: public CComponent
{
protected:
	CVIBuffer(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);
	CVIBuffer(const CVIBuffer&) = delete;
	CVIBuffer& operator=(const CVIBuffer&) = delete;

public:
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT		Initialize_Buffer();
	virtual void		Render_Buffer();

protected:
	ComPtr<ID3D11Device>		m_pDevice;	// 버퍼 생성 그래픽 컴객체
	ComPtr<ID3D11DeviceContext> m_pContext; // 버퍼 활용 그래픽 컴객체

	ComPtr<ID3D11Buffer> m_pVB = nullptr;	// 정점 버퍼
	ComPtr<ID3D11Buffer> m_pIB = nullptr;	// 인덱스 버퍼

	uint32_t		m_iVtxSize	= 0;		// 정점의 크기
	uint32_t		m_iVtxCnt	= 0;		// 정점의 개수
	uint32_t		m_iIdxCnt	= 0;		// 인덱스의 개수

	DXGI_FORMAT m_eIndexFormat = DXGI_FORMAT_R32_UINT; // 인덱스 한 개의 자료형

	D3D11_PRIMITIVE_TOPOLOGY m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; // 점들을 어떤 도형으로 연결할지 지정
};

NS_END