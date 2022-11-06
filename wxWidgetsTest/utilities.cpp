#include "utilities.h"
#include <codecvt>

std::wstring convertUtf8ToWide(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring conv = converter.from_bytes(str);
    return conv;
}

std::string convertWideToUtf8(const std::wstring& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string conv = converter.to_bytes(str);
    return conv;
}
