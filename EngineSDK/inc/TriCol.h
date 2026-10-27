#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTriCol : public CVIBuffer
{
protected:
	explicit CTriCol(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	
public:
	virtual ~CTriCol() = default;

public:
	HRESULT Initialize_Buffer() override;
	void	Render_Buffer() override;

public:
	static unique_ptr<CTriCol> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

};

NS_END