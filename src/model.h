#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <string>

class Model
{
	ID3D11Device* m_device;
	ID3D11Buffer* m_vertexBuffer;

public:
	Model(ID3D11Device* device);
	~Model();

	bool load(const std::string& path);

	ID3D11Buffer* vb() const { return m_vertexBuffer; }

};