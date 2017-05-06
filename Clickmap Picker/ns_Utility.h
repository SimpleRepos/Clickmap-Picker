#pragma once
#include <functional>
#include <vector>
#include <cassert>

#define HR(a) { HRESULT hr = a; assert(SUCCEEDED(hr)); }

namespace Utility {
  class OnScopeExit {
  public:
    OnScopeExit(std::function<void()> func);
    ~OnScopeExit();

    void reset(std::function<void()> func);
    void release();

  private:
    std::function<void()> func;

  };

  std::vector<char> readFile(const std::string& filename);


  std::string stringf(const char* format, ...);
  std::wstring stringf(const wchar_t* format, ...);
}

