#include "Shader.h"

CShader::CShader(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext)
	:m_pDevice(pDevice),
	m_pContext(pContext)
{
}

HRESULT CShader::Initialize_Shader(const wstring_t& strFilePath)
{
    if (m_pDevice == nullptr || m_pContext == nullptr)
        return E_FAIL;

    if (strFilePath.empty())
        return E_INVALIDARG;

    // HLSL 컴파일러 옵션
    // D3DCOMPILE_ENABLE_STRICTNESS : HLSL 문법을 엄격하게 검사
    uint32_t iFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
    iFlags |= D3DCOMPILE_DEBUG;
    iFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    // ===============================================================================//
    // ===============================================================================//
    // ===============================================================================//
 
    // 정점 셰이더 객체 생성

    // 컴파일 결과와 오류 메시지를 받을 공간
    // ID3DBlob : 임의의 바이트 데이터를 담는 컴객체
    ComPtr<ID3DBlob> pVSCode = nullptr;
    ComPtr<ID3DBlob> pError = nullptr;

    // D3DCompileFromFile() : HLSL 파일 컴파일 하는 함수
    HRESULT hr = D3DCompileFromFile(
        strFilePath.c_str(),                // 읽을 HLSL 파일 경로
        nullptr,                            //
        D3D_COMPILE_STANDARD_FILE_INCLUDE,  //
        "VS_MAIN",                          // 실행 시작점으로 컴파일할 함수 이름
        "vs_5_0",                           // 정점 셰이더, 셰이더 모델 5.0
        iFlags,                             // 엄격한 검사·디버깅 등의 컴파일 옵션
        0,                                  //
        pVSCode.ReleaseAndGetAddressOf(),   // 컴파일 결과 코드를 받을 주소
        pError.ReleaseAndGetAddressOf());   // 컴파일 오류 메시지를 받을 주소

    // 오류났을 경우
    if (FAILED(hr))
    {
        if (pError != nullptr)
        {
            MessageBoxA(nullptr, 
                        static_cast<const char*>(pError->GetBufferPointer()),
                        "정점 셰이더 컴파일 실패",
                        MB_OK);
        }
        else
        {
            MSG_BOX("셰이더 파일 경로와 접근 권한을 확인하세요.");
        }

        return hr;
    }

    // 셰이더 객체 생성
    // 이 주소부터 이 크기만큼 있는 컴파일 결과를 사용해 정점 셰이더를 만들라는 뜻
    hr = m_pDevice->CreateVertexShader(
        pVSCode->GetBufferPointer(),                // 위에서 HLSL 컴파일한 결과 데이터 시작 주소
        pVSCode->GetBufferSize(),                   // 위에서 HLSL 컴파일한 결과 크기
        nullptr,
        m_pVertexShader.ReleaseAndGetAddressOf());

    if (FAILED(hr))
        return hr;

    // ===============================================================================//
    // ===============================================================================//
    // ===============================================================================//


    // 픽셀 셰이더 객체 생성

    ComPtr<ID3DBlob> pPSCode = nullptr;

    hr = D3DCompileFromFile(
        strFilePath.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "PS_MAIN",
        "ps_5_0",
        iFlags,
        0,
        pPSCode.GetAddressOf(),
        pError.ReleaseAndGetAddressOf());

    if (FAILED(hr))
    {
        if (pError != nullptr)
        {
            MessageBoxA(
                nullptr,
                static_cast<const char*>(pError->GetBufferPointer()),
                "픽셀 셰이더 컴파일 실패",
                MB_OK);
        }
        else
        {
            MSG_BOX("픽셀 셰이더 컴파일에 실패했습니다.");
        }

        return hr;
    }

    hr = m_pDevice->CreatePixelShader(
        pPSCode->GetBufferPointer(),
        pPSCode->GetBufferSize(),
        nullptr,
        m_pPixelShader.ReleaseAndGetAddressOf());

    if (FAILED(hr))
        return hr;

    // ===============================================================================//
    // ===============================================================================//
    // ===============================================================================//

    // 입력 레이아웃 생성
    // 레이아웃이란 GPU에게 정점 데이터를 어떻게 읽을지 알려주는 설명서

    D3D11_INPUT_ELEMENT_DESC Elements[2]{};

    // 위치 데이터 설명

    // SemanticName : HLSL의 POSITION, COLOR와 연결
    // SemanticIndex : 시맨틱(이름표) 번호 POSTION, COLOR 이름이 다르니까 번호도 그냥 둘다 0번
    // Format : 읽어올 바이트 크기와 성분, 자료형 설정(위치는 float형 3개를 32바이트 씩이니까 총 12바이트, 색은 똑같은데 4개니까 총 16바이트)
    // AlignedByteOffset : C++에서 읽기 시작할 위치


    Elements[0].SemanticName = "POSITION";
    Elements[0].SemanticIndex = 0;
    Elements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    Elements[0].InputSlot = 0;
    Elements[0].AlignedByteOffset =
        static_cast<uint32_t>(offsetof(VTXCOL, vPosition));
    Elements[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    Elements[0].InstanceDataStepRate = 0;

    // 색상 데이터 설명
    Elements[1].SemanticName = "COLOR";
    Elements[1].SemanticIndex = 0;
    Elements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    Elements[1].InputSlot = 0;
    Elements[1].AlignedByteOffset =
        static_cast<uint32_t>(offsetof(VTXCOL, vColor));
    Elements[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    Elements[1].InstanceDataStepRate = 0;


    // VS_MAIN의 매개변수는 VS_INPUT이다.
    // 그러므로 VS_MAIN이 매개변수로 입력 받는게 VS_INPUT이니까 입력 레이아웃을 VS_INPUT에 전달한다.
    hr = m_pDevice->CreateInputLayout(
        Elements,
        2,
        pVSCode->GetBufferPointer(),             // VS_MAIN을 컴파일한 코드
        pVSCode->GetBufferSize(),
        m_pInputLayout.ReleaseAndGetAddressOf());

    if (FAILED(hr))
        return hr;

    return S_OK;
}

void CShader::Bind_Shader()
{
    if (m_pContext == nullptr ||
        m_pInputLayout == nullptr ||
        m_pVertexShader == nullptr ||
        m_pPixelShader == nullptr)
        return;

    // 정점 버퍼를 읽는 방법 연결
    m_pContext->IASetInputLayout(m_pInputLayout.Get());

    // 정점 처리 단계에선 m_pVertexShader객체를 사용하라는 의미
    // 쉽게 표현하면 만든 정점 셰이더 객체를 정점 셰이더에 연결
    m_pContext->VSSetShader(
        m_pVertexShader.Get(),
        nullptr,
        0);

    // 픽셀 처리 단계에선 m_pPixelShader객체를 사용하라는 의미
    // 쉽게 표현하자면 만든 픽셀 셰이더 객체를 픽셀 셰이더에 연결
    m_pContext->PSSetShader(
        m_pPixelShader.Get(),
        nullptr,
        0);
}

unique_ptr<CShader> CShader::Create(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext, const wstring_t& strFilePath)
{
    auto pInstance = unique_ptr<CShader>(new CShader(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Shader(strFilePath)))
    {
        MSG_BOX("Failed to Created : CShader");
        pInstance.reset();
    }

    return pInstance;
}
