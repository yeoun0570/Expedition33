#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "Loader.h"

NS_BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext);

public:
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT		Initialize(LEVEL eNextLevelID);
	virtual void		Update(f32_t) override;
	virtual HRESULT		Render() override;

private:
	LEVEL					m_eNextLevelID = { LEVEL::END };
	shared_ptr<CLoader>		m_pLoader = { nullptr };

public:
	static shared_ptr<CLevel_Loading> Create(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext, LEVEL eNextLevelID);
};

NS_END