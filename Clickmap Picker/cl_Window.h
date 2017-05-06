#pragma once
#include <Windows.h>
#include <string>
#include <unordered_map>
#include <functional>

///<summary>Class encapsualting a WinAPI window class and instance.</summary>
class Window {
public:
  ///<summary>LONG width and height used to represent window dimensions (in pixels)</summary>
  struct Dimensions {
    LONG width;
    LONG height;
  };

  ///<summary>Create a window</summary>
  ///<param name="windowTitle">Text to display in the title bar</param>
  ///<param name="userAreaDims">Size of the "user area" within the window borders.
  ///The window itself will be larger than this.
  ///This should be the same size you intend for the D3D viewport.</param>
  Window(const std::string& windowTitle, Dimensions userAreaDims);
  ~Window();

  Window(Window&& other);

  ///<summary>Add a function to execute on WM_KEYDOWN for a specific virtual key code.</summary>
  ///<remarks>Any existing function associated with the key will be overridden.</remarks>
  ///<param name="vkey">The virtual keycode to associate with a function (ex: VK_ESC).</param>
  ///<param name="func">The function to call when the key is pressed.</param>
  void addKeyFunc(WPARAM vkey, std::function<void(HWND, LPARAM)> func);

  ///<summary>Remove any function associated with a virtual key via Window::addKeyFunc().</summary>
  ///<param name="vkey">The virtual keycode to remove associations from (ex: VK_ESC).</param>
  void clearKeyFunc(WPARAM vkey);

  ///<summary>Add a function to execute when a specific window message is recieved via the WindowProc.</summary>
  ///<remarks>Any existing function associated with the message will be overridden.</remarks>
  ///<param name="message">The message to associate with a function (ex: WM_LBUTTONDOWN).</param>
  ///<param name="func">The function to call when the message is recieved.</param>
  void addProcFunc(UINT message, std::function<LRESULT(HWND, WPARAM, LPARAM)> func);

  ///<summary>Remove any function associated with a window message via Window::addProcFunc().</summary>
  ///<param name="message">The message to remove associations from (ex: WM_LBUTTONDOWN).</param>
  void clearProcFunc(UINT message);

  ///<summary>Process the OS message queue.</summary>
  /// <remarks>
  /// This function should be called often. Once per frame is usual.
  /// This function returns a boolean value indicating whether the window is still available. If the value is false then the window has closed.
  /// This can be used as the argument to the top-level control loop, possibly ANDed with other conditions.
  /// </remarks>
  ///<returns>boolean calue indicating whether the window is still available - if this vaue is false then the window has closed</returns>
  bool update();

  ///<summary>Retrieve the OS window handle.</summary>
  ///<returns>The HWND of the encapsulated window</returns>
  HWND getHandle();

  ///<summary>The dimensions - in pixels - of the user area, as provided at constuction.</summary>
  const Dimensions USER_AREA_DIMS;

  ///<summary>The dimensions - in pixels - of the window, including the borders.</summary>
  const Dimensions WINDOW_DIMS;

private:
  ///<summary>This object can not be copied. Try moving it instead.</summary>
  Window(const Window&) = delete;

  ///<summary>This object can not be copied. Try moving it instead.</summary>
  void operator=(const Window&) = delete;

  ///<summary>This object can not be move-assigned. Try move-construction instead.</summary>
  Window& operator=(Window&&) = delete;

  HWND handle;
  std::string cName;

  static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  std::unordered_map<WPARAM, std::function<void(HWND, LPARAM)>> keyFuncs;
  std::unordered_map<UINT, std::function<LRESULT(HWND, WPARAM, LPARAM)>> procFuncs;

  Dimensions calcWinDims(Dimensions userDims);

  static const long STYLE = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;

};
