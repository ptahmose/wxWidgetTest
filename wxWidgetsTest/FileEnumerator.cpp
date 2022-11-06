#include "FileEnumerator.h"
#include "utilities.h"

using namespace std;

  // Use explicit instantiations of these types. Any inconsistency in the
  // value of __default_lock_policy between code including this header and
  // the library will cause a linker error.
//   extern template class
//     std::__shared_ptr<std::filesystem::_Dir>;


FileEnumerator::FileEnumerator()
{
    
}

void FileEnumerator::SetFolder(const std::wstring& folder_name, bool recursive)
{
    filesystem::path path{convertWideToUtf8(folder_name),filesystem::path::native_format};
    auto directory_enumerator_ = std::filesystem::directory_iterator{path};

    // Note:
    // For reasons beyond me, if we directly assign to "this->current_iterator_", we get a linker-error with
    // gcc. c.f https://gcc.gnu.org/bugzilla/show_bug.cgi?id=91067
    auto iterator = filesystem::begin(directory_enumerator_);
    IterateUntilNextFile(iterator);
    this->current_iterator_ = iterator;
}

bool FileEnumerator::GetNext(Item& item)
{
    // c.f. https://en.cppreference.com/w/cpp/filesystem/directory_iterator
    // -> a default constructed iterator is marks the end of the enumeration
     if (this->current_iterator_ == filesystem::directory_iterator())
     {
         return false;
     }

    item.filename_utf8 = this->current_iterator_->path().u8string();
    item.filename_wide = this->current_iterator_->path().wstring();
    item.file_size = this->current_iterator_->file_size();

    ++this->current_iterator_;
    IterateUntilNextFile(this->current_iterator_);
    return true;
}

void FileEnumerator::IterateUntilNextFile(std::filesystem::directory_iterator& it)
{
    for (;;)
    {
        if (it == filesystem::directory_iterator())
        {
            break;
        }

        if (it->is_regular_file())
        {
            break;
        }

        ++it;
    }
}