#pragma once

#include "Client_Defines.h"

#include "GameObject.h"

class CCharacter abstract : public CGameObject
{
public:
	typedef struct tagCharacterDesc : public CGameObject::GAMEOBJECT_DESC
	{

	}CHARACTER_DESC;

protected:
	CCharacter(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);

public:
	virtual ~CCharacter() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(const f32_t fTimeDelta) override;
	virtual void Update(const f32_t fTimeDelta) override;
	virtual void Late_Update(const f32_t fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual shared_ptr<CPrototype> Clone(void* pArg) = 0;
};

