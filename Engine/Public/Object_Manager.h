#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class CObject_Manager final
{
private:
	CObject_Manager();

public:
	~CObject_Manager() = default;

public:
	HRESULT Initialize(uint32_t iNumLevels);
	HRESULT Add_GameObject(uint32_t iPrototypeLevelIndex, const wstring_t& strPrototypeTag, uint32_t iLayerLevelIndex, const wstring_t& strLayerTag, void* pArg);

private:
	shared_ptr<class CLayer> Find_Layer(uint32_t iLayerLevelIndex, const wstring_t& strLayerTag);

public:
	static unique_ptr<CObject_Manager> Create(uint32_t iNumLevels);
};

NS_END