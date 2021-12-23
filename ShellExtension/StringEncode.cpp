/*
***************************************************************************************************
* 许可声明：
* 本软件（含源码、二进制）为开源软件，遵循MIT开源许可协议（若以下声明与MIT许可协议发送冲突，以本声明为准）。
* 任何个人或组织都可以不受限制的使用、修改该软件。
* 任何个人或组织都可以以源代码和二进制的形式重新分发、使用该软件。
* 任何个人或组织都可以不受限制地将该软件（或修改后的任意版本）用于商业、非商业的用途。
* 
* 免责声明：
* 作者不对使用、修改、分发（以及其他任何形式的使用）该软件（及其他修改后的任意版本）所产生的后果负有任何法律责任。
* 作者也不对该软件的安全性、稳定性做出任何保证。
* 
* 使用、修改、分发该软件时需要保留上述声明，且默认已经同意上述声明。
***************************************************************************************************
*/
#include "StringEncode.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#pragma warning(disable : 4309)

std::string UnicodeToAnsi(const std::wstring& str, unsigned int code_page /*= 0*/) {
  std::string strRes;
  int iSize = ::WideCharToMultiByte(code_page, 0, str.c_str(), -1, NULL, 0, NULL, NULL);

  if (iSize == 0)
    return strRes;

  char* szBuf = new (std::nothrow) char[iSize];

  if (!szBuf)
    return strRes;

  memset(szBuf, 0, iSize);

  ::WideCharToMultiByte(code_page, 0, str.c_str(), -1, szBuf, iSize, NULL, NULL);

  strRes = szBuf;
  delete[] szBuf;

  return strRes;
}

std::wstring AnsiToUnicode(const std::string& str, unsigned int code_page /*= 0*/) {
  std::wstring strRes;

  int iSize = ::MultiByteToWideChar(code_page, 0, str.c_str(), -1, NULL, 0);

  if (iSize == 0)
    return strRes;

  wchar_t* szBuf = new (std::nothrow) wchar_t[iSize];

  if (!szBuf)
    return strRes;

  memset(szBuf, 0, iSize * sizeof(wchar_t));

  ::MultiByteToWideChar(code_page, 0, str.c_str(), -1, szBuf, iSize);

  strRes = szBuf;
  delete[] szBuf;

  return strRes;
}

std::string UnicodeToUtf8(const std::wstring& str) {
  std::string strRes;

  int iSize = ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);

  if (iSize == 0)
    return strRes;

  char* szBuf = new (std::nothrow) char[iSize];

  if (!szBuf)
    return strRes;

  memset(szBuf, 0, iSize);

  ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, szBuf, iSize, NULL, NULL);

  strRes = szBuf;
  delete[] szBuf;

  return strRes;
}

std::string UnicodeToUtf8BOM(const std::wstring& str) {
  std::string strRes;

  int iSize = ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);

  if (iSize == 0)
    return strRes;

  char* szBuf = new (std::nothrow) char[iSize + 3];

  if (!szBuf)
    return strRes;

  memset(szBuf, 0, iSize + 3);

  ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &szBuf[3], iSize, NULL, NULL);
  szBuf[0] = 0xef;
  szBuf[1] = 0xbb;
  szBuf[2] = 0xbf;

  strRes = szBuf;
  delete[] szBuf;

  return strRes;
}

std::wstring Utf8ToUnicode(const std::string& str) {
  std::wstring strRes;
  int iSize = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

  if (iSize == 0)
    return strRes;

  wchar_t* szBuf = new (std::nothrow) wchar_t[iSize];

  if (!szBuf)
    return strRes;

  memset(szBuf, 0, iSize * sizeof(wchar_t));
  ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, szBuf, iSize);

  strRes = szBuf;
  delete[] szBuf;

  return strRes;
}

std::string AnsiToUtf8(const std::string& str, unsigned int code_page /*= 0*/) {
  return UnicodeToUtf8(AnsiToUnicode(str, code_page));
}

std::string AnsiToUtf8BOM(const std::string& str, unsigned int code_page /* = 0*/) {
  return UnicodeToUtf8BOM(AnsiToUnicode(str, code_page));
}

std::string Utf8ToAnsi(const std::string& str, unsigned int code_page /*= 0*/) {
  return UnicodeToAnsi(Utf8ToUnicode(str), code_page);
}
