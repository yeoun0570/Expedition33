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
	HRESULT Initialize(uint32_t iNumLevels);
	HRESULT	Add_Model(const wstring_t& strModelTag, const char* pFilePath, uint32_t iModelLevelIndex);
	HRESULT	Render_Model(uint32_t iModelLevelIndex, const wstring_t& strModelTag);
	void	Clear(uint32_t iClearLevelIndex);

private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

	uint32_t		m_iNumLevels = {};
	shared_ptr<map<const wstring_t, shared_ptr<CModel>>[]>		m_pModels;
	typedef map<const wstring_t, shared_ptr<CModel>>			MODELS;

private:
	shared_ptr<CModel> Find_Model(uint32_t iModelLevelIndex, const wstring_t& strModelTag);

public:
	static unique_ptr<CModel_Manager> Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext, uint32_t iNumLevels);
};

NS_END