#include "ns_Utility.h"

template<class T, size_t SZ>
void DynamicVertexBuffer<T, SZ>::update() {
  D3D11_MAPPED_SUBRESOURCE res;
  context->Map(vBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
  Utility::OnScopeExit temp([this]() { context->Unmap(vBuffer, 0); });
  memcpy(res.pData, data.data(), sizeof(data));
}

template<class T, size_t SZ>
void DynamicVertexBuffer<T, SZ>::set(UINT slotNum) {
  if(vBuffer == nullptr) {
    context->IASetVertexBuffers(slotNum, 0, 0, 0, 0);
    return;
  }

  constexpr UINT NUM_BUFFERS = 1;
  UINT strides[NUM_BUFFERS] = { sizeof(T) };
  UINT offsets[NUM_BUFFERS] = { 0 };
  context->IASetVertexBuffers(slotNum, NUM_BUFFERS, &vBuffer.p, strides, offsets);
}

template<class T, size_t SZ>
DynamicVertexBuffer<T, SZ>::DynamicVertexBuffer(ID3D11Device* device, ID3D11DeviceContext* context) : 
  context(context)
{
  if(SZ) {
    D3D11_BUFFER_DESC vbDesc = { SZ * sizeof(T), D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };
    HR(device->CreateBuffer(&vbDesc, nullptr, &vBuffer));
  }
}
