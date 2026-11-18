#include "PixelShader.h"

CPixelShader::CPixelShader(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext)
    : CShader { pDevice, pContext }
{
}

HRESULT CPixelShader::Initialize_Prototype(const wchar_t* pShaderFilePath, const char_t* pEntryPoint, const char_t* pVersion)
{
    if (FAILED(__super::Initialize_Prototype(pShaderFilePath, pEntryPoint, pVersion)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreatePixelShader(m_pShaderByteCode->GetBufferPointer(), m_pShaderByteCode->GetBufferSize(), nullptr, &m_pShader)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPixelShader::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CPixelShader::Begin()
{
    m_pContext->PSSetShader(m_pShader.Get(), nullptr, 0);

    return S_OK;
}

HRESULT CPixelShader::End()
{
    m_pContext->PSSetShader(nullptr, nullptr, 0);

    return S_OK;
}

shared_ptr<CPixelShader> CPixelShader::Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext, const wchar_t* pShaderFilePath, const char_t* pEntryPoint, const char_t* pVersion)
{
    auto    pInstance = shared_ptr<CPixelShader>(new CPixelShader(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pEntryPoint, pVersion)))
    {
        MSG_BOX("Failed to Created : CPixelShader");
        pInstance.reset();
    }

    return pInstance;
}

shared_ptr<CPrototype> CPixelShader::Clone(void* pArg)
{
    auto    pInstance = shared_ptr<CPixelShader>(new CPixelShader(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CPixelShader");
        pInstance.reset();
    }

    return pInstance;
}
