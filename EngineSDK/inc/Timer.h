#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class CTimer final 
{
private:
	CTimer();
public:
	~CTimer() = default;

public:
	HRESULT Initialize();
	f32_t Compute_Timer();

private:
	LARGE_INTEGER		m_FrameTime = {};
	LARGE_INTEGER		m_FixTime = {};
	LARGE_INTEGER		m_LastTime = {};
	LARGE_INTEGER		m_CpuTick = {};
	f32_t				m_fTimeDelta = {};

public:
	static shared_ptr<CTimer> Create();
};

NS_END