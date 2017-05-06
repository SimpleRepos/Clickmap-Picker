#pragma once
#include "st_ColorF.h"
#include <atlbase.h>
#include <d3d11.h>
#include "cl_Window.h"

class GfxFactory;

class Graphics {
public:
  Graphics(Window& win);
  ~Graphics();

  void clear(const ColorF& color = ColorF::BLACK);

  void clearEx(bool clearTarget, bool clearDepth, bool clearStencil, const ColorF& color = ColorF::BLACK, float depth = 1.0f, UINT8 stencil = 0);

  void draw(UINT vertexCount);

  void drawIndexed(UINT indexCount, UINT startOffset = 0);

  void present();

  const Window::Dimensions VIEWPORT_DIMS;

  GfxFactory createFactory();

  void setStencilReferenceValue(uint8_t val);

  uint8_t fetchStencilValue(int x, int y);

private:
  CComPtr<ID3D11Device> device;
  CComPtr<ID3D11DeviceContext> context;
  CComPtr<IDXGISwapChain> swapChain;
  CComPtr<ID3D11RenderTargetView> backBuffer;
  CComPtr<ID3D11DepthStencilView> depthStencilView;
  CComPtr<ID3D11DepthStencilState> depthStencilState;
  CComPtr<ID3D11ShaderResourceView> depthStencilSRV;
#if defined(DEBUG) || defined(_DEBUG)
  CComPtr<ID3D11Debug> debug;
  #endif

  void generateDeviceContextAndSwapChain(HWND winHandle);
  void generateBackBufferView();
  void generateDepthStencil();
  void setTargetsAndViewport();

};
