#include "cl_Window.h"
#include "cl_Graphics.h"
#include "cl_GfxFactory.h"
#include "cl_Texture.h"
#include "tcl_DynamicVertexBuffer.h"
#include "cl_VertexShader.h"
#include "cl_PixelShader.h"

constexpr int VP_WIDTH  = 800;
constexpr int VP_HEIGHT = 600;

struct Vec2i {
  int x, y;
  Vec2i operator-(const Vec2i& other) const { return Vec2i{ x - other.x, y - other.y }; }
  void operator=(LPARAM lp) { x = lp & 0xFFFF; y = lp >> 16; }
};

class DragonSprite {
public:
  DragonSprite(GfxFactory& factory, float x, float y, float z, float w, float h, uint8_t stencilVal);
  Vec2i getPos() const { return Vec2i{ (int)x, (int)y }; }
  void moveTo(const Vec2i& pos) { x = (float)pos.x; y = (float)pos.y; }
  void draw(Graphics& gfx);

private:
  struct Vertex { DirectX::XMFLOAT3 pos; DirectX::XMFLOAT2 tex; };
  DynamicVertexBuffer<DragonSprite::Vertex, 4> vBuffer;
  VertexShader vShader;
  PixelShader pShader;
  float x, y, z, w, h;
  uint8_t stencilVal;

};

int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int) {
  Window win("Clickmap Picker", { VP_WIDTH, VP_HEIGHT });
  Graphics gfx(win);
  GfxFactory factory(gfx.createFactory());
  Texture tex = factory.createTexture(L"../Clickmap Picker/widget.png");
  tex.set(0);

  win.addKeyFunc(VK_ESCAPE, [](HWND, LPARAM) { PostQuitMessage(0); });

  Vec2i mousePos, grabOffset;
  bool mouseDown{}, mouseUp{};
  int grabbedDragon = -1;
  win.addProcFunc(WM_LBUTTONDOWN, [&mouseDown](HWND, WPARAM, LPARAM coords) -> LRESULT {  mouseDown = true; return 0; });
  win.addProcFunc(WM_LBUTTONUP,     [&mouseUp](HWND, WPARAM, LPARAM coords) -> LRESULT {    mouseUp = true; return 0; });
  win.addProcFunc(WM_MOUSEMOVE,    [&mousePos](HWND, WPARAM, LPARAM coords) -> LRESULT { mousePos = coords; return 0; });

  DragonSprite dragons[] = {
    DragonSprite(factory,  10,  10, 5, 120, 120, 1),
    DragonSprite(factory, 250, 250, 4, 100, 100, 2),
    DragonSprite(factory, 500, 250, 6, 200, 200, 3)
  };

  while(win.update()) {
    //update
    if(mouseDown) {
      grabbedDragon = gfx.fetchStencilValue(mousePos.x, mousePos.y);
      grabbedDragon--;
      if(grabbedDragon >= 0) {
        grabOffset = mousePos - dragons[grabbedDragon].getPos();
      }

      mouseDown = false;
      mouseUp = false;
    }

    if(grabbedDragon >= 0) {
      dragons[grabbedDragon].moveTo(mousePos - grabOffset);
      if(mouseUp) { grabbedDragon = -1; }
    }

    //draw
    gfx.clear(ColorF::CYAN);
    for(auto& drg : dragons) { drg.draw(gfx); }
    gfx.present();
  }

}

DragonSprite::DragonSprite(GfxFactory& factory, float x, float y, float z, float w, float h, uint8_t stencilVal) :
  vBuffer(factory.createDynamicVertexBuffer<DragonSprite::Vertex, 4>()),
  vShader(factory.createVShader("../Debug/VertexShader.cso", D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP)),
  pShader(factory.createPShader("../Debug/PixelShader.cso")),
  x(x), y(y), z(z), w(w), h(h), stencilVal(stencilVal)
{
  //nop
}

void DragonSprite::draw(Graphics& gfx) {
  vShader.set();
  pShader.set();
  vBuffer.set(0);

  float l = x;
  float t = VP_HEIGHT - y;
  float r = l + w;
  float b = t - h;

  vBuffer.data[0] = Vertex{ DirectX::XMFLOAT3(l, t, z), DirectX::XMFLOAT2(0, 0) };
  vBuffer.data[1] = Vertex{ DirectX::XMFLOAT3(r, t, z), DirectX::XMFLOAT2(1, 0) };
  vBuffer.data[2] = Vertex{ DirectX::XMFLOAT3(l, b, z), DirectX::XMFLOAT2(0, 1) };
  vBuffer.data[3] = Vertex{ DirectX::XMFLOAT3(r, b, z), DirectX::XMFLOAT2(1, 1) };
  vBuffer.update();

  gfx.setStencilReferenceValue(stencilVal);
  gfx.draw(vBuffer.data.size());
}
