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
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	void	Render_Buffer() override;

public:
	static unique_ptr<CTriCol> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;

};

NS_END