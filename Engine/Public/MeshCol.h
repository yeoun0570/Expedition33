#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class CMeshCol : public CVIBuffer
{
private:
	CMeshCol(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);
	
public:
	~CMeshCol() = default;

public:
	virtual HRESULT Initialize_Prototype(const MESH_DATA& MeshData);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Render_Buffer() override;


public:
	static shared_ptr<CMeshCol>	Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext, const MESH_DATA& MeshData);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END