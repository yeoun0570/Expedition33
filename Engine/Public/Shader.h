#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CShader abstract : public CComponent
{
protected:
	CShader(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext);

public:
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const wchar_t* pShaderFilePath, const char_t* pEntryPoint, const char_t* pVersion);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Begin() = 0;
	virtual HRESULT End() = 0;

protected:
	ComPtr<ID3DBlob>		m_pShaderByteCode = { nullptr };

public:
	virtual shared_ptr<CPrototype> Clone(void* pArg) = 0;
};

NS_END