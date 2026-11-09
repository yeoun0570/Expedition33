#pragma once

#include "Client_Defines.h"

#include "UIObject.h"

NS_BEGIN(Client)

class CBackGround final : public CUIObject
{
public:
	typedef struct tagBackGroundDesc : public CUIObject::UIOBJECT_DESC
	{

	}BACKGROUND_DESC;

private:
	CBackGround(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);

public:
	virtual ~CBackGround() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(const f32_t fTimeDelta) override;
	virtual void Update(const f32_t fTimeDelta) override;
	virtual void Late_Update(const f32_t fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	float4_t				m_vColor = {};
	shared_ptr<CTexture>	m_pTextureCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CBackGround> Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END