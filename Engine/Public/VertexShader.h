#pragma once

#include "Shader.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVertexShader final : public CShader
{
protected:
	CVertexShader(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);

public:
	virtual ~CVertexShader() = default;

public:
	HRESULT Initialize_Prototype(const wchar_t* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, uint32_t iNumElement, const char_t* pEntryPoint, const char_t* pVersion);
	HRESULT Initialize(void* pArg) override;

public:
	HRESULT Begin() override;
	HRESULT End() override;

private:
	ComPtr<ID3D11VertexShader>		m_pShader = { nullptr };
	ComPtr<ID3D11InputLayout>		m_pInputLayout = { nullptr };

public:
	static shared_ptr<CVertexShader> Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext, const wchar_t* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, uint32_t iNumElement, const char_t* pEntryPoint = "main", const char_t* pVersion = "vs_5_0");
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END