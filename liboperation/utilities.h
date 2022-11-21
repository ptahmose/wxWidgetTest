#pragma once

#include <string>
#include <filesystem>

std::wstring convertUtf8ToWide(const std::string& str);
std::string convertWideToUtf8(const std::wstring& str);

/// Canonicalize the specified path (i.e. identified a folder). What this
/// does is ensure that it ends with a slash (or back-slash).
///
/// \param  path    Full pathname (i.e. a "folder path")
///
/// \returns    The "canonicalized" path.
std::filesystem::path canonicalize_path(const std::wstring& path);