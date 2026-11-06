#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct tagTransformDesc
	{
		f32_t	fSpeedPerSec;
		f32_t	fRotationDegreePerSec;
	}TRANSFORM_DESC;

private:
	CTransform(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext);

public:
	virtual ~CTransform() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	vector_t Get_State(STATE eState) {
		return XMLoadFloat4x4(&m_WorldMatrix).r[ETOUI(eState)];
	}

	void Set_State(STATE eState, fvector_t vState) {
		XMStoreFloat4(reinterpret_cast<float4_t*>(m_WorldMatrix.m[ETOUI(eState)]), vState);
	}

	float3_t Get_Scale() {
		return float3_t(
			XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))),
			XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
			XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK))));
	}

public:
	void SetUp_Scale(f32_t fScaleX, f32_t fScaleY, f32_t fScaleZ);

	void Go_Straight(const f32_t fTimeDelta);
	void Go_Backward(const f32_t fTimeDelta);
	void Go_Right(const f32_t fTimeDelta);
	void Go_Left(const f32_t fTimeDelta);

	void Rotation(fvector_t vAxis, f32_t fAngle);
	void Rotation(f32_t fAngleX, f32_t fAngleY, f32_t fAngleZ);

	void Turn(fvector_t vAxis, const f32_t fTimeDelta);
	void Turn(const f32_t fTimeDelta, bool_t isRotX = false, bool_t isRotY = false, bool_t isRotZ = false);

	void LookAt(fvector_t vAt);
	void Chase(fvector_t vTarget, const f32_t fTimeDelta, f32_t fLimitDistance);

private:
	float4x4_t		m_WorldMatrix = {};
	f32_t			m_fSpeedPerSec = {};
	f32_t			m_fRotationPerSec = {};


public:
	static shared_ptr<CTransform> Create(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END