#pragma once

#include <string>
#include <memory>
#include <filesystem>

class FileEnumerator
{
private:
    std::unique_ptr<std::filesystem::directory_iterator> directory_enumerator_;
    std::filesystem::directory_iterator current_iterator_;
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
};