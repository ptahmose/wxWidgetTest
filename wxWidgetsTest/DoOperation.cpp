#include "DoOperation.h"
#include <chrono>
#include <sstream>

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
    this->parameters_ = parameters;
    this->worker_thread_ = std::thread([this] {this->RunOperation(); });
}

bool DoOperation::IsFinished()
{
    return false;
}

void DoOperation::RunOperation()
{
    for (int i = 0; i < 100; ++i)
    {
        this_thread::sleep_for(std::chrono::milliseconds(500));

        ostringstream ss;
        ss << "Message #" << i << endl;

        ProgressInformation progress_information;
        progress_information.message = ss.str();
        this->parameters_.report_progress_functor(progress_information);
    }
}