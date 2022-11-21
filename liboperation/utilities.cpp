#include "utilities.h"

#include <codecvt>
#include <locale>

using namespace std;

std::wstring convertUtf8ToWide(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8conv;
    std::wstring conv = utf8conv.from_bytes(str);
    return conv;
}

std::string convertWideToUtf8(const std::wstring& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    std::string conv = utf8_conv.to_bytes(str);
    return conv;
}

std::filesystem::path canonicalize_path(const std::wstring& path)
{
    // ok, what we do is:
    // * we use "filesystem_path" in order to normalize the path (as detailed here https://en.cppreference.com/w/cpp/filesystem/path)
    // * we append a forward-slash at the end
    std::filesystem::path filesystem_path;

    // note: on Windows, 'preferred_separator' is '\', on Linux it's '/'
    if (!path.empty() && path.back() != L'/' && path.back() != std::filesystem::path::preferred_separator)
    {
        filesystem_path = filesystem::path(path + L'/');
    }
    else
    {
        filesystem_path = filesystem::path(path);
    }

    auto filesystem_path_normalized = filesystem_path.lexically_normal();
    return filesystem_path_normalized;
}