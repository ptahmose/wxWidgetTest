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

    this->source_folder_ = canonicalize_path(convertUtf8ToWide(parameters.source_folder));
    this->destination_folder_ = canonicalize_path(convertUtf8ToWide(parameters.destination_folder));

    this->file_enumerator_ = make_unique<FileEnumerator>();
    this->file_enumerator_->SetFolder(this->source_folder_.generic_wstring(), parameters.recursive_folder_traversal);

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

        auto destination_file = this->DetermineDestinationFilename(item.filename_wide);

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

void DoOperation::ProcessFile(const FileEnumerator::Item& source_file, const std::filesystem::path& destination_file)
{
    uint32_t characters_to_go_back;
    string percent_done = DoOperation::GetPercentString(0);
    characters_to_go_back = static_cast<uint32_t>(percent_done.size());

    // for display purposes, convert the "generic path representations" into the "preferred format",
    //  i.e. on Windows use backslashes
    filesystem::path destination_file_preferred = filesystem::path(destination_file).make_preferred();
    filesystem::path source_file_preferred = filesystem::path(source_file.filename_wide).make_preferred();

    ProgressInformation progress_information;
    ostringstream ss;
    ss << source_file_preferred.u8string() << " -> " << destination_file_preferred.u8string() << " : " << percent_done;

    progress_information.message = ss.str();
    progress_information.message_valid = true;
    this->parameters_.report_progress_functor(progress_information);

    // TODO: here we would not do the processing...
    for (float f = 0; f <= 100; f += 9.3f)
    {
        this_thread::sleep_for(std::chrono::milliseconds(100));

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

std::filesystem::path DoOperation::DetermineDestinationFilename(const std::wstring& filename)
{
    const filesystem::path dest_filename_path(filename);
    const filesystem::path rel_filename = dest_filename_path.lexically_relative(this->source_folder_);
    filesystem::path dest_filename{ this->destination_folder_ };
    dest_filename /= rel_filename;
    return dest_filename.generic_wstring();
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
