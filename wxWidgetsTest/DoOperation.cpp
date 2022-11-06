#include "DoOperation.h"
#include <chrono>
#include <sstream>
#include "utilities.h"

using namespace std;

DoOperation::DoOperation()
{
}

DoOperation::~DoOperation()
{
    this->cancellation_requested_.store(true);
    if (this->worker_thread_.joinable())
    {
        this->worker_thread_.join();
    }
}

void DoOperation::Start(const Parameters& parameters)
{
    if (this->worker_thread_.joinable())
    {
        this->worker_thread_.join();
    }

    this->file_enumerator_ = make_unique<FileEnumerator>();
    this->file_enumerator_->SetFolder(convertUtf8ToWide(parameters.source_folder), parameters.recursive_folder_traversal);

    this->cancellation_requested_.store(false);
    this->total_number_of_files_processed_ = 0;
    this->total_sum_of_filesizes_of_files_processed_ = 0;
    this->parameters_ = parameters;

    this->worker_thread_ = std::thread([this] {this->RunOperation(); });
}

void DoOperation::RequestCancel()
{
    this->cancellation_requested_.store(true);
}

void DoOperation::RunOperation()
{
    for (;;)
    {
        if (this->cancellation_requested_.load())
        {
            this->NotifyCancelled();
            break;
        }

        FileEnumerator::Item item;
        const bool isValid = this->file_enumerator_->GetNext(item);
        if (!isValid)
        {
            this->NotifyCompleted();
            break;
        }

        wstring destination_file = this->GenerateDestinationFilename(item.filename_wide);

        this->ProcessFile(item, destination_file);

        ++this->total_number_of_files_processed_;
        ++this->total_sum_of_filesizes_of_files_processed_ += item.file_size;

        ProgressInformation progress_information;
        progress_information.data_size_of_files_before_compression_processed = this->total_sum_of_filesizes_of_files_processed_;
        progress_information.data_size_of_files_processed_before_compression_valid = true;
        progress_information.no_of_files_processed = this->total_number_of_files_processed_;
        progress_information.no_of_files_processed_valid = true;
        this->parameters_.report_progress_functor(progress_information);
    }
}

void DoOperation::ProcessFile(const FileEnumerator::Item& source_file, const std::wstring& destination_file)
{
    uint32_t characters_to_go_back;
    string percent_done = DoOperation::GetPercentString(0);
    characters_to_go_back = static_cast<uint32_t>(percent_done.size());

    ProgressInformation progress_information;
    ostringstream ss;
    ss << source_file.filename_utf8 << " - " << percent_done;

    progress_information.message = ss.str();
    progress_information.message_valid = true;
    this->parameters_.report_progress_functor(progress_information);

    for (float f = 0; f <= 100; f += 9.3f)
    {
        this_thread::sleep_for(std::chrono::milliseconds(500));

        percent_done = DoOperation::GetPercentString(f);
        progress_information.message = percent_done;
        progress_information.remove_characters_before_adding_message = characters_to_go_back;
        this->parameters_.report_progress_functor(progress_information);
        characters_to_go_back = static_cast<uint32_t>(percent_done.size());
    }

    ss = ostringstream();
    ss << "done" << endl;

    progress_information.message = ss.str();
    progress_information.remove_characters_before_adding_message = characters_to_go_back;
    progress_information.message_valid = true;
    this->parameters_.report_progress_functor(progress_information);
}

std::wstring DoOperation::GenerateDestinationFilename(const std::wstring& filename)
{
    return L"dest";
}

/*static*/std::string DoOperation::GetPercentString(float f)
{
    ostringstream ss;
    ss << fixed << setprecision(1) << f << "%";
    return ss.str();
}

void DoOperation::NotifyCancelled()
{
    ProgressInformation progress_information;
    progress_information.operation_ongoing = false;

    ostringstream ss;
    ss << "Operation cancelled." << endl;
    progress_information.message = ss.str();
    progress_information.message_valid = true;
    this->parameters_.report_progress_functor(progress_information);
}

void DoOperation::NotifyCompleted()
{
    ProgressInformation progress_information;
    progress_information.operation_ongoing = false;

    ostringstream ss;
    ss << "Operation complete." << endl;
    progress_information.message = ss.str();
    progress_information.message_valid = true;
    this->parameters_.report_progress_functor(progress_information);
}
