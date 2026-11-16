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
	HRESULT Assemble_Model(const shared_ptr<CModel>& pSourceModel, const map<wstring_t, shared_ptr<CModel>>& PartModels, shared_ptr<CModel>& pOut);
	HRESULT	Add_Animation(const wstring_t& strAnimTag, const char* pFilePath, uint32_t iLevelIndex);
	HRESULT	Set_Animation(const shared_ptr<CModel>& pModel, const wstring_t& strAnimTag, uint32_t iLevelIndex);
	void	Clear(uint32_t iClearLevelIndex);

private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

	uint32_t		m_iNumLevels = {};
	shared_ptr<map<const wstring_t, shared_ptr<CModel>>[]>		m_pModels;
	typedef map<const wstring_t, shared_ptr<CModel>>			MODELS;

	shared_ptr<map<const wstring_t, shared_ptr<FBX_ANIM_DATA>>[]>	m_pAnimations;
	typedef map<const wstring_t, shared_ptr<FBX_ANIM_DATA>>			ANIMATIONS;

private:
	shared_ptr<CModel>			Find_Model(uint32_t iModelLevelIndex, const wstring_t& strModelTag);
	shared_ptr<FBX_ANIM_DATA>	Find_Animation(uint32_t iLevelIndex, const wstring_t& strAnimTag);

	HRESULT Load_JointMap(const char* pFbxFilePath, map<string, string>& JointMap) const;

public:
	static unique_ptr<CModel_Manager> Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext, uint32_t iNumLevels);
};

NS_END