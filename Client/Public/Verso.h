#pragma once

#include "Character.h"

NS_BEGIN(Client)

class CVerso final : public CCharacter
{
public:
	typedef struct tagVersoDesc : public CCharacter::CHARACTER_DESC
	{

	}VERSO_DESC;

private:
	CVerso(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);

public:
	virtual ~CVerso() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(const f32_t fTimeDelta) override;
	virtual void Update(const f32_t fTimeDelta) override;
	virtual void Late_Update(const f32_t fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static shared_ptr<CVerso> Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END