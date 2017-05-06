#pragma once
#include <string>
#include <atlbase.h>
#include <d3d11.h>

///<summary>Class for encapsulating a D3D pixel shader - generate via Graphics::createPShader()</summary>
class PixelShader {
public:
  ///<summary>Set this shader to be the active pixel shader</summary>
  void set();

private:
  friend class GfxFactory;
  PixelShader(const std::string& filename, ID3D11Device* device, ID3D11DeviceContext* context);

  CComPtr<ID3D11DeviceContext> context;
  CComPtr<ID3D11PixelShader> pShader;
  CComPtr<ID3D11SamplerState> sampler;

};
