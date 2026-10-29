#pragma once

#include "Engine_Defines.h"

/*
1. 현재 게임내에서 보여줘야할 레벨의 주소를 들고 있는다.
2. 새로운 레벨 할당 시, 기존레벨을 삭제해준다.
3. 기존레벨 삭제 시, 기존 레벨용 자원을 정리해준다.
4. 현재 레벨의 반복적인 함수 호출을 수행해준다.
*/

NS_BEGIN(Engine)

class CLevel_Manager final
{
private:
	CLevel_Manager();

public:
	~CLevel_Manager() = default;

public:
	HRESULT Change_Level(uint32_t iLevelIndex, shared_ptr<class CLevel> pNewLevel);
	void Update(f32_t fTimeDelta);
	HRESULT Render();

private:
	shared_ptr<class CLevel>		m_pCurrentLevel = { nullptr };
	uint32_t						m_iCurrentLevelIndex = {};

public:
	static unique_ptr<CLevel_Manager> Create();
};

NS_END