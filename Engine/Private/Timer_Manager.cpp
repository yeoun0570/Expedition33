#include "Timer_Manager.h"
#include "Timer.h"

CTimer_Manager::CTimer_Manager()
{
}

f32_t CTimer_Manager::Compute_TimeDelta(const wstring_t& strTimerTag)
{
	auto	pTimer = Find_Timer(strTimerTag);
	if (nullptr == pTimer)
		return 0.f;

	return pTimer->Compute_Timer();	
}

HRESULT CTimer_Manager::Add_Timer(const wstring_t& strTimerTag)
{
	if (nullptr != Find_Timer(strTimerTag))
		return E_FAIL;

	auto		pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_Timers.emplace(strTimerTag, pTimer);

	return S_OK;
}

shared_ptr<class CTimer> CTimer_Manager::Find_Timer(const wstring_t& strTimerTag)
{
	auto	iter = m_Timers.find(strTimerTag);

	if (iter == m_Timers.end())
		return nullptr;

	return iter->second;
}

