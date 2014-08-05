#pragma once
#ifndef _STRING_UTILS_H_
#define _STRING_UTILS_H_
#include <cstring>
#include <string>
#include <windows.h>

inline std::wstring ansi2wide(const char* ansi, size_t ansiLength = 0/*auto detect*/)
{
    std::wstring result;
    if (!ansiLength)
        ansiLength = strlen(ansi);

    int sizeNeeded = ::MultiByteToWideChar(CP_ACP, 0, ansi, ansiLength, NULL, 0);
    if (sizeNeeded > 0)
    {
        result.resize(sizeNeeded);
        ::MultiByteToWideChar(CP_ACP, 0, ansi, ansiLength, &result[0], result.size());
    }

    return result;
}

inline std::wstring ansi2wide(const std::string& ansi)
{
    return ansi2wide(ansi.c_str(), ansi.length());
}

inline std::string wide2ansi(const wchar_t* wide, size_t wideLength = 0/*auto detect*/)
{
    std::string result;
    if(!wideLength)
        wideLength = wcslen(wide);

    int sizeNeeded = ::WideCharToMultiByte(CP_ACP, 0, wide, wideLength, NULL, 0, NULL, NULL);
    if (sizeNeeded > 0)
    {
        result.resize(sizeNeeded);
        ::WideCharToMultiByte(CP_ACP, 0, wide, wideLength, &result[0], result.size(), NULL, NULL);
    }

    return result;
}

inline std::string wide2ansi(const std::wstring& wide)
{
    return wide2ansi(wide.c_str(), wide.length());
}

#endif // _STRING_UTILS_H_