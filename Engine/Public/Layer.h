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

private:
	list<shared_ptr<CGameObject>>			m_GameObjects;

public:
	static shared_ptr<CLayer> Create();
};

NS_END