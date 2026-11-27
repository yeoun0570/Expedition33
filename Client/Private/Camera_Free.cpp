#include "Camera_Free.h"

#include "GameInstance.h"

CCamera_Free::CCamera_Free(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
	: CCamera{ pDevice, pContext }
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		auto		pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);

		m_fMouseSensor = pDesc->fMouseSensor;
	}

	return S_OK;
}

void CCamera_Free::Priority_Update(f32_t fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CCamera_Free::Update(f32_t fTimeDelta)
{
	if(CGameInstance::Get().Get_DIKeyState(DIK_W) & 0x8000)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}

	if (CGameInstance::Get().Get_DIKeyState(DIK_A) & 0x8000)
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}

	if (CGameInstance::Get().Get_DIKeyState(DIK_S) & 0x8000)
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (CGameInstance::Get().Get_DIKeyState(DIK_D) & 0x8000)
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}

	int32_t iMouseMove = {};

	if (iMouseMove = CGameInstance::Get().Get_DIMouseMove(DIMM::X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * iMouseMove * m_fMouseSensor);
	}

	if (iMouseMove = CGameInstance::Get().Get_DIMouseMove(DIMM::Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * iMouseMove * m_fMouseSensor);
	}

	__super::Update(fTimeDelta);
}

void CCamera_Free::Late_Update(f32_t fTimeDelta)
{
	__super::Late_Update(fTimeDelta);


	CGameInstance::Get().Add_RenderGroup(RENDERGROUP::PRIORITY, dynamic_pointer_cast<CGameObject>(shared_from_this()));

}

shared_ptr<CCamera_Free> CCamera_Free::Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
{
	auto    pInstance = shared_ptr<CCamera_Free>(new CCamera_Free(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Free");
		pInstance.reset();
	}

	return pInstance;
}


shared_ptr<CPrototype> CCamera_Free::Clone(void* pArg)
{
	auto    pInstance = shared_ptr<CCamera_Free>(new CCamera_Free(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Free");
		pInstance.reset();
	}

	return pInstance;
}