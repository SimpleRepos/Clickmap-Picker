#include "cl_VertexShader.h"
#include "ns_Utility.h"
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <unordered_map>
#include <array>

#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "dxguid.lib")

namespace {
  D3D11_INPUT_ELEMENT_DESC convertSigToElement(D3D11_SIGNATURE_PARAMETER_DESC sigDesc) {
    D3D11_INPUT_ELEMENT_DESC elmDesc = { 0 };
    elmDesc.SemanticName = sigDesc.SemanticName;
    elmDesc.SemanticIndex = sigDesc.SemanticIndex;
    elmDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

    assert(sigDesc.Mask != 0);

    size_t ct = 0;
    for(int i = 1; i < 4; i++) {
      if(sigDesc.Mask & (1 << i)) { ct++; }
    }

    const std::unordered_map<D3D_REGISTER_COMPONENT_TYPE, std::array<DXGI_FORMAT, 4>> FORMATS = {
      { D3D_REGISTER_COMPONENT_UINT32,  std::array<DXGI_FORMAT, 4>{ DXGI_FORMAT_R32_UINT,  DXGI_FORMAT_R32G32_UINT,  DXGI_FORMAT_R32G32B32_UINT,  DXGI_FORMAT_R32G32B32A32_UINT  } },
      { D3D_REGISTER_COMPONENT_SINT32,  std::array<DXGI_FORMAT, 4>{ DXGI_FORMAT_R32_SINT,  DXGI_FORMAT_R32G32_SINT,  DXGI_FORMAT_R32G32B32_SINT,  DXGI_FORMAT_R32G32B32A32_SINT  } },
      { D3D_REGISTER_COMPONENT_FLOAT32, std::array<DXGI_FORMAT, 4>{ DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT } }
    };

    elmDesc.Format = FORMATS.at(sigDesc.ComponentType)[ct];

    return elmDesc;
  }
}

VertexShader::VertexShader(const std::string& filename, D3D11_PRIMITIVE_TOPOLOGY topology, ID3D11Device* device, ID3D11DeviceContext* context) :
  context(context),
  topology(topology)
{
  auto data = Utility::readFile(filename);
  HR(device->CreateVertexShader(data.data(), data.size(), nullptr, &vShader));

  auto desc = reflectInputLayout(data);

  if(desc.size() > 0) {
    HR(device->CreateInputLayout(desc.data(), desc.size(), data.data(), data.size(), &inputLayout));
  }
}

void VertexShader::set() {
  context->VSSetShader(vShader, 0, 0);
  context->IASetInputLayout(inputLayout.p);
  context->IASetPrimitiveTopology(topology);
}

namespace {

}

std::vector<D3D11_INPUT_ELEMENT_DESC> VertexShader::reflectInputLayout(const std::vector<char>& data) {
  std::vector<D3D11_SIGNATURE_PARAMETER_DESC> sigDesc;

  {
    //CComPtr<ID3D11ShaderReflection> reflection; //~~! this causes the stack top be laid out wrong in release build
    ID3D11ShaderReflection* reflection;
    HR(D3DReflect(data.data(), data.size(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&reflection)));
    //Utility::OnScopeExit ose([reflection]() { reflection->Release(); }); //~~! so does this

    D3D11_SHADER_DESC shaderDesc;
    HR(reflection->GetDesc(&shaderDesc));

    for(size_t i = 0; i < shaderDesc.InputParameters; i++) {
      D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
      reflection->GetInputParameterDesc(i, &paramDesc);
      sigDesc.push_back(paramDesc);
    }

    //reflection->Release(); //~~! okay, something is up here...
  }

  std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc;
  for(const auto& sig : sigDesc) {
    layoutDesc.push_back(convertSigToElement(sig));
  }

  return layoutDesc;
}

