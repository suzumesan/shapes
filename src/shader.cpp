#include "shader.h"

#include <cmath>

namespace
{
	class FormatFactory
	{
		struct Type
		{
			DXGI_FORMAT formats[3];
			UINT formatSize;
		};

		Type m_types[4];

	public:
		FormatFactory()
		{
			m_types[0].formats[0] = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
			m_types[0].formats[1] = DXGI_FORMAT::DXGI_FORMAT_R32_SINT;
			m_types[0].formats[2] = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;

			m_types[1].formats[0] = DXGI_FORMAT::DXGI_FORMAT_R32G32_UINT;
			m_types[1].formats[1] = DXGI_FORMAT::DXGI_FORMAT_R32G32_SINT;
			m_types[1].formats[2] = DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;

			m_types[2].formats[0] = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_UINT;
			m_types[2].formats[1] = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_SINT;
			m_types[2].formats[2] = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;

			m_types[3].formats[0] = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_UINT;
			m_types[3].formats[1] = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_SINT;
			m_types[3].formats[2] = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		DXGI_FORMAT getFormatForDesc(const D3D11_SIGNATURE_PARAMETER_DESC& desc) const
		{
			BYTE msb = static_cast<BYTE>(log2(desc.Mask));
			return m_types[msb].formats[desc.ComponentType - 1];
		}

		UINT getSizeForDesc(const D3D11_SIGNATURE_PARAMETER_DESC& desc) const
		{
			BYTE msb = static_cast<BYTE>(log2(desc.Mask));
			return msb + 1;
		}
	};

	static constexpr bool DEBUG_COMPILE = false;
	static constexpr bool DEBUG_WARNING_AS_ERROR = false;
	static constexpr size_t INPUT_ELEMENT_DESC_MAX_SIZE = 8;
	static const FormatFactory s_formatFactory;

	static const char* VS_ENTRY_POINT = "VSMain";
	static const char* VS_TARGET = "vs_5_0";
	static const char* PS_ENTRY_POINT = "PSMain";
	static const char* PS_TARGET = "ps_5_0";
	static const wchar_t* FILE_EXTENSION = L".hlsl";

	ID3DBlob* compileShader(
		const std::wstring& path,
		const char* entryPoint,
		const char* target)
	{
		ID3DBlob* code = nullptr;
		ID3DBlob* errorMsgs = nullptr;
		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
		if (DEBUG_COMPILE)
			flags |= D3DCOMPILE_DEBUG;
		if (DEBUG_WARNING_AS_ERROR)
			flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
		HRESULT hr = D3DCompileFromFile(
			path.c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPoint,
			target,
			flags,
			0,
			&code,
			&errorMsgs
		);

		if (errorMsgs != nullptr)
		{
			OutputDebugStringA((char*)errorMsgs->GetBufferPointer());
			errorMsgs->Release();
		}

		return code;
	}
}


ID3D11VertexShader* Shader::compileVS(const std::wstring& path, ID3DBlob** outVSBuffer /* = nullptr */)
{
	ID3DBlob* blob = compileShader(path + FILE_EXTENSION, 
		VS_ENTRY_POINT,
		VS_TARGET);
	ID3D11VertexShader* output = nullptr;
	auto hr = device()->CreateVertexShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&output);

	if (outVSBuffer)
		*outVSBuffer = blob;
	else
		blob->Release();

	return output;
}

ID3D11PixelShader* Shader::compilePS(const std::wstring& path, ID3DBlob** outPSBuffer /* = nullptr */)
{
	ID3DBlob* blob = compileShader(path + FILE_EXTENSION,
		PS_ENTRY_POINT, 
		PS_TARGET);
	ID3D11PixelShader* ps = nullptr;
	auto hr = device()->CreatePixelShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&ps);

	if (outPSBuffer)
		*outPSBuffer = blob;
	else
		blob->Release();
	return ps;
}

ID3D11ShaderReflection* Shader::createReflection(ID3DBlob* shaderBuffer)
{
	ID3D11ShaderReflection* reflector = nullptr;
	D3DReflect(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(),
		IID_ID3D11ShaderReflection, (void**)&reflector);
	return reflector;
}

ID3D11InputLayout* Shader::createInputLayout(ID3D11ShaderReflection* reflector, ID3DBlob* vsBuffer)
{
	D3D11_SHADER_DESC shaderDesc;
	reflector->GetDesc(&shaderDesc);

	const UINT elementCount = shaderDesc.InputParameters;
	D3D11_INPUT_ELEMENT_DESC elementDescArr[INPUT_ELEMENT_DESC_MAX_SIZE];
	UINT alignedTotalOffset = 0;

	for (UINT paramIdx = 0; paramIdx < elementCount; ++paramIdx)
	{
		D3D11_SIGNATURE_PARAMETER_DESC signature;
		reflector->GetInputParameterDesc(paramIdx, &signature);

		D3D11_INPUT_ELEMENT_DESC& elementDesc = elementDescArr[paramIdx];
		elementDesc.SemanticName = signature.SemanticName;
		elementDesc.SemanticIndex = signature.SemanticIndex;
		elementDesc.Format = s_formatFactory.getFormatForDesc(signature);
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = alignedTotalOffset;
		elementDesc.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		alignedTotalOffset += s_formatFactory.getSizeForDesc(signature);
	}

	ID3D11InputLayout* inputLayout = nullptr;
	device()->CreateInputLayout(
		elementDescArr,
		elementCount,
		vsBuffer->GetBufferPointer(),
		vsBuffer->GetBufferSize(),
		&inputLayout
	);

	 return inputLayout;
}

ID3D11Buffer* Shader::createConstantBuffer(UINT bufferSize)
{
	D3D11_BUFFER_DESC constBufferDesc;
	constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBufferDesc.ByteWidth = bufferSize;
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDesc.MiscFlags = 0;
	constBufferDesc.StructureByteStride = 0;

	ID3D11Buffer* constantBuffer;
	device()->CreateBuffer(
		&constBufferDesc,
		nullptr,
		&constantBuffer);
	return constantBuffer;
}

Shader::Shader(RenderDevice_SPtr& rd) :
	m_rd(rd)
{
}

Shader::~Shader()
{
	m_rd = nullptr;
}