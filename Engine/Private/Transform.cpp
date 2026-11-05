#include "Transform.h"

CTransform::CTransform(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext)
	: CComponent { pDevice, pContext }
{
}

HRESULT CTransform::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	if (nullptr == pArg)
		return E_FAIL;

	auto	pDesc = static_cast<TRANSFORM_DESC*>(pArg);

	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRotationPerSec = XMConvertToRadians(pDesc->fRotationDegreePerSec);

	return S_OK;
}

void CTransform::SetUp_Scale(f32_t fScaleX, f32_t fScaleY, f32_t fScaleZ)
{
	Set_State(STATE::RIGHT, XMVector3Normalize(Get_State(STATE::RIGHT)) * fScaleX);
	Set_State(STATE::UP, XMVector3Normalize(Get_State(STATE::UP)) * fScaleY);
	Set_State(STATE::LOOK, XMVector3Normalize(Get_State(STATE::LOOK)) * fScaleZ);
}

void CTransform::Go_Straight(const f32_t fTimeDelta)
{
	vector_t vPosition = Get_State(STATE::POSITION);
	vector_t vLook = Get_State(STATE::LOOK);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Backward(const f32_t fTimeDelta)
{
	vector_t vPosition = Get_State(STATE::POSITION);
	vector_t vLook = Get_State(STATE::LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Right(const f32_t fTimeDelta)
{
	vector_t vPosition = Get_State(STATE::POSITION);
	vector_t vRight = Get_State(STATE::RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Left(const f32_t fTimeDelta)
{
	vector_t vPosition = Get_State(STATE::POSITION);
	vector_t vRight = Get_State(STATE::RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

shared_ptr<CTransform> CTransform::Create(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext)
{
	auto    pInstance = unique_ptr<CTransform>(new CTransform(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CPrototype> CTransform::Clone(void* pArg)
{
	auto    pInstance = unique_ptr<CTransform>(new CTransform(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform");
		pInstance.reset();
	}

	return pInstance;
}
