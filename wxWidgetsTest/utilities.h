#pragma once

#include <string>

std::wstring convertUtf8ToWide(const std::string& str);
std::string convertWideToUtf8(const std::wstring& str);