#pragma once
#include <atlbase.h>
#include <d3d11.h>

///<summary>Encapsulation for a constant buffer</summary>
template<class T>
class ConstantBuffer {
public:
  ///<summary>Live object - make changes to this and call ConstantBuffer::update() to commit them</summary>
  T object;

  ///<summary>Copy the current state of the object to the GPU</summary>
  void update();

  ///<summary>Assign this buffer to a GPU register</summary>
  ///<param name="slotNum">Index of register to assign to</param>
  void set(UINT slotNum);

private:
  friend class GfxFactory;
  ConstantBuffer(ID3D11Device* device, ID3D11DeviceContext* context);

  CComPtr<ID3D11DeviceContext> context;
  CComPtr<ID3D11Buffer> cBuffer;

};


#include "tcl_ConstantBuffer.inl"


