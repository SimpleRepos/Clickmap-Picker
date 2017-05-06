#include "ns_Utility.h"
#include <fstream>
#include <cstdarg>

Utility::OnScopeExit::OnScopeExit(std::function<void()> func) {
  reset(func);
}

Utility::OnScopeExit::~OnScopeExit() {
  func();
}

void Utility::OnScopeExit::reset(std::function<void()> func) {
  this->func = func;
}

void Utility::OnScopeExit::release() {
  reset([]() {});
}

std::vector<char> Utility::readFile(const std::string& filename) {
  std::ifstream file(filename, std::ifstream::binary);
  assert(file);

  file.seekg(0, std::ifstream::end);
  size_t len = (size_t)file.tellg();
  file.seekg(0, std::ifstream::beg);

  std::vector<char> data(len);
  file.read(data.data(), data.size());

  return data;
}

std::string Utility::stringf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  std::vector<char> buffer(_vscprintf(format, args) + 1);
  _vsnprintf_s(buffer.data(), buffer.size(), buffer.size(), format, args);
  va_end(args);

  return buffer.data();
}

std::wstring Utility::stringf(const wchar_t* format, ...) {
  va_list args;
  va_start(args, format);
  std::vector<wchar_t> buffer(_vscwprintf(format, args) + 1);
  _vsnwprintf_s(buffer.data(), buffer.size(), buffer.size(), format, args);
  va_end(args);

  return buffer.data();
}
