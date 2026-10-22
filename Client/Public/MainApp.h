#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"

NS_BEGIN(Client)

class CMainApp final 
{
private:
	CMainApp();
public:
	~CMainApp() = default;

public:
	HRESULT Initialize();
	void Update(f32_t fTimeDelta);
	HRESULT Render();

public:
	static unique_ptr<CMainApp> Create();
};

NS_END