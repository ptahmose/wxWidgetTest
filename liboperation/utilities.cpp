#include "utilities.h"

std::wstring convertUtf8ToWide(const std::string& str)
{
    std::wstring conv(str.size(), 0);
    size_t size = std::mbstowcs(&conv[0], str.c_str(), str.size());
    conv.resize(size);
    return conv;
}

std::string convertWideToUtf8(const std::wstring& str)
{
    size_t requiredSize = std::wcstombs(nullptr, str.c_str(), 0);
    std::string conv(requiredSize, 0);
    conv.resize(std::wcstombs(&conv[0], str.c_str(), requiredSize));
    return conv;
}
