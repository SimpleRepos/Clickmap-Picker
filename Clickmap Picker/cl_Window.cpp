#include "cl_Window.h"

Window::Window(const std::string& title, Dimensions userAreaDims) : 
  USER_AREA_DIMS(userAreaDims), 
  WINDOW_DIMS(calcWinDims(userAreaDims)),
  cName(title.c_str())
{
  WNDCLASS wc = {
    CS_HREDRAW|CS_VREDRAW, WindowProc, 0, 8, GetModuleHandle(NULL), 0,
    LoadCursor(NULL, IDC_ARROW), (HBRUSH)COLOR_WINDOW, 0, cName.c_str()
  };
  if(!RegisterClass(&wc)) { throw std::runtime_error("Failed to register window class."); }

  handle = CreateWindow(cName.c_str(), title.c_str(), STYLE, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_DIMS.width, WINDOW_DIMS.height, 0, 0, wc.hInstance, 0);

  SetLastError(0);
  SetWindowLongPtr(handle, 0, LONG_PTR(this));
  if(GetLastError()) { throw std::runtime_error("Failed to backlink Window object to HWND."); }

  addProcFunc(WM_DESTROY, [](HWND, WPARAM, LPARAM) -> LRESULT { PostQuitMessage(0); return 0; });
}

Window::~Window() {
  if(!cName.empty()) {
    UnregisterClass(cName.c_str(), GetModuleHandle(NULL));
  }
}

Window::Window(Window&& other) : 
  USER_AREA_DIMS(other.USER_AREA_DIMS), 
  WINDOW_DIMS(other.WINDOW_DIMS),
  handle(other.handle),
  cName(std::move(other.cName)),
  keyFuncs(std::move(other.keyFuncs)),
  procFuncs(std::move(other.procFuncs))
{
  other.cName.clear();
}

void Window::addKeyFunc(WPARAM message, std::function<void(HWND, LPARAM)> func) {
  keyFuncs[message] = func;
}

void Window::clearKeyFunc(WPARAM message) {
  procFuncs.erase(message);
}

void Window::addProcFunc(UINT message, std::function<LRESULT(HWND, WPARAM, LPARAM)> func) {
  procFuncs[message] = func;
}

void Window::clearProcFunc(UINT message) {
  procFuncs.erase(message);
}

bool Window::update() {
  MSG msg;
  while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    if(msg.message == WM_QUIT) { return false; }
  }

  return true;
}

HWND Window::getHandle() {
  return handle;
}

LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 	{
  Window* pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, 0));

  if(pWindow) {
    if(message == WM_KEYDOWN && pWindow->keyFuncs.count(wParam)) {
      pWindow->keyFuncs[wParam](hWnd, lParam);
      return 0;
    }

    if(pWindow->procFuncs.count(message)) {
      return pWindow->procFuncs[message](hWnd, wParam, lParam);
    }

  }

  return DefWindowProc(hWnd, message, wParam, lParam);
}

Window::Dimensions Window::calcWinDims(Dimensions userDims) {
  RECT rect{ 0, 0, userDims.width, userDims.height };
  AdjustWindowRect(&rect, STYLE, FALSE);
  return Dimensions{ rect.right - rect.left, rect.bottom - rect.top };
}

