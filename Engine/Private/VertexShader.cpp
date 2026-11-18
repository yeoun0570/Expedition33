#include "VertexShader.h"

CVertexShader::CVertexShader(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext)
    : CShader { pDevice, pContext }
{
}

HRESULT CVertexShader::Initialize_Prototype(const wchar_t* pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* pElements, uint32_t iNumElement, const char_t* pEntryPoint, const char_t* pVersion)
{
    if (FAILED(__super::Initialize_Prototype(pShaderFilePath, pEntryPoint, pVersion)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateVertexShader(m_pShaderByteCode->GetBufferPointer(), m_pShaderByteCode->GetBufferSize(), nullptr, &m_pShader)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElement, m_pShaderByteCode->GetBufferPointer(), m_pShaderByteCode->GetBufferSize(), &m_pInputLayout)))
        return E_FAIL;

    return S_OK;
}

HRESULT CVertexShader::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CVertexShader::Begin()
{
    m_pContext->VSSetShader(m_pShader.Get(), nullptr, 0);
    m_pContext->IASetInputLayout(m_pInputLayout.Get());

    return S_OK;
}

HRESULT CVertexShader::End()
{
    m_pContext->VSSetShader(nullptr, nullptr, 0);

    return S_OK;
}

shared_ptr<CVertexShader> CVertexShader::Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext, const wchar_t* pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* pElements, uint32_t iNumElement, const char_t* pEntryPoint, const char_t* pVersion)
{
    auto    pInstance = shared_ptr<CVertexShader>(new CVertexShader(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElements, iNumElement, pEntryPoint, pVersion)))
    {
        MSG_BOX("Failed to Created : CVertexShader");
        pInstance.reset();
    }

    return pInstance;
}

shared_ptr<CPrototype> CVertexShader::Clone(void* pArg)
{
    auto    pInstance = shared_ptr<CVertexShader>(new CVertexShader(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CVertexShader");
        pInstance.reset();
    }

    return pInstance;
}
