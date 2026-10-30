#include "Prototype.h"

CPrototype::CPrototype(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
	: m_pDevice { pDevice },
	m_pContext { pContext } 
{
}

HRESULT CPrototype::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPrototype::Initialize(void* pArg)
{
	return S_OK;
}
