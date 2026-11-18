#include "ConstantBuffer.h"

CConstantBuffer::CConstantBuffer(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice { pDevice },
    m_pContext { pContext }
{
}

HRESULT CConstantBuffer::Initialize(const void* pData, uint32_t iLength)
{
    D3D11_BUFFER_DESC       BufferDesc
    {
        .ByteWidth = iLength,
        .Usage = D3D11_USAGE_DYNAMIC,
        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
        .MiscFlags = 0,
        .StructureByteStride = iLength,
    };

    D3D11_SUBRESOURCE_DATA  InitialData
    {
        .pSysMem = pData,
    };

    if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &InitialData, &m_pCB)))
        return E_FAIL;

    return S_OK;
}

HRESULT CConstantBuffer::Update(const void* pData, uint32_t iLength)
{
    D3D11_MAPPED_SUBRESOURCE        SubResource{};

    if (FAILED(m_pContext->Map(m_pCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
        return E_FAIL;

    memcpy(SubResource.pData, pData, iLength);

    m_pContext->Unmap(m_pCB.Get(), 0);

    return S_OK;
}

HRESULT CConstantBuffer::Bind_VertexShader(uint32_t iSlot)
{
    ID3D11Buffer* pCB[] =
    {
        m_pCB.Get(),
    };

    m_pContext->VSSetConstantBuffers(iSlot, 1, pCB);

    return S_OK;
}

HRESULT CConstantBuffer::Bind_PixelShader(uint32_t iSlot)
{
    ID3D11Buffer* pCB[] =
    {
        m_pCB.Get(),
    };

    m_pContext->PSSetConstantBuffers(iSlot, 1, pCB);

    return S_OK;
}

shared_ptr<CConstantBuffer> CConstantBuffer::Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext, const void* pData, uint32_t iLength)
{
    auto    pInstance = shared_ptr<CConstantBuffer>(new CConstantBuffer(pDevice, pContext));

    if (FAILED(pInstance->Initialize(pData, iLength)))
    {
        MSG_BOX("Failed to Created : CConstantBuffer");
        pInstance.reset();
    }

    return pInstance;
}
