#pragma once

#include "Prototype.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CPrototype
{
public:
	// 오브젝트의 DESC 구조체들은 사본에서만 사용하는 추가 데이터들
	typedef struct tagGameObjectDesc
	{
		/*GameObject 생성 시에 필요한 데이터 받아온다.*/
		uint32_t iData = {};
	}GAMEOBJECT_DESC;

protected:
	CGameObject(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);

public:
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(const f32_t fTimeDelta);
	virtual void Update(const f32_t fTimeDelta);
	virtual void Late_Update(const f32_t fTimeDelta);
	virtual HRESULT Render();

protected:
	uint32_t			m_iData = {};

public:
	virtual shared_ptr<CPrototype> Clone(void* pArg) = 0;
};

NS_END