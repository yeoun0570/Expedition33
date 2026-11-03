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

	HRESULT Add_Prototype(uint32_t iPrototypeLevelIndex, const wstring_t& strPrototypeTag, shared_ptr<class CPrototype> pPrototype);
	shared_ptr<CPrototype> Clone_Prototype(uint32_t iPrototypeLevelIndex, const wstring_t& strPrototypeTag, void* pArg);
	void	Clear(uint32_t iClearLevelIndex);

private:
	shared_ptr<class CPrototype> Find_Prototype(uint32_t iPrototypeLevelIndex, const wstring_t& strPrototypeTag);

private:
	uint32_t			m_iNumLevels = {};

	shared_ptr<map<const wstring_t, shared_ptr<class CPrototype>>[]>		m_pPrototypes;
	typedef map<const wstring_t, shared_ptr<class CPrototype>>				PROTOTYPES;

public:
	static unique_ptr<CPrototype_Manager> Create(uint32_t iNumLevels);
};

NS_END