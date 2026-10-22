#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class CFbxLoader final
{
private:
	CFbxLoader();

public:
	~CFbxLoader();

public:
	HRESULT Initialize();

private:
	FbxManager*		m_pManager	= nullptr;
	FbxScene*		m_pScene	= nullptr;

public:
	static shared_ptr<CFbxLoader> Create();
};

NS_END