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
    for (;;)
    {
        FileEnumerator::Item item;
        bool isValid = this->file_enumerator_.GetNext(item);
        if (!isValid)
        {
            break;
        }

        ProgressInformation progress_information;
        ostringstream ss;
        ss << "File: '" << item.filename_utf8 << "'" << endl;
        progress_information.message = ss.str();
        this->parameters_.report_progress_functor(progress_information);
    }

  
}