#pragma once

#include "Shader.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPixelShader final : public CShader
{
protected:
	CPixelShader(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext);

public:
	virtual ~CPixelShader() = default;

public:
	HRESULT Initialize_Prototype(const wchar_t* pShaderFilePath, const char_t* pEntryPoint, const char_t* pVersion) override;
	HRESULT Initialize(void* pArg) override;

public:
	HRESULT Begin() override;
	HRESULT End() override;

private:
	ComPtr<ID3D11PixelShader>		m_pShader = { nullptr };

public:
	static shared_ptr<CPixelShader> Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext, const wchar_t* pShaderFilePath, const char_t* pEntryPoint = "main", const char_t* pVersion = "ps_5_0");
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END