#include "model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>


Model::Model(ID3D11Device* device) :
	m_device(device),
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_indexCount((UINT)-1),
	m_stride((UINT)-1)
{
}

Model::~Model()
{
	m_device = nullptr;
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}
}

bool Model::load(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (!scene)
	{
		return false;
	}

	aiMesh* mesh = scene->mMeshes[0];
	UINT stride = m_stride = sizeof(aiVector3D);
	unsigned int indexCount = m_indexCount = mesh->mNumFaces * 3;

	D3D11_BUFFER_DESC vbDesc;
	vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vbDesc.ByteWidth = mesh->mNumVertices * stride;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	vbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = (void*)mesh->mVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	auto hr = m_device->CreateBuffer(
		&vbDesc,
		&vertexData,
		&m_vertexBuffer);

	D3D11_BUFFER_DESC ibDesc;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = indexCount * sizeof(unsigned int);
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.StructureByteStride = 0;

	std::vector<unsigned int> indices;
	indices.reserve(indexCount);
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		indices.push_back(mesh->mFaces[i].mIndices[0]);
		indices.push_back(mesh->mFaces[i].mIndices[1]);
		indices.push_back(mesh->mFaces[i].mIndices[2]);
	}

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = (void*)indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	hr = m_device->CreateBuffer(
		&ibDesc,
		&indexData,
		&m_indexBuffer
	);

	return true;
}