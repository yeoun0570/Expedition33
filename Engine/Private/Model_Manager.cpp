#include <fstream>
#include <json.hpp>

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
	m_pAnimations = make_shared<ANIMATIONS[]>(iNumLevels);

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

	
	// FBX 옆의 JSON에서 뼈 매핑 표 읽기 (JSON이 없으면 빈 표)
	map<string, string> JointMap;
	if (FAILED(Load_JointMap(pFilePath, JointMap)))
		return E_FAIL;

	// 가져온 Fbx파일 메시 데이터, 노드들을 모델에 연결해서 생성
	auto pModel = CModel::Create(m_pDevice, m_pContext, pLoader->Get_MeshDatas(), pLoader->Get_NodeDatas(), JointMap);
	if (nullptr == pModel)
		return E_FAIL;

	// 생성한 모델을 컴포넌트 원형으로도 등록합니다.
	if (FAILED(CGameInstance::Get().Add_Prototype(iModelLevelIndex, strModelTag, pModel)))
		return E_FAIL;
	

	// 해당 모델을 관리하기 위해 map에 저장
	m_pModels[iModelLevelIndex].emplace(strModelTag, pModel);


#ifdef _DEBUG
	{
		// [임시] FBX 옆의 같은 이름 JSON 읽기 시험 (예: Faces/Default.fbx → Faces/Default.fbx.json)
		const string strJsonPath = string(pFilePath) + ".json";

		ifstream File(strJsonPath);
		if (File.is_open())
		{
			const nlohmann::json Json = nlohmann::json::parse(File, nullptr, false);	// false: 문법 오류여도 예외 대신 discarded 값

			if (!Json.is_discarded() && Json.contains("jointMapping"))
			{
				const auto& JointMapping = Json["jointMapping"];

				char_t szLog[256]{};
				sprintf_s(szLog, "[Json] %s : jointMapping %zu개\n", strJsonPath.c_str(), JointMapping.size());
				OutputDebugStringA(szLog);

				// 처음 3개만 찍어 보기
				for (size_t i = 0; i < JointMapping.size() && i < 3; ++i)
				{
					const string strJoint = JointMapping[i].value("joint", "");
					const string strFrom = JointMapping[i].value("copyWorldFrom", "");

					sprintf_s(szLog, "    %s -> %s\n", strJoint.c_str(), strFrom.c_str());
					OutputDebugStringA(szLog);
				}
			}
		}
	}
#endif

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

HRESULT CModel_Manager::Assemble_Model(const shared_ptr<CModel>& pSourceModel, const map<wstring_t, shared_ptr<CModel>>& PartModels, shared_ptr<CModel>& pOut)
{
	if (nullptr == pSourceModel)
		return E_FAIL;

	// 외형 메시 없이 기준 노드만 복사해 전체 모델을 만든다.
	// 조립 모델은 공통 뼈대 역할만 하니까 매핑 표가 필요 없다.
	auto pModel = CModel::Create(m_pDevice, m_pContext, vector<MESH_DATA>{}, pSourceModel->Get_NodeDatas(), map<string, string>{});

	if (nullptr == pModel)
		return E_FAIL;

	// 사용할 구성 요소로 초기화한다.
	if (FAILED(pModel->Initialize(nullptr)))
		return E_FAIL;

	// 전체 모델에 복사한 뼈대를 기준으로 기존 파츠들을 조립한다.
	if (FAILED(pModel->Assemble(*pModel, PartModels)))
		return E_FAIL;

	// 모두 성공했을 때만 완성된 모델을 전달한다.
	pOut = pModel;
	return S_OK;
}

HRESULT CModel_Manager::Add_Animation(const wstring_t& strAnimTag, const char* pFilePath, uint32_t iLevelIndex)
{
	/* 애니메이션 FBX를 한 번만 읽어서 태그로 보관 */

	if (strAnimTag.empty() || nullptr == pFilePath || pFilePath[0] == '\0')
		return E_FAIL;

	if (nullptr != Find_Animation(iLevelIndex, strAnimTag))
		return E_FAIL;

	auto pLoader = CFbxLoader::Create();
	if (nullptr == pLoader)
		return E_FAIL;

	if (FAILED(pLoader->Load_Animation(pFilePath)) || pLoader->Get_AnimDatas().empty())
		return E_FAIL;

	// 파일 하나에 클립 하나라고 보고 첫 번째 클립 보관
	m_pAnimations[iLevelIndex].emplace(strAnimTag, make_shared<FBX_ANIM_DATA>(pLoader->Get_AnimDatas()[0]));

	return S_OK;
}

HRESULT CModel_Manager::Set_Animation(const shared_ptr<CModel>& pModel, const wstring_t& strAnimTag, uint32_t iLevelIndex)
{
	/* 보관해 둔 애니메이션을 모델에 연결 */

	if (nullptr == pModel)
		return E_FAIL;

	auto pAnimData = Find_Animation(iLevelIndex, strAnimTag);
	if (nullptr == pAnimData)
		return E_FAIL;

	return pModel->Set_Animation(pAnimData);
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

shared_ptr<FBX_ANIM_DATA> CModel_Manager::Find_Animation(uint32_t iLevelIndex, const wstring_t& strAnimTag)
{
	auto iter = m_pAnimations[iLevelIndex].find(strAnimTag);
	if (iter == m_pAnimations[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

HRESULT CModel_Manager::Load_JointMap(const char* pFbxFilePath, map<string, string>& JointMap) const
{
	/* "모델.fbx" 옆의 "모델.fbx.json"에서 jointMapping(joint → copyWorldFrom) 읽기 */
	JointMap.clear();

	ifstream File(string(pFbxFilePath) + ".json");

	// JSON이 없는 모델도 있으니 실패가 아니라 빈 표로 처리
	if (!File.is_open())
		return S_OK;

	const nlohmann::json Json = nlohmann::json::parse(File, nullptr, false);
	if (Json.is_discarded())
		return E_FAIL;		// 파일은 있는데 형식이 깨진 경우

	if (!Json.contains("jointMapping"))
		return S_OK;

	for (const auto& Mapping : Json["jointMapping"])
	{
		const string strJoint = Mapping.value("joint", "");
		const string strFrom = Mapping.value("copyWorldFrom", "");

		if (strJoint.empty() || strFrom.empty())
			continue;

		JointMap.emplace(strJoint, strFrom);
	}

	return S_OK;
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

