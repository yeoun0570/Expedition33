#include "Shader.h"

CShader::CShader(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext)
    : CComponent { pDevice, pContext }
{
}

HRESULT CShader::Initialize_Prototype(const wchar_t* pShaderFilePath, const char_t* pEntryPoint, const char_t* pVersion)
{
    if (FAILED(D3DCompileFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, pEntryPoint, pVersion, 0, 0, &m_pShaderByteCode, nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
    return S_OK;
}
