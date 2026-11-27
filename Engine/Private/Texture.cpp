#include "Texture.h"

CTexture::CTexture(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext)
	: CComponent { pDevice, pContext}
{
}

HRESULT CTexture::Initialize_Prototype(const wchar_t* pTextureFilePath, uint32_t iNumTextures)
{
	m_iNumSRVs = iNumTextures;

	m_SRVs.reserve(iNumTextures);

	for (uint32_t i = 0; i < iNumTextures; ++i)
	{
		wchar_t		szTextureFilePath[MAX_PATH] = {};

		wsprintf(szTextureFilePath, pTextureFilePath, i);

		wchar_t szEXT[MAX_PATH] = {};

		_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

		HRESULT hr = {};

		ComPtr<ID3D11ShaderResourceView>		pSRV = { nullptr };

		if (false == lstrcmp(szEXT, TEXT(".dds")))
			hr = CreateDDSTextureFromFile(m_pDevice.Get(), szTextureFilePath, nullptr, &pSRV);

		else if (false == lstrcmp(szEXT, TEXT(".tga")))
			return E_FAIL;

		else
			hr = CreateWICTextureFromFile(m_pDevice.Get(), szTextureFilePath, nullptr, &pSRV);

		if(FAILED(hr))
			return E_FAIL;

		m_SRVs.push_back(pSRV);
	}

	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_PixelShader(uint32_t iSlot, uint32_t iIndex)
{
	if (iIndex >= m_iNumSRVs)
		return E_FAIL;

	ID3D11ShaderResourceView* pSRVs[1] =
	{
		m_SRVs[iIndex].Get(),
	};

	m_pContext->PSSetShaderResources(iSlot, 1, pSRVs);

	return S_OK;
}

HRESULT CTexture::Bind_PixelShader(uint32_t iSlot)
{
	if (m_iNumSRVs >= D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT)
		return E_FAIL;

	ID3D11ShaderResourceView* pSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {nullptr};

	for (uint32_t i = 0; i < m_iNumSRVs; ++i)
	{
		pSRVs[i] = m_SRVs[i].Get();
	}

	m_pContext->PSSetShaderResources(iSlot, m_iNumSRVs, pSRVs);

	return S_OK;
}

shared_ptr<CTexture> CTexture::Create(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext, const wchar_t* pTextureFilePath, uint32_t iNumTextures)
{
	auto    pInstance = shared_ptr<CTexture>(new CTexture(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Created : CTexture");
		pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CPrototype> CTexture::Clone(void* pArg)
{
	auto    pInstance = shared_ptr<CTexture>(new CTexture(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
		pInstance.reset();
	}

	return pInstance;
}