#pragma once
#include <atlbase.h>
#include <d3d11.h>
#include <string>
#include <vector>
///<summary>Class for encapsulating a D3D vertex shader - generate via Graphics::createVShader()</summary>
class VertexShader {
public:
  ///<summary>Set this shader to be the active vertex shader</summary>
  void set();

private:
  friend class GfxFactory;
  VertexShader(const std::string& filename, D3D11_PRIMITIVE_TOPOLOGY topology, ID3D11Device* device, ID3D11DeviceContext* context);

  std::vector<D3D11_INPUT_ELEMENT_DESC> reflectInputLayout(const std::vector<char>& data);

  CComPtr<ID3D11DeviceContext> context;
  CComPtr<ID3D11VertexShader> vShader;
  CComPtr<ID3D11InputLayout> inputLayout;
  D3D11_PRIMITIVE_TOPOLOGY topology;

};

