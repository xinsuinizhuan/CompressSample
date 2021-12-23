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
#include "stdafx.h"
#include "CmdlineParse.h"
#include <locale>
#include <algorithm>
#if (defined _WIN32 || defined WIN32)
#include <tchar.h>
#endif
#include <map>

class CmdLineParser::Impl {
   public:
    Impl() { value_map_.clear(); }

    ~Impl() { value_map_.clear(); }

    ITERPOS findKey(const wstring& key) const {
        wstring s = akali::StringCaseConvert(key, akali::EasyCharToLowerW);
        return value_map_.find(s);
    }

    CmdLineParser::ValsMap value_map_;
};

const wchar_t delims[] = L"-/";
const wchar_t quotes[] = L"\"";
const wchar_t value_sep[] = L" :=";  // don't forget space!!

CmdLineParser::CmdLineParser(const wstring& cmdline) {
    impl_ = new Impl();

    if (cmdline.length() > 0) {
        Parse(cmdline);
    }
}

CmdLineParser::~CmdLineParser() {
    SAFE_DELETE(impl_);
}

bool CmdLineParser::Parse(const wstring& cmdline) {
    const wstring sEmpty = L"";
    int nArgs = 0;

    impl_->value_map_.clear();
    cmdline_ = cmdline;

    wstring strW = cmdline_;
    const wchar_t* sCurrent = strW.c_str();

    for (;;) {
        if (sCurrent[0] == L'\0')
            break;

        // 查找任一分隔符
        const wchar_t* sArg = wcspbrk(sCurrent, delims);

        if (!sArg)
            break;

        sArg++;  // 字符指针sArg向后移动一个字符

        if (sArg[0] == L'\0')
            break;  // ends with delim

        const wchar_t* sVal = wcspbrk(sArg, value_sep);

        if (sVal == NULL) {
            wstring Key(sArg);
            Key = akali::StringCaseConvert(Key, akali::EasyCharToLowerW);
            impl_->value_map_.insert(CmdLineParser::ValsMap::value_type(Key, sEmpty));
            break;
        }
        else if (sVal[0] == L' ' || wcslen(sVal) == 1) {
            // cmdline ends with /Key: or a key with no value
            wstring Key(sArg, (int)(sVal - sArg));

            if (Key.length() > 0) {
                Key = akali::StringCaseConvert(Key, akali::EasyCharToLowerW);
                impl_->value_map_.insert(CmdLineParser::ValsMap::value_type(Key, sEmpty));
            }

            sCurrent = sVal + 1;
            continue;
        }
        else {
            // key has value
            wstring Key(sArg, (int)(sVal - sArg));
            Key = akali::StringCaseConvert(Key, akali::EasyCharToLowerW);

            sVal++;

            const wchar_t* sQuote = wcspbrk(sVal, quotes);
            const wchar_t* sEndQuote = NULL;

            if (sQuote == sVal) {
                // string with quotes (defined in quotes, e.g. '")
                sQuote = sVal + 1;
                sEndQuote = wcspbrk(sQuote, quotes);
            }
            else {
                sQuote = sVal;
                sEndQuote = wcschr(sQuote, L' ');
            }

            if (sEndQuote == NULL) {
                // no end quotes or terminating space, take the rest of the string to its end
                wstring csVal(sQuote);

                if (Key.length() > 0) {
                    impl_->value_map_.insert(CmdLineParser::ValsMap::value_type(Key, csVal));
                }

                break;
            }
            else {
                // end quote
                if (Key.length() > 0) {
                    wstring csVal(sQuote, (int)(sEndQuote - sQuote));
                    impl_->value_map_.insert(CmdLineParser::ValsMap::value_type(Key, csVal));
                }

                sCurrent = sEndQuote + 1;
                continue;
            }
        }
    }

    return (nArgs > 0);  // TRUE if arguments were found
}

bool CmdLineParser::HasKey(const wstring& key) const {
    ITERPOS it = impl_->findKey(key);

    if (it == impl_->value_map_.end())
        return false;

    return true;
}

bool CmdLineParser::HasVal(const wstring& key) const {
    ITERPOS it = impl_->findKey(key);

    if (it == impl_->value_map_.end())
        return false;

    if (it->second.length() == 0)
        return false;

    return true;
}

wstring CmdLineParser::GetVal(const wstring& key) const {
    ITERPOS it = impl_->findKey(key);

    if (it == impl_->value_map_.end())
        return wstring();

    return it->second;
}

int CmdLineParser::GetParamCount() const {
    return impl_->value_map_.size();
}

CmdLineParser::ITERPOS CmdLineParser::Begin() const {
    return impl_->value_map_.begin();
}

CmdLineParser::ITERPOS CmdLineParser::End() const {
    return impl_->value_map_.end();
}