#pragma once

#include "Engine_Defines.h"
#include "GameObject.h"
#include "Component.h"

NS_BEGIN(Engine)

class CGameObject_Manager final
{
private:
	CGameObject_Manager();

public:
	~CGameObject_Manager() = default;

public:
	HRESULT		Initialize(uint32_t iNumLevels);
	void		Update_GameObject(uint32_t iNumLevels, const f32_t fTimeDelta);
	void		LateUpdate_GameObject(uint32_t iNumLevels, const f32_t fTimeDelta);
	void		Render_GameObject(uint32_t iNumLevels);

private:
	shared_ptr<map<const wstring_t, shared_ptr<CGameObject>>[]>			m_Objects;
	typedef map<const wstring_t, shared_ptr<CGameObject>>				OBJECTS;

public:
	static unique_ptr<CGameObject_Manager> Create(uint32_t iNumLevels);
};

NS_END