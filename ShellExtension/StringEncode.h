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
#ifndef STRINGENCODE_H_
#define STRINGENCODE_H_
#include <assert.h>
#include <sstream>
#include <string>
#include <vector>

std::string UnicodeToAnsi(const std::wstring& str, unsigned int code_page = 0);
std::wstring AnsiToUnicode(const std::string& str, unsigned int code_page = 0);
std::string UnicodeToUtf8(const std::wstring& str);
std::wstring Utf8ToUnicode(const std::string& str);
std::string AnsiToUtf8(const std::string& str, unsigned int code_page = 0);
std::string Utf8ToAnsi(const std::string& str, unsigned int code_page = 0);

std::string UnicodeToUtf8BOM(const std::wstring& str);
std::string AnsiToUtf8BOM(const std::string& str, unsigned int code_page = 0);

#if (defined UNICODE || defined _UNICODE)
#define TCHARToAnsi(str) akali::UnicodeToAnsi((str), 0)
#define TCHARToUtf8(str) akali::UnicodeToUtf8((str))
#define AnsiToTCHAR(str) akali::AnsiToUnicode((str), 0)
#define Utf8ToTCHAR(str) akali::Utf8ToUnicode((str))
#define TCHARToUnicode(str) ((std::wstring)(str))
#define UnicodeToTCHAR(str) ((std::wstring)(str))
#else
#define TCHARToAnsi(str) ((std::string)(str))
#define TCHARToUtf8 akali::AnsiToUtf8((str), 0)
#define AnsiToTCHAR(str) ((std::string)(str))
#define Utf8ToTCHAR(str) akali::Utf8ToAnsi((str), 0)
#define TCHARToUnicode(str) akali::AnsiToUnicode((str), 0)
#define UnicodeToTCHAR(str) akali::UnicodeToAnsi((str), 0)
#endif

#endif  // STRINGENCODE_H_
