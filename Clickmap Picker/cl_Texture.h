#pragma once
#include <atlbase.h>
#include <d3d11.h>
#include <string>
#include <DirectXMath.h>
#include <tuple>
#include "st_ColorF.h"
#include <vector>

///<summary>Class for encapsulating a texture buffer and SRV - generate with Graphics::createTexture()</summary>
class Texture {
public:
  ///<summary>Assign the texture to a shader register</summary>
  void set(UINT slot);

  ///<summary>Dimensions of the texture in texels</summary>
  const DirectX::XMUINT2 dims;

private:
  friend class GfxFactory;
  Texture(ID3D11Device* device, ID3D11DeviceContext* context, std::pair<DirectX::XMUINT2, ID3D11Texture2D*> data, D3D11_FILTER filter);

  std::vector<ColorF::uCol32> rawTexels;
  CComPtr<ID3D11DeviceContext> context;
  CComPtr<ID3D11Texture2D> d3dtex;
  CComPtr<ID3D11ShaderResourceView> srv;
  CComPtr<ID3D11SamplerState> sampler;

};
