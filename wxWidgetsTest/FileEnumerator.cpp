#include "FileEnumerator.h"
#include "utilities.h"

using namespace std;

FileEnumerator::FileEnumerator()
{
}

void FileEnumerator::SetFolder(const std::wstring& folder_name, bool recursive)
{
    const filesystem::path path{ folder_name };

    if (!recursive)
    {
        auto directory_enumerator_ = std::filesystem::directory_iterator{ path };

        // Note:
        // For reasons beyond me, if we directly assign to "this->current_directory_iterator_", we get a linker-error with
        // gcc. c.f https://gcc.gnu.org/bugzilla/show_bug.cgi?id=91067
        auto iterator = filesystem::begin(directory_enumerator_);
        FileEnumerator::IterateIfNecessaryEnsureRegularFile(iterator);
        this->current_directory_iterator_ = iterator;
    }
    else
    {
        auto directory_recursive_enumerator_ = std::filesystem::recursive_directory_iterator{ path };
        auto iterator = filesystem::begin(directory_recursive_enumerator_);
        FileEnumerator::IterateIfNecessaryEnsureRegularFile(iterator);
        this->current_recursive_directory_iterator_ = iterator;
    }

    this->is_recursive_mode = recursive;
}

bool FileEnumerator::GetNext(Item& item)
{
    if (!this->is_recursive_mode)
    {
        // c.f. https://en.cppreference.com/w/cpp/filesystem/directory_iterator
        // -> a default constructed iterator is marks the end of the enumeration
        if (this->current_directory_iterator_ == filesystem::directory_iterator())
        {
            return false;
        }

        item.filename_utf8 = this->current_directory_iterator_->path().u8string();
        item.filename_wide = this->current_directory_iterator_->path().wstring();
        item.file_size = this->current_directory_iterator_->file_size();

        ++this->current_directory_iterator_;
        FileEnumerator::IterateIfNecessaryEnsureRegularFile(this->current_directory_iterator_);
        return true;
    }
    else
    {
        if (this->current_recursive_directory_iterator_ == filesystem::recursive_directory_iterator())
        {
            return false;
        }

        item.filename_utf8 = this->current_recursive_directory_iterator_->path().u8string();
        item.filename_wide = this->current_recursive_directory_iterator_->path().wstring();
        item.file_size = this->current_recursive_directory_iterator_->file_size();

        ++this->current_recursive_directory_iterator_;
        FileEnumerator::IterateIfNecessaryEnsureRegularFile(this->current_recursive_directory_iterator_);
        return true;
    }
}
