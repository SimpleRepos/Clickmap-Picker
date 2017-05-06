#include "cl_GfxFactory.h"
#include "cl_ImageLoader.h"
#include "cl_VertexShader.h"
#include "cl_PixelShader.h"
#include "cl_Texture.h"

GfxFactory::GfxFactory(ID3D11Device* device, ID3D11DeviceContext* context) :
  imageLoader(std::make_unique<ImageLoader>()),
  device(device),
  context(context)
{
  //nop
}

GfxFactory::~GfxFactory() {}

VertexShader GfxFactory::createVShader(const std::string& filename, D3D11_PRIMITIVE_TOPOLOGY topology) {
  return VertexShader(filename, topology, device.p, context.p);
}

PixelShader GfxFactory::createPShader(const std::string& filename) {
  return PixelShader(filename, device.p, context.p);
}

Texture GfxFactory::createTexture(const std::wstring& filename, D3D11_FILTER filter) {
  return Texture(device.p, context.p, imageLoader->load(filename, device.p), filter);
}

Texture GfxFactory::createTexture(const DirectX::XMUINT2 dimensions, const std::vector<ColorF::uCol32>& data, D3D11_FILTER filter) {
  return Texture(device.p, context.p, imageLoader->create(dimensions, data, device.p), filter);
}

Texture GfxFactory::createTexture(const DirectX::XMUINT2 dimensions, const std::vector<uint8_t>& data, D3D11_FILTER filter) {
  return Texture(device.p, context.p, imageLoader->create(dimensions, data, device.p), filter);
}

std::pair<DirectX::XMUINT2, std::vector<ColorF::uCol32>> GfxFactory::loadRawTextureData(const std::wstring& filename) {
  return imageLoader->loadRaw(filename);
}
