#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
	typedef struct tagUIObjectDesc
	{
		f32_t fX, fY, fSizeX, fSizeY;
	}UIOBJECT_DESC;

protected:
	CUIObject(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);

public:
	virtual ~CUIObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(const f32_t fTimeDelta) override;
	virtual void Update(const f32_t fTimeDelta) override;
	virtual void Late_Update(const f32_t fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	f32_t			m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};

protected:
	virtual shared_ptr<CPrototype> Clone(void* pArg) = 0;
};

NS_END