#include "cl_ImageLoader.h"
#include "ns_Utility.h"
#include <vector>

#pragma comment(lib, "windowscodecs.lib")

ImageLoader::ImageLoader() {
  CoInitialize(0);
  HR(CoCreateInstance(CLSID_WICImagingFactory, 0, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<LPVOID*>(&factory.p)));
}

ImageLoader::~ImageLoader() {
  factory.Release();
  CoUninitialize();
}

std::pair<DirectX::XMUINT2, std::vector<ColorF::uCol32>> ImageLoader::loadRaw(const std::wstring& filename) {
  CComPtr<IWICBitmapDecoder> decoder;
  HR(factory->CreateDecoderFromFilename(filename.c_str(), 0, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder));

  CComPtr<IWICBitmapFrameDecode> frame;
  HR(decoder->GetFrame(0, &frame));
  DirectX::XMUINT2 dims;
  frame->GetSize(&dims.x, &dims.y);

  CComPtr<IWICFormatConverter> converter;
  HR(factory->CreateFormatConverter(&converter));
  converter->Initialize(frame, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, 0, 0, WICBitmapPaletteTypeMedianCut);

  //~~? got this from MSDN - no clue about IWIC pitch. works for 512x512, needs testing otherwise
  constexpr int bpp = 32;
  D3D11_SUBRESOURCE_DATA subData;
  subData.SysMemPitch = (dims.x * bpp + 7) / 8;
  subData.SysMemSlicePitch = subData.SysMemPitch * dims.y;

  std::vector<ColorF::uCol32> imageData(subData.SysMemSlicePitch / sizeof(ColorF::uCol32));
  subData.pSysMem = imageData.data();

  HR(converter->CopyPixels(0, subData.SysMemPitch, imageData.size() * sizeof(ColorF::uCol32), reinterpret_cast<BYTE*>(imageData.data())));

  return std::make_pair(dims, imageData);
}

std::pair<DirectX::XMUINT2, ID3D11Texture2D*> ImageLoader::load(const std::wstring& filename, ID3D11Device* device) {
  auto raw = loadRaw(filename);
  return create(raw.first, raw.second, device);
}

std::pair<DirectX::XMUINT2, ID3D11Texture2D*> ImageLoader::create(const DirectX::XMUINT2 dimensions, const std::vector<ColorF::uCol32>& data, ID3D11Device* device) {
  constexpr int bpp = 32;
  D3D11_SUBRESOURCE_DATA subData;
  subData.SysMemPitch = (dimensions.x * bpp + 7) / 8;
  subData.SysMemSlicePitch = subData.SysMemPitch * dimensions.y;
  subData.pSysMem = data.data();


  D3D11_TEXTURE2D_DESC tDesc;
  tDesc.Width  = dimensions.x;
  tDesc.Height = dimensions.y;
  tDesc.MipLevels = 1;
  tDesc.ArraySize = 1;
  tDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  tDesc.SampleDesc = { 1, 0 };
  tDesc.Usage = D3D11_USAGE_DEFAULT;
  tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  tDesc.CPUAccessFlags = 0;
  tDesc.MiscFlags = 0;

  ID3D11Texture2D* tex;
  HR(device->CreateTexture2D(&tDesc, &subData, &tex));

  return std::make_pair(dimensions, tex);
}

std::pair<DirectX::XMUINT2, ID3D11Texture2D*> ImageLoader::create(const DirectX::XMUINT2 dimensions, const std::vector<uint8_t>& data, ID3D11Device* device) {
  constexpr int bpp = 8;
  D3D11_SUBRESOURCE_DATA subData;
  subData.SysMemPitch = (dimensions.x * bpp + 7) / 8;
  subData.SysMemSlicePitch = subData.SysMemPitch * dimensions.y;
  subData.pSysMem = data.data();

  D3D11_TEXTURE2D_DESC tDesc;
  tDesc.Width  = dimensions.x;
  tDesc.Height = dimensions.y;
  tDesc.MipLevels = 1;
  tDesc.ArraySize = 1;
  tDesc.Format = DXGI_FORMAT_R8_UNORM;
  tDesc.SampleDesc = { 1, 0 };
  tDesc.Usage = D3D11_USAGE_DEFAULT;
  tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  tDesc.CPUAccessFlags = 0;
  tDesc.MiscFlags = 0;

  ID3D11Texture2D* tex;
  HR(device->CreateTexture2D(&tDesc, &subData, &tex));

  return std::make_pair(dimensions, tex);
}

