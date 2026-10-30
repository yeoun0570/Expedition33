#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class CPrototype_Manager final
{
private:
	CPrototype_Manager();
public:
	~CPrototype_Manager() = default;

public:
	HRESULT Initialize(uint32_t iNumLevels);

private:
	uint32_t			m_iNumLevels = {};

	shared_ptr<map<const wstring_t, shared_ptr<class CPrototype>>[]>		m_Prototypes;
	typedef map<const wstring_t, shared_ptr<class CPrototype>>				PROTOTYPES;

public:
	static unique_ptr<CPrototype_Manager> Create(uint32_t iNumLevels);
};

NS_END