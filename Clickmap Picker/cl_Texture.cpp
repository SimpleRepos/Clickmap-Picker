#include "cl_Texture.h"
#include <wincodec.h>
#include <vector>
#include "ns_Utility.h"

#pragma comment(lib, "windowscodecs.lib")

Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* context, std::pair<DirectX::XMUINT2, ID3D11Texture2D*> data, D3D11_FILTER filter) : 
  dims(data.first),
  context(context)
{
  d3dtex.Attach(data.second);

  D3D11_TEXTURE2D_DESC tDesc;
  d3dtex->GetDesc(&tDesc);

  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
  srvDesc.Format = tDesc.Format;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MipLevels = 1;
  srvDesc.Texture2D.MostDetailedMip = 0;

  HR(device->CreateShaderResourceView(d3dtex, &srvDesc, &srv));

  D3D11_SAMPLER_DESC sampDesc = {};
  sampDesc.Filter = filter; 
  sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.MaxAnisotropy = 1;
  sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

  HR(device->CreateSamplerState(&sampDesc, &sampler));
}

void Texture::set(UINT slot) {
  context->DSSetShaderResources(slot, 1, &srv.p);
  context->PSSetShaderResources(slot, 1, &srv.p);
  context->PSSetSamplers(slot, 1, &sampler.p);
}
