#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CBackGround final : public CUIObject
{
public:
	typedef struct tagBackGroundDesc : public CUIObject::UIOBJECT_DESC
	{
		float4_t		vColor;
	}BACKGROUND_DESC;
private:
	CBackGround(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);
public:
	virtual ~CBackGround() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(f32_t fTimeDelta) override;
	virtual void Update(f32_t fTimeDelta) override;
	virtual void Late_Update(f32_t fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	float4_t			m_vColor = {};

	shared_ptr<CTexture>		m_pTextureCom = { nullptr };
	shared_ptr<CVIBuffer_Rect>	m_pVIBufferCom = { nullptr };
	shared_ptr<CShader>			m_pShaderCom[ETOUI(SHADER::END)] = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_Shader_Resources();
	HRESULT Release_Shader_Resources();

public:
	static shared_ptr<CBackGround> Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END
