#include "FileEnumerator.h"

using namespace std;

FileEnumerator::FileEnumerator()
{
    
}

void FileEnumerator::SetFolder(const std::wstring& folder_name, bool recursive)
{
    filesystem::path path{folder_name};
    this->directory_enumerator_ = make_unique<std::filesystem::directory_iterator>(path);
    this->current_iterator_ = filesystem::begin(*this->directory_enumerator_);
}

bool FileEnumerator::GetNext(Item& item)
{
    if (this->current_iterator_ == filesystem::end(*this->directory_enumerator_))
    {
        return false;
    }

    item.filename_utf8 = this->current_iterator_->path().u8string();
    item.filename_wide = this->current_iterator_->path().wstring();
    item.file_size = this->current_iterator_->file_size();

    ++this->current_iterator_;
    return true;
}