#pragma once

#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Client)

class CLevel_Logo final : public CLevel
{
private:
	CLevel_Logo(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext);

public:
	virtual ~CLevel_Logo() = default;

public:
	virtual HRESULT		Initialize() override;
	virtual void		Update(f32_t) override;
	virtual HRESULT		Render() override;

public:
	static shared_ptr<CLevel_Logo> Create(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext);
};

NS_END