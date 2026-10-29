#include "Model_Manager.h"
#include "FbxLoader.h"

CModel_Manager::CModel_Manager(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
	: m_pDevice { pDevice },
	m_pContext { pContext }
{
}

HRESULT CModel_Manager::Add_Model(const wstring_t& strModelTag, const char* pFilePath)
{
	if (nullptr != Find_Model(strModelTag))
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

	// 가져온 Fbx파일 메시 데이터들을 모델에 연결해서 생성
	auto pModel = CModel::Create(m_pDevice, m_pContext, pLoader->Get_MeshDatas());
	if (nullptr == pModel)
		return E_FAIL;

	// 해당 모델을 관리하기 위해 map에 저장
	m_Models.emplace(strModelTag, pModel);

	return S_OK;
}

HRESULT CModel_Manager::Render_Model(const wstring_t& strModelTag)
{
	auto	pModel = Find_Model(strModelTag);
	if (nullptr == pModel)
		return E_FAIL;
	
	pModel->Render();

	return S_OK;
}

shared_ptr<CModel> CModel_Manager::Find_Model(const wstring_t& strModelTag)
{
	auto	iter = m_Models.find(strModelTag);

	if (iter == m_Models.end())
		return nullptr;

	return iter->second;
}

unique_ptr<CModel_Manager> CModel_Manager::Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
{
	return unique_ptr<CModel_Manager>(new CModel_Manager(pDevice, pContext));
}

