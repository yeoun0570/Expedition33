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
	f32_t Compute_TimeDelta(const _tchar* pTimerTag);

public:
	HRESULT		Ready_Timer(const _tchar* pTimerTag);

private:
	CTimer* Find_Timer(const _tchar* pTimerTag);

private:
	map<const _tchar*, CTimer*>								m_mapTimer;
	map<const wstring_t, shared_ptr<class CTimer>>			m_mapTimer;

private:
	virtual void Free();

};

NS_END