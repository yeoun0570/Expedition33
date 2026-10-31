#pragma once
#include "Prototype.h"

NS_BEGIN(Engine)

class ENGINE_DLL CComponent : public CPrototype
{
protected:
	CComponent(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);

public:
	virtual ~CComponent() = default;

public:
	virtual void Update_Component(const f32_t fTimeDelta);
	virtual void LateUpdate_Component(const f32_t fTimeDelta);

private:
	shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END