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
        bool message_valid{ false };
        std::uint32_t remove_characters_before_adding_message{ 0 };
        std::string message;

        bool no_of_files_processed_valid{ false };
        std::uint64_t no_of_files_processed{ 0 };

        bool data_size_of_files_processed_before_compression_valid{ false };
        std::uint64_t data_size_of_files_before_compression_processed{ 0 };

        bool data_size_of_files_processed_after_compression_valid{ false };
        std::uint64_t data_size_of_files_processed_after_compression{ 0 };

        bool progress_of_current_file_valid{ false };
        float progress_of_current_file{ 0 };
    };

    struct Parameters
    {
        /// Whether subfolders (within the source-folder) are to be traversed.
        bool recursive_folder_traversal{ false };

        /// The source-folder (in UTF8 encoding).
        std::string source_folder;

        /// The destination-folder (in UTF8 encoding).
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

    std::wstring source_folder_;
    std::wstring destination_folder_;
    Parameters parameters_;

    FileEnumerator file_enumerator_;

    uint64_t total_number_of_files_processed_ = 0;
    uint64_t total_sum_of_filesizes_of_files_processed_ = 0;

private:
    void RunOperation();
    std::wstring GenerateDestinationFilename(const std::wstring& filename);
    void ProcessFile(const FileEnumerator::Item& source_file, const std::wstring& destination_file);

    static std::string GetPercentString(float f);
};
