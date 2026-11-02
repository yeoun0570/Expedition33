#pragma once

#include "Prototype.h"

class ENGINE_DLL CComponent abstract : public CPrototype
{
protected:
	CComponent(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext);

public:
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual shared_ptr<CPrototype> Clone(void* pArg) = 0;
};

