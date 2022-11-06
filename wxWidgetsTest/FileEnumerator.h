#pragma once

#include <string>
#include <memory>
#include <filesystem>

class FileEnumerator
{
private:
    std::filesystem::directory_iterator current_directory_iterator_;
    std::filesystem::recursive_directory_iterator current_recursive_directory_iterator_;
    bool is_recursive_mode{ false };
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

    /// If the iterator is not pointing to a "regular file", the iterator will be incremented
    /// until it points to a "regular file" or the enumeration is ended.
    /// \param [in,out] it  The iterator.
    static void IterateIfNecessaryEnsureRegularFile(std::filesystem::directory_iterator& it);

    static void IterateIfNecessaryEnsureRegularFile(std::filesystem::recursive_directory_iterator& it);
};