#include "Model_Manager.h"
#include "FbxLoader.h"

#include "GameInstance.h"

CModel_Manager::CModel_Manager(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
	: m_pDevice { pDevice },
	m_pContext { pContext }
{
}

HRESULT CModel_Manager::Initialize(uint32_t iNumLevels)
{
	m_iNumLevels = iNumLevels;

	m_pModels = make_shared<MODELS[]>(iNumLevels);

	return S_OK;
}

HRESULT CModel_Manager::Add_Model(const wstring_t& strModelTag, const char* pFilePath, uint32_t iModelLevelIndex)
{
	if (nullptr != Find_Model(iModelLevelIndex, strModelTag))
		return E_FAIL;

	if (strModelTag.empty())
		return E_FAIL;

	if (m_pDevice == nullptr || m_pContext == nullptr)
		return E_FAIL;

	if (pFilePath == nullptr || pFilePath[0] == '\0')
		return E_FAIL;

	// 이번 파일을 읽을 Fbx 로더 생성
	auto pLoader = CFbxLoader::Create();

	if (pLoader == nullptr)
		return E_FAIL;

	if (FAILED(pLoader->Load_Model(pFilePath)))
	{
		MSG_BOX("모델 파일 읽기에 실패했습니다.");
		return E_FAIL;
	}

	// 가져온 Fbx파일 메시 데이터, 노드들을 모델에 연결해서 생성
	auto pModel = CModel::Create(m_pDevice, m_pContext, pLoader->Get_MeshDatas(), pLoader->Get_NodeDatas());
	if (nullptr == pModel)
		return E_FAIL;

	// 해당 모델을 관리하기 위해 map에 저장
	m_pModels[iModelLevelIndex].emplace(strModelTag, pModel);

	return S_OK;
}

HRESULT CModel_Manager::Render_Model(uint32_t iModelLevelIndex, const wstring_t& strModelTag)
{
	auto	pModel = Find_Model(iModelLevelIndex, strModelTag);
	if (nullptr == pModel)
		return E_FAIL;
	
	pModel->Render();

	return S_OK;
}

void CModel_Manager::Clear(uint32_t iClearLevelIndex)
{
	for (auto& Pair : m_pModels[iClearLevelIndex])
	{
		Pair.second.reset();
	}

	m_pModels[iClearLevelIndex].clear();
}

shared_ptr<CModel> CModel_Manager::Find_Model(uint32_t iModelLevelIndex, const wstring_t& strModelTag)
{
	auto    iter = m_pModels[iModelLevelIndex].find(strModelTag);
	if (iter == m_pModels[iModelLevelIndex].end())
		return nullptr;

	return iter->second;
}

unique_ptr<CModel_Manager> CModel_Manager::Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext, uint32_t iNumLevels)
{
	auto    pInstance = unique_ptr<CModel_Manager>(new CModel_Manager(pDevice, pContext));

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CModel_Manager");
		pInstance.reset();
	}

	return pInstance;
}

