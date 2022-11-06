#pragma once

#include <locale>
#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/textctrl.h>
#include <wx/statline.h>
#include <wx/windowptr.h>
#include <wx/spinctrl.h>

#include "DoOperation.h"

class Frame : public wxFrame
{
private:
    static constexpr int PROGRESS_EVENT_ID = 100000;
    static std::locale kFormatting_locale;

    // IDs for the controls.
    enum Ids
    {
        ChooseSourceFolderButton,
        ChooseDestinationFolderButton,
        StartButton,
        StopButton
    };

    DoOperation operation_;

    wxTextCtrl* source_folder_ctrl_;
    wxTextCtrl* destination_folder_ctrl_;
    wxCheckBox* options_recursive_ctrl_;
    wxComboBox* options_what_to_compress_ctrl_;
    wxSpinCtrl* options_compression_level_ctrl_;
    wxTextCtrl* statistics_files_processed_ctrl_;
    wxTextCtrl* statistics_total_original_filesize_ctrl_;
    wxTextCtrl* statistics_total_compressed_filesize_ctrl_;
    wxTextCtrl* log_text_ctrl_;
    wxButton* start_button_ctrl_;
    wxButton* stop_button_ctrl_;
public:
    Frame();

private:
    wxDECLARE_EVENT_TABLE();

    static std::string GetDisplayText(CompressionOptions::WhatToCompress value);
    static void SetNumericValueInCtrl(bool valid, std::uint64_t value, wxTextCtrl* text_control);
    CompressionOptions GetCompressionOptions();
    void OnStartButton(wxCommandEvent& event);
    void OnStopButton(wxCommandEvent& event);
    void ProgressEvent(const DoOperation::ProgressInformation& information);
    void OnProgressEvent(wxCommandEvent& event);
    void OnChooseSourceFolderButton(wxCommandEvent& event);
    void OnChooseDestinationFolderButton(wxCommandEvent& event);
    void ChooseFolderHandler(wxTextCtrl* text_control);
    void UpdateState(bool operation_ongoing);
};
