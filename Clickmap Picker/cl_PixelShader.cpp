#include "cl_PixelShader.h"
#include "ns_Utility.h"

PixelShader::PixelShader(const std::string& filename, ID3D11Device* device, ID3D11DeviceContext* context) :
  context(context)
{
  auto data = Utility::readFile(filename);

  HR(device->CreatePixelShader(data.data(), data.size(), nullptr, &pShader));

  D3D11_SAMPLER_DESC sDesc;
  sDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  sDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  sDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  sDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  sDesc.MipLODBias = 0;
  sDesc.MaxAnisotropy = 1;
  sDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
  ZeroMemory(sDesc.BorderColor, sizeof(sDesc.BorderColor));
  sDesc.MinLOD = 0;
  sDesc.MaxLOD = D3D11_FLOAT32_MAX;

  HR(device->CreateSamplerState(&sDesc, &sampler));
  context->PSSetSamplers(0, 1, &sampler.p);

}

void PixelShader::set() {
  context->PSSetShader(pShader, 0, 0);
}


