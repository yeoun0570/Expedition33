#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTimer_Manager final
{
	DECLARE_SINGLETON(CTimer_Manager)

private:
	CTimer_Manager();
public:
	~CTimer_Manager() = default;

public:
	f32_t Compute_TimeDelta(const wstring_t& strTimerTag);

public:
	HRESULT	Add_Timer(const wstring_t& strTimerTag);

private:
	map<const wstring_t, shared_ptr<class CTimer>>		m_Timers;

private:
	shared_ptr<class CTimer> Find_Timer(const wstring_t& strTimerTag);
};

NS_END