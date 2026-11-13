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