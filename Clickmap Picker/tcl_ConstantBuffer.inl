#include "cl_Graphics.h"
#include "ns_Utility.h"

template<class T>
void ConstantBuffer<T>::update() {
  D3D11_MAPPED_SUBRESOURCE res;
  context->Map(cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
  Utility::OnScopeExit temp([this]() { context->Unmap(cBuffer, 0); });
  memcpy(res.pData, &object, sizeof(object));
}

template<class T>
void ConstantBuffer<T>::set(UINT slotNum) {
  context->VSSetConstantBuffers(slotNum, 1, &cBuffer.p);
  context->HSSetConstantBuffers(slotNum, 1, &cBuffer.p);
  context->DSSetConstantBuffers(slotNum, 1, &cBuffer.p);
  context->GSSetConstantBuffers(slotNum, 1, &cBuffer.p);
  context->PSSetConstantBuffers(slotNum, 1, &cBuffer.p);
}

template<class T>
ConstantBuffer<T>::ConstantBuffer(ID3D11Device* device, ID3D11DeviceContext* context) : context(context) {
  D3D11_BUFFER_DESC cbDesc = { sizeof(object), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };

  HR(device->CreateBuffer(&cbDesc, nullptr, &cBuffer));
}
