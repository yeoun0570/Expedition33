#pragma once

namespace Engine
{
	static const wstring	g_strTransformComTag = { TEXT("Com_Transform") };

	static const wstring			g_strLinearSamplerTag = { TEXT("Sampler_Linear") };
	static const D3D11_SAMPLER_DESC g_LinearSampler = {
		.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
		.MinLOD = 0.f,
		.MaxLOD = D3D11_FLOAT32_MAX
	};

	static const wstring			g_strPointSamplerTag = { TEXT("Sampler_Point") };
	static const D3D11_SAMPLER_DESC g_PointSampler = {
		.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
		.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
		.MinLOD = 0.f,
		.MaxLOD = D3D11_FLOAT32_MAX
	};

}