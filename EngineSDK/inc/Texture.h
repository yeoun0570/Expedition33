#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext);

public:
	virtual ~CTexture() = default;

public:
	HRESULT Initialize_Prototype(const wchar_t* pTextureFilePath, uint32_t iNumTextures);
	HRESULT Initialize(void* pArg) override;

private:
	uint32_t										m_iNumSRVs = {};
	vector<ComPtr<ID3D11ShaderResourceView>>		m_SRVs = { nullptr };


public:
	static shared_ptr<CTexture> Create(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext, const wchar_t* pTextureFilePath, uint32_t iNumTextures = 1);
	virtual shared_ptr<CPrototype> Clone(void* pArg);
};

NS_END