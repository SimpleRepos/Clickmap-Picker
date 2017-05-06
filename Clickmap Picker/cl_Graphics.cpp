#include "cl_Graphics.h"
#include <cassert>
#include "ns_Utility.h"
#include "cl_GfxFactory.h"

#pragma comment(lib, "d3d11.lib")

Graphics::Graphics(Window& win) :
  VIEWPORT_DIMS(win.USER_AREA_DIMS)
{
  generateDeviceContextAndSwapChain(win.getHandle());
  generateBackBufferView();
  generateDepthStencil();
  setTargetsAndViewport();
}

Graphics::~Graphics() {
  #if defined(DEBUG) || defined(_DEBUG)
  //debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
  #endif
}

void Graphics::clear(const ColorF& color) {
  clearEx(true, true, true, color, 1.0f, 0);
}

void Graphics::clearEx(bool clearTarget, bool clearDepth, bool clearStencil, const ColorF& color, float depth, UINT8 stencil) {
  if(clearTarget) { context->ClearRenderTargetView(backBuffer, color); }

  UINT flags = 0;
  if(clearDepth)   { flags |= D3D11_CLEAR_DEPTH; }
  if(clearStencil) { flags |= D3D11_CLEAR_STENCIL; }
  if(flags) { context->ClearDepthStencilView(depthStencilView, flags, depth, stencil); }
}

void Graphics::draw(UINT vertexCount) {
  context->Draw(vertexCount, 0);
}

void Graphics::drawIndexed(UINT indexCount, UINT startOffset) {
  context->DrawIndexed(indexCount, 0, startOffset);
}

void Graphics::present() {
  swapChain->Present(0, 0);
}

GfxFactory Graphics::createFactory() {
  return GfxFactory(device.p, context.p);
}

void Graphics::setStencilReferenceValue(uint8_t val) {
  context->OMSetDepthStencilState(depthStencilState, val);
}

uint8_t Graphics::fetchStencilValue(int x, int y) {
  CComPtr<ID3D11Texture2D> tex;
  depthStencilSRV->GetResource(reinterpret_cast<ID3D11Resource**>(&tex));

  D3D11_TEXTURE2D_DESC desc;
  tex->GetDesc(&desc);
  desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
  desc.Usage = D3D11_USAGE_STAGING;
  desc.BindFlags = 0;

  CComPtr<ID3D11Texture2D> copyBuffer;
  HR(device->CreateTexture2D(&desc, 0, &copyBuffer));
  context->CopyResource(copyBuffer, tex);

  D3D11_MAPPED_SUBRESOURCE resource;
  HR(context->Map(copyBuffer, 0, D3D11_MAP_READ, 0, &resource));
  Utility::OnScopeExit([this, copyBuffer]() { context->Unmap(copyBuffer, 0); });

  uint32_t* pixels = reinterpret_cast<uint32_t*>(resource.pData);
  int index = x + (y * (resource.RowPitch / 4));
  uint32_t pix = pixels[index];

  return pix >> 24;
}

void Graphics::generateDeviceContextAndSwapChain(HWND winHandle) {
  DXGI_SWAP_CHAIN_DESC sd = {
    DXGI_MODE_DESC{
      (UINT)VIEWPORT_DIMS.width, (UINT)VIEWPORT_DIMS.height,
      DXGI_RATIONAL{ 60, 1 },
      DXGI_FORMAT_R8G8B8A8_UNORM,
      DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
      DXGI_MODE_SCALING_UNSPECIFIED
    },
    DXGI_SAMPLE_DESC{ 1, 0 },
    DXGI_USAGE_RENDER_TARGET_OUTPUT,
    1,
    winHandle,
    true,
    DXGI_SWAP_EFFECT_DISCARD,
    0
  };

  UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
  #if defined(DEBUG) || defined(_DEBUG)
  flags |= D3D11_CREATE_DEVICE_DEBUG;
  #endif

  HR(D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, flags, 0, 0, D3D11_SDK_VERSION, &sd, &swapChain, &device, 0, &context));

  #if defined(DEBUG) || defined(_DEBUG)
  device->QueryInterface(IID_PPV_ARGS(&debug));
  #endif
}

void Graphics::generateBackBufferView() {
  CComPtr<ID3D11Texture2D> bbTex;
  HR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&bbTex.p));
  HR(device->CreateRenderTargetView(bbTex, 0, &backBuffer));
}

void Graphics::generateDepthStencil() {
  D3D11_TEXTURE2D_DESC dbDesc = {
    (UINT)VIEWPORT_DIMS.width, (UINT)VIEWPORT_DIMS.height,
    1, 1,
    DXGI_FORMAT_R24G8_TYPELESS,
    DXGI_SAMPLE_DESC{ 1, 0 },
    D3D11_USAGE_DEFAULT,
    D3D11_BIND_DEPTH_STENCIL|D3D11_BIND_SHADER_RESOURCE,
    0, 0
  };

  CComPtr<ID3D11Texture2D> depthStencilBuffer;
  HR(device->CreateTexture2D(&dbDesc, 0, &depthStencilBuffer));

  D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
  ZeroMemory(&dsvDesc, sizeof(dsvDesc));
  dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  dsvDesc.Texture2D.MipSlice = 0;

  HR(device->CreateDepthStencilView(depthStencilBuffer, &dsvDesc, &depthStencilView));

  D3D11_DEPTH_STENCIL_DESC dsDesc = {
    TRUE, //depth enable
    D3D11_DEPTH_WRITE_MASK_ALL, //depth write mask
    D3D11_COMPARISON_LESS, //depth func (write if this evaluates to true)
    TRUE, //stencil enable
    0xFF, 0xFF, //stencil read/write masks
    { //front-facing stencil ops
      D3D11_STENCIL_OP_KEEP,  //fail op
      D3D11_STENCIL_OP_KEEP,  //depth fail op
      D3D11_STENCIL_OP_REPLACE,  //pass op
      D3D11_COMPARISON_ALWAYS //func
    },
    { //back facing stencil ops
      D3D11_STENCIL_OP_KEEP,  //fail op
      D3D11_STENCIL_OP_KEEP,  //depth fail op
      D3D11_STENCIL_OP_KEEP,  //pass op
      D3D11_COMPARISON_ALWAYS //func
    }
  };

  HR(device->CreateDepthStencilState(&dsDesc, &depthStencilState));
  setStencilReferenceValue(0);

  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
  ZeroMemory(&srvDesc, sizeof(srvDesc));
  srvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
  srvDesc.Texture2D.MipLevels = 1;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

  HR(device->CreateShaderResourceView(depthStencilBuffer, &srvDesc, &depthStencilSRV));

}

void Graphics::setTargetsAndViewport() {
  context->OMSetRenderTargets(1, &backBuffer.p, depthStencilView);
  
  D3D11_VIEWPORT vp = { 0.0f, 0.0f, (float)VIEWPORT_DIMS.width, (float)VIEWPORT_DIMS.height, 0.0f, 1.0f };
  context->RSSetViewports(1, &vp);
}
