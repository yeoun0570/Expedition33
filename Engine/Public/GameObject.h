#pragma once
#include "Prototype.h"
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject : public CPrototype
{
protected:
	CGameObject(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);

public:
	virtual ~CGameObject() = default;

public:
	shared_ptr<CComponent> Get_Component(COMPONENTID eID, const wstring_t& strComponentTag);

public:
	virtual			HRESULT		Initialize_GameObject();
	virtual			void		Update_GameObject(const f32_t fTimeDelta);
	virtual			void		LateUpdate_GameObject(const f32_t fTimeDelta);
	virtual			void		Render_GameObject() = 0;
	virtual			void		Add_Component(void* pArg) = 0;
	
protected:
	map<wstring_t, shared_ptr<CComponent>>			m_Components[ETOUI(COMPONENTID::END)];

private:
	shared_ptr<CComponent> Find_Component(COMPONENTID eID, const wstring_t& strComponentTag);
	shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END