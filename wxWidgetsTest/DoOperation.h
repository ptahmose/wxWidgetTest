#pragma once

#include <functional>
#include <string>
#include <thread>

#include "compressionOptions.h"

class DoOperation
{
public:
    struct ProgressInformation
    {
        std::string message;
    };

    struct Parameters
    {
        std::string source_folder;
        std::string destination_folder;
        CompressionOptions compression_options;
        std::function<void(const ProgressInformation& information)> report_progress_functor;
    };
public:
    DoOperation();
    ~DoOperation();
    void Start(const Parameters& parameters);
    bool IsFinished();

private:
    std::thread worker_thread_;
    Parameters parameters_;

private:
    void RunOperation();
};
