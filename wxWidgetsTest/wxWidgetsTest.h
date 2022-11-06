#pragma once

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

    // IDs for the controls.
    enum Ids
    {
        ChooseSourceFolderButton,
        ChooseDestinationFolderButton,
        StartButton
    };

    DoOperation operation_;

    wxTextCtrl* source_folder_ctrl_;
    wxTextCtrl* destination_folder_ctrl_;
    wxComboBox* options_what_to_compress_ctrl_;
    wxSpinCtrl* options_compression_level_ctrl_;
    wxTextCtrl* statistics_files_processed_ctrl_;
    wxTextCtrl* statistics_data_sized_reduced_by_ctrl_;
    wxTextCtrl* log_text_ctrl_;

public:
    Frame();

private:
    wxDECLARE_EVENT_TABLE();

    static std::string GetDisplayText(CompressionOptions::WhatToCompress value);
    CompressionOptions GetCompressionOptions();
    void OnStartButton(wxCommandEvent& event);
    void ProgressEvent(const DoOperation::ProgressInformation& information);
    void OnProgressEvent(wxCommandEvent& event);
    void OnChooseSourceFolderButton(wxCommandEvent& event);
    void OnChooseDestinationFolderButton(wxCommandEvent& event);
    void ChooseFolderHandler(wxTextCtrl* text_control);
};
