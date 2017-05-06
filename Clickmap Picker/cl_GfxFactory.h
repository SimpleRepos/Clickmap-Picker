#pragma once
#include <string>
#include <vector>
#include <atlbase.h>
#include <d3d11.h>
#include "tcl_ConstantBuffer.h"
#include "tcl_DynamicVertexBuffer.h"
#include <memory>
#include <DirectXMath.h>

class ImageLoader;
class Texture;
class VertexShader;
class PixelShader;

class GfxFactory {
public:
  GfxFactory(ID3D11Device* device, ID3D11DeviceContext* context);
  GfxFactory(GfxFactory&&) = default;
  ~GfxFactory();

  template<class T>
  ConstantBuffer<T> createConstantBuffer() {
    return ConstantBuffer<T>(device.p, context.p);
  }

  template<class T, size_t SZ>
  DynamicVertexBuffer<T, SZ> createDynamicVertexBuffer() {
    return DynamicVertexBuffer<T, SZ>(device.p, context.p);
  }

  VertexShader createVShader(const std::string& filename, D3D11_PRIMITIVE_TOPOLOGY topology);

  PixelShader createPShader(const std::string& filename);

  Texture createTexture(const std::wstring& filename, D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR);

  Texture createTexture(const DirectX::XMUINT2 dimensions, const std::vector<ColorF::uCol32>& data, D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR);

  Texture createTexture(const DirectX::XMUINT2 dimensions, const std::vector<uint8_t>& data, D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR);

  std::pair<DirectX::XMUINT2, std::vector<ColorF::uCol32>> loadRawTextureData(const std::wstring& filename);

private:
  std::unique_ptr<ImageLoader> imageLoader;
  CComPtr<ID3D11Device> device;
  CComPtr<ID3D11DeviceContext> context;

};

