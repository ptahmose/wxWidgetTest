#pragma once

#include <string>
#include <memory>
#include <filesystem>

/// This class is implementing a "file enumerator" - given a folder, it will enumerate all files within
/// this folder (and, optionally, all files within subfolders).
/// After construction, a folder needs to be set and whether the enumeration should be recursive or not.
/// Then, the "GetNext"-method will return all the files, one by one.
/// Note that in current implementation, an instance can only be used once - i.e. "SetFolder" may be
/// called exactly once. And there is no way to reset the enumeration or something like that.
class FileEnumerator
{
private:
    std::filesystem::directory_iterator current_directory_iterator_;
    std::filesystem::recursive_directory_iterator current_recursive_directory_iterator_;
    bool is_recursive_mode_{ false };
    bool is_initialized_{ false };
public:
    struct Item
    {
        std::uint64_t file_size;

        std::wstring filename_wide;

        std::string filename_utf8;
    };
public:
    FileEnumerator();

    void SetFolder(const std::wstring& folder_name, bool recursive);

    bool GetNext(Item& item);

private:
    /// If the iterator is not pointing to a "regular file", the iterator will be incremented
    /// until it points to a "regular file" or the enumeration is ended.
    ///
    /// \typeparam  t   Generic type parameter (expecting filesystem::directory_iterator or filesystem::recursive_directory_iterator here).
    /// \param [in,out] it  The iterator.
    template <typename t>
    static void IterateIfNecessaryEnsureRegularFile(t& it)
    {
        while (it != t{})
        {
            if (it->is_regular_file())
            {
                break;
            }

            ++it;
        }
    }
};
