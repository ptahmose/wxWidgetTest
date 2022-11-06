#pragma once

#include <functional>
#include <string>
#include <thread>

#include "compressionOptions.h"
#include "FileEnumerator.h"

class DoOperation
{
public:
    struct ProgressInformation
    {
        std::string message;

        bool no_of_files_processed_valid{ false };
        std::uint64_t no_of_files_processed{ 0 };

        bool data_size_of_files_processed_before_compression_valid{ false };
        std::uint64_t data_size_of_files_before_compression_processed{ 0 };

        bool data_size_of_files_processed_after_compression_valid{ false };
        std::uint64_t data_size_of_files_processed_after_compression{ 0 };

        bool progress_of_current_file_valid{false};
        float progress_of_current_file{ 0 };
    };

    struct Parameters
    {
        bool recursive_folder_traversal{ false };
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

    FileEnumerator file_enumerator_;

private:
    void RunOperation();
};
