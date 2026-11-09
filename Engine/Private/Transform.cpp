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

void CTransform::Rotation(fvector_t vAxis, f32_t fAngle)
{
	float3_t vScaled = Get_Scale();

	vector_t vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	vector_t vUp	= XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	vector_t vLook	= XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	matrix_t RotationMatrix = XMMatrixRotationAxis(vAxis, XMConvertToRadians(fAngle));

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(f32_t fAngleX, f32_t fAngleY, f32_t fAngleZ)
{
	float3_t vScaled = Get_Scale();

	vector_t vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	vector_t vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	vector_t vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	vector_t vQuaternion = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(fAngleX), XMConvertToRadians(fAngleY), XMConvertToRadians(fAngleZ));

	matrix_t RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn(fvector_t vAxis, const f32_t fTimeDelta)
{
	vector_t        vRight = Get_State(STATE::RIGHT);
	vector_t        vUp = Get_State(STATE::UP);
	vector_t        vLook = Get_State(STATE::LOOK);

	matrix_t RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec + fTimeDelta);

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn(const f32_t fTimeDelta, bool_t isRotX, bool_t isRotY, bool_t isRotZ)
{
	vector_t        vRight = Get_State(STATE::RIGHT);
	vector_t        vUp = Get_State(STATE::UP);
	vector_t        vLook = Get_State(STATE::LOOK);

	vector_t vQuaternion = XMQuaternionRotationRollPitchYaw(isRotX * m_fRotationPerSec * fTimeDelta, 
															isRotY * m_fRotationPerSec * fTimeDelta, 
															isRotZ * m_fRotationPerSec * fTimeDelta);

	matrix_t RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::LookAt(fvector_t vAt)
{
	vector_t vPosition = Get_State(STATE::POSITION);

	vector_t		vLook = vAt - vPosition;
	vector_t		vRight = XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	vector_t		vUp = XMVector3Cross(vLook, vRight);

	float3_t		vScaled = Get_Scale();

	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE::UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Chase(fvector_t vTarget, const f32_t fTimeDelta, f32_t fLimitDistance)
{
	vector_t		vPosition = Get_State(STATE::POSITION);
	vector_t		vMoveDir = vTarget - vPosition;

	f32_t			fDistance = XMVectorGetX(XMVector3Length(vMoveDir));

	if (fDistance <= fLimitDistance)
		return;

	vPosition += XMVector3Normalize(vMoveDir) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

shared_ptr<CTransform> CTransform::Create(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext)
{
	auto    pInstance = shared_ptr<CTransform>(new CTransform(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CPrototype> CTransform::Clone(void* pArg)
{
	auto    pInstance = shared_ptr<CTransform>(new CTransform(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform");
		pInstance.reset();
	}

	return pInstance;
}
