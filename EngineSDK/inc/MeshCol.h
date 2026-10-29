#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class CMeshCol : public CVIBuffer
{
private:
	CMeshCol(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext);
	CMeshCol(const CMeshCol&) = delete;
	CMeshCol& operator=(const CMeshCol&) = delete;

public:
	~CMeshCol() = default;

public:
	HRESULT Initialize_Buffer(const MESH_DATA& MeshData);
	void	Render_Buffer() override;


public:
	static unique_ptr<CMeshCol>	Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext> pContext, const MESH_DATA& MeshData);
};

NS_END