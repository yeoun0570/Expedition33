#include "CTimerMgr.h"

IMPLEMENT_SINGLETON(CTimer_Manager)


CTimer_Manager::CTimer_Manager()
{
}

CTimer_Manager::~CTimer_Manager()
{
	Free();
}

_float CTimer_Manager::Get_TimeDelta(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);

	if (pTimer == nullptr)
		return 0.f;

	return pTimer->Get_TimeDelta();
}

void CTimer_Manager::Set_TimeDelta(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);

	if (pTimer == nullptr)
		return;

	pTimer->Update_Timer();
}

HRESULT CTimer_Manager::Ready_Timer(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);

	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_mapTimer.insert({ pTimerTag, pTimer });

	return S_OK;
}

CTimer* CTimer_Manager::Find_Timer(const _tchar* pTimerTag)
{
	auto		iter = find_if(m_mapTimer.begin(), m_mapTimer.end(), CTag_Finder(pTimerTag));

	if (iter == m_mapTimer.end())
		return nullptr;
	
	return iter->second;
}

void CTimer_Manager::Free()
{
	for_each(m_mapTimer.begin(), m_mapTimer.end(), CDeleteMap());
	m_mapTimer.clear();
}
