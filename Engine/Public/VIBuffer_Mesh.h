#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class CVIBuffer_Mesh : public CVIBuffer
{
private:
	CVIBuffer_Mesh(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext);
	
public:
	~CVIBuffer_Mesh() = default;

public:
	virtual HRESULT Initialize_Prototype(const MESH_DATA& MeshData);
	virtual HRESULT Initialize(void* pArg) override;


public:
	static shared_ptr<CVIBuffer_Mesh>	Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext, const MESH_DATA& MeshData);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END