#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"

NS_BEGIN(Client)

class CLoader final
{
private:
	CLoader(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext);

public:
	~CLoader() = default;

public:
	HRESULT		Initialize(LEVEL eNextLevelID);

	bool_t		IsFinished() const { return m_isFinished; }

private:
	ComPtr<ID3D11Device>				m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>			m_pContext = { nullptr };

	LEVEL								m_eNextLevelID = { LEVEL::END };
	bool_t								m_isFinished = { false };
	wchar_t								m_szLoadingText[MAX_PATH] = {};

public:
	static shared_ptr<CLoader> Create(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext, LEVEL eNextLevelID);
};

NS_END