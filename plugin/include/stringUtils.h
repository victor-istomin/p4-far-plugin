#pragma once
#ifndef _STRING_UTILS_H_
#define _STRING_UTILS_H_
#include <cstring>
#include <string>
#include <windows.h>
#include <clocale>
#include <cctype>

// undef ugly win macros
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

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

// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

inline std::vector<std::string> tokenize(const std::string& input, std::string::value_type token)
{
    std::vector<std::string> result;
    std::string::size_type currentPosition = 0;

    while(currentPosition < input.length())
    {
        std::string::size_type nextToken     = input.find(token, currentPosition);
        std::string::size_type substringSize = nextToken == std::string::npos ? input.length() - currentPosition : nextToken - currentPosition;
        std::string            substring     = input.substr(currentPosition, substringSize);

        currentPosition += substringSize + 1;
        result.push_back(std::move(substring));
    }

    return result;
}

#endif // _STRING_UTILS_H_