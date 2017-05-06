#pragma once
#include <atlbase.h>
#include <d3d11.h>
#include <array>

#include "ns_Utility.h"

///<summary>Encapsulation for a dynamic (can be changed) vertex buffer</summary>
template<class T, size_t SZ>
class DynamicVertexBuffer {
public:
  ///<summary>Number of vertices in the buffer</summary>
  static constexpr size_t SIZE = SZ;

  ///<summary>Vertex data - make changes and then call DynamicVertexBuffer::update() to commit them.</summary>
  std::array<T, SZ> data;

  ///<summary>Upload the current data to the GPU</summary>
  void update();

  ///<summary>Assign this vertex buffer to a GPU register</summary>
  ///<param name="slotNum">Index of register to assign to</param>
  void set(UINT slotNum);

private:
  friend class GfxFactory;
  DynamicVertexBuffer(ID3D11Device* device, ID3D11DeviceContext* context);

  CComPtr<ID3D11DeviceContext> context;
  CComPtr<ID3D11Buffer> vBuffer;

};

#include "tcl_DynamicVertexBuffer.inl"
