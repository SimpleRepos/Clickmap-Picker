#pragma once
#include <atlbase.h>
#include <string>
#include <d3d11.h>
#include <wincodec.h>
#include <tuple>
#include <DirectXMath.h>
#include "st_ColorF.h"
#include <vector>

///<summary>Tool for loading image files into D3D textures</summary>
class ImageLoader {
public:
  ImageLoader();
  ~ImageLoader();

  ///<summary>Load an image from disk into a std::vector of texels</summary>
  ///<returns>A std::pair containing the texel dimensions and the vector of data</returns>
  ///<param name="filename">The path of the file to read from</param>
  std::pair<DirectX::XMUINT2, std::vector<ColorF::uCol32>> loadRaw(const std::wstring& filename);

  ///<summary>Load an image from disk into a texture</summary>
  ///<returns>A std::pair containing the texel dimensions and the D3D texture interface</returns>
  ///<param name="filename">The path of the file to read from</param>
  ///<param name="device">The device interface to create the texture through</param>
  std::pair<DirectX::XMUINT2, ID3D11Texture2D*> load(const std::wstring& filename, ID3D11Device* device);

  ///<summary>Generate a texture from raw texel data</summary>
  ///<returns>A std::pair containing the texel dimensions and the D3D texture interface</returns>
  ///<param name="dimensions">The X and Y size of the texture to create</param>
  ///<param name="data">The raw texels</param>
  ///<param name="device">The device interface to create the texture through</param>
  std::pair<DirectX::XMUINT2, ID3D11Texture2D*> create(const DirectX::XMUINT2 dimensions, const std::vector<ColorF::uCol32>& data, ID3D11Device* device);

  ///<summary>Generate a texture from raw texel data</summary>
  ///<returns>A std::pair containing the texel dimensions and the D3D texture interface</returns>
  ///<param name="dimensions">The X and Y size of the texture to create</param>
  ///<param name="data">The raw texels</param>
  ///<param name="device">The device interface to create the texture through</param>
  std::pair<DirectX::XMUINT2, ID3D11Texture2D*> create(const DirectX::XMUINT2 dimensions, const std::vector<uint8_t>& data, ID3D11Device* device);

private:
  CComPtr<IWICImagingFactory> factory;

};
