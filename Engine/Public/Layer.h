#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class CLayer
{
private:
	CLayer();

public:
	~CLayer() = default;

public:
	HRESULT Add_GameObject(shared_ptr<CGameObject> pGameObject);
	void Priority_Update(const f32_t fTimeDelta);
	void Update(const f32_t fTimeDelta);
	void Late_Update(const f32_t fTimeDelta);

private:
	list<shared_ptr<CGameObject>>			m_GameObjects;

public:
	static shared_ptr<CLayer> Create();
};

NS_END