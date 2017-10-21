#include "shader.h"

#include <d3d11.h>
#include <d3dcompiler.h>
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
	static constexpr size_t INPUT_ELEMENT_DESC_SIZE = 8;
	static const FormatFactory s_formatFactory;

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
			nullptr,
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


void Shader::compileVS(const std::wstring& path)
{
	static const char entryPoint[] = "main";
	static const char target[] = "vs_5_0";
	ID3DBlob* blob = compileShader(path + L".vs", entryPoint, target);
	auto hr = m_device->CreateVertexShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&m_vs);

	ID3D11ShaderReflection* reflector = nullptr;
	hr = D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(),
		IID_ID3D11ShaderReflection, (void**)&reflector);

	D3D11_SHADER_DESC shaderDesc;
	reflector->GetDesc(&shaderDesc);

	const UINT elementCount = shaderDesc.InputParameters;
	D3D11_INPUT_ELEMENT_DESC elementDescArr[INPUT_ELEMENT_DESC_SIZE];
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

	hr = m_device->CreateInputLayout(
		elementDescArr,
		elementCount,
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		&m_il
	);

	reflector->Release();
	blob->Release();
}

void Shader::compilePS(const std::wstring& path)
{
	static const char entryPoint[] = "main";
	static const char target[] = "ps_5_0";
	ID3DBlob* blob = compileShader(path + L".ps", entryPoint, target);
	auto hr = m_device->CreatePixelShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&m_ps);

	blob->Release();
}

Shader::Shader(ID3D11Device* device) :
	m_device(device),
	m_vs(nullptr),
	m_ps(nullptr),
	m_il(nullptr)
{
}

Shader::~Shader()
{
	m_device = nullptr;

	if (m_vs)
	{
		m_vs->Release();
		m_vs = nullptr;
	}

	if (m_ps)
	{
		m_ps->Release();
		m_ps = nullptr;
	}

	if (m_il)
	{
		m_il->Release();
		m_il = nullptr;
	}
}

void Shader::compile(const std::wstring& path)
{
	compileVS(path);
	compilePS(path);
}