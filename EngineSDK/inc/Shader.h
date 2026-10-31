#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class CShader final
{
private:
	CShader(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext);

	CShader(const CShader&) = delete;
	CShader& operator=(const CShader&) = delete;

public:
	~CShader() = default;

public:
	HRESULT		Initialize_Shader(const wstring_t& strFilePath);
	void		Bind_Shader();
	HRESULT		Update_Transform(const TRANSFORM& Transform);

private:
	ComPtr<ID3D11Device> m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

	// ID3D11VertexShader : 정점 셰이더
	ComPtr<ID3D11VertexShader> m_pVertexShader = { nullptr };

	// ID3D11PixelShader : 픽셀 셰이더
	ComPtr<ID3D11PixelShader> m_pPixelShader = { nullptr };

	// ID3D11InputLayout : 정점 버퍼의 위치/색상을 HLSL 입력에 연결하는 설명서
	ComPtr<ID3D11InputLayout> m_pInputLayout = { nullptr };

	// 상수 버퍼(월드/뷰/투영 행렬 변환) 버퍼
	ComPtr<ID3D11Buffer> m_pTransformBuffer = { nullptr };

public:
	static unique_ptr<CShader> Create(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext, const wstring_t& strFilePath);
};

NS_END