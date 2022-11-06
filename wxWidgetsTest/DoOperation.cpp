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
    if (this->worker_thread_.joinable())
    {
        this->worker_thread_.join();
    }
}

void DoOperation::Start(const Parameters& parameters)
{
    this->file_enumerator_.SetFolder(convertUtf8ToWide(parameters.source_folder), parameters.recursive_folder_traversal);

    this->parameters_ = parameters;
    this->worker_thread_ = std::thread([this] {this->RunOperation(); });
}

bool DoOperation::IsFinished()
{
    return false;
}

void DoOperation::RunOperation()
{
    uint64_t total_number_of_files_processed = 0;
    uint64_t total_sum_of_files = 0;
    for (;;)
    {
        FileEnumerator::Item item;
        bool isValid = this->file_enumerator_.GetNext(item);
        if (!isValid)
        {
            break;
        }

        wstring destination_file = this->GenerateDestinationFilename(item.filename_wide);

        this->ProcessFile(item, destination_file);

        /*ProgressInformation progress_information;
        ostringstream ss;
        ss << "File: '" << item.filename_utf8 << "'" << endl;
        progress_information.message = ss.str();
        this->parameters_.report_progress_functor(progress_information);*/
    }
}

void DoOperation::ProcessFile(const FileEnumerator::Item& source_file, const std::wstring& destination_file)
{
    uint32_t characters_to_go_back;
    string percent_done = GetPercentString(0);
    characters_to_go_back = (uint32_t)percent_done.size();

    ProgressInformation progress_information;
    ostringstream ss;
    ss << source_file.filename_utf8 << " - " << percent_done;

    progress_information.message = ss.str();
    progress_information.message_valid = true;
    this->parameters_.report_progress_functor(progress_information);

    for (float f = 0; f <= 100; f += 9.3f)
    {
        this_thread::sleep_for(std::chrono::milliseconds(500));

        percent_done = GetPercentString(f);
        progress_information.message = percent_done;
        progress_information.remove_characters_before_adding_message = characters_to_go_back;
        this->parameters_.report_progress_functor(progress_information);
        characters_to_go_back = (uint32_t)percent_done.size();
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