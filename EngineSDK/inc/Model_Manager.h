#pragma once

#include "Model.h"

NS_BEGIN(Engine)

class CModel_Manager final
{
private:
	CModel_Manager(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);

public:
	~CModel_Manager() = default;

public:
	HRESULT	Add_Model(const wstring_t& strModelTag, const char* pFilePath);
	HRESULT	Render_Model(const wstring_t& strModelTag);


private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

	map<const wstring_t, shared_ptr<CModel>>		m_Models;

private:
	shared_ptr<CModel> Find_Model(const wstring_t& strModelTag);

public:
	static unique_ptr<CModel_Manager> Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);
};

NS_END