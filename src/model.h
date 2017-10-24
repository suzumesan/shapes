#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <string>

class Model
{
	ID3D11Device* m_device;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	UINT m_indexCount;
	UINT m_stride;

public:
	Model(ID3D11Device* device);
	~Model();

	bool load(const std::string& path);

	ID3D11Buffer* vb() const { return m_vertexBuffer; }
	ID3D11Buffer* ib() const { return m_indexBuffer; }
	UINT indexCount() const { return m_indexCount; }
	UINT stride() const { return m_stride; }
};