#include <memory>
#include <wx/wx.h>
#include <wx/dirdlg.h>
#include <wx/stdpaths.h>
#include <wx/textctrl.h>
#include <wx/statline.h>
#include <wx/windowptr.h>
#include <wx/spinctrl.h>
#include "wxWidgetsTest.h"
#include "wxProgressInfoClientData.h"
#include "compressionOptions.h"
#include "DoOperation.h"

using namespace std;

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
    Frame() : wxFrame(nullptr, wxID_ANY, "wxWidget Demo")
    {
        // SetIcon(wxICON(sample));
        wxPanel* p = new wxPanel(this, wxID_ANY);
        wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
        p->SetSizer(topsizer);

        wxFlexGridSizer* flexGridsizer = new wxFlexGridSizer(
            3, // columns
            5, // vertical gap
            5);// horizontal gap
        flexGridsizer->SetFlexibleDirection(wxHORIZONTAL);
        flexGridsizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_NONE);
        flexGridsizer->AddGrowableCol(1);
        flexGridsizer->Add(new wxStaticText(p, wxID_ANY, "Source Folder:"), wxSizerFlags().Align(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL));
        this->source_folder_ctrl_ = new wxTextCtrl(p, wxID_ANY, "");
        flexGridsizer->Add(this->source_folder_ctrl_, wxSizerFlags(1).Align(wxGROW | wxALIGN_CENTER_VERTICAL));
        wxButton* button = new wxButton(p, Ids::ChooseSourceFolderButton, " ... ", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
        flexGridsizer->Add(button, wxSizerFlags(1).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 1));
        flexGridsizer->Add(new wxStaticText(p, wxID_ANY, "Destination Folder:"), wxSizerFlags().Align(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL));
        this->destination_folder_ctrl_ = new wxTextCtrl(p, wxID_ANY, "");
        flexGridsizer->Add(this->destination_folder_ctrl_, wxSizerFlags(1).Align(wxGROW | wxALIGN_CENTER_VERTICAL));
        button = new wxButton(p, Ids::ChooseDestinationFolderButton, " ... ", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
        flexGridsizer->Add(button, wxSizerFlags(1).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 1));
        topsizer->Add(flexGridsizer, wxSizerFlags().Proportion(0).Expand().Border(wxALL, 10));

        wxBoxSizer* statsizer = new wxStaticBoxSizer(
            new wxStaticBox(p, wxID_ANY, "Options"), wxVERTICAL);
        wxFlexGridSizer* optionsGridsizer = new wxFlexGridSizer(2, 5, 5);
        optionsGridsizer->Add(new wxStaticText(p, wxID_ANY, "What to compress:"),
            wxSizerFlags().Align(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL));
        wxArrayString items;
        items.Add(GetDisplayText(CompressionOptions::WhatToCompress::kOnlyUncompressed));
        items.Add(GetDisplayText(CompressionOptions::WhatToCompress::kUncompressedAndZstd));
        items.Add(GetDisplayText(CompressionOptions::WhatToCompress::kUncompressedAndZstdAndJpgxr));
        this->options_what_to_compress_ctrl_ = new wxComboBox(p, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
            items,
            wxCB_READONLY | wxCB_DROPDOWN);
        this->options_what_to_compress_ctrl_->SetSelection(0);
        optionsGridsizer->Add(this->options_what_to_compress_ctrl_, wxSizerFlags(1).Align(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL));
        optionsGridsizer->Add(new wxStaticText(p, wxID_ANY, "compression level:"), wxSizerFlags().Align(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL));
        this->options_compression_level_ctrl_ = new wxSpinCtrl(p, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
        this->options_compression_level_ctrl_->SetRange(-2 * 65535, 22);
        optionsGridsizer->Add(this->options_compression_level_ctrl_, wxSizerFlags(1).Align(wxGROW | wxALIGN_CENTER_VERTICAL));

        statsizer->Add(optionsGridsizer);
        topsizer->Add(statsizer, wxSizerFlags().Expand().Border(wxALL, 10));

        wxBoxSizer* button_box = new wxBoxSizer(wxHORIZONTAL);
        button_box->Add(new wxButton(p, Ids::StartButton, "Start"), wxSizerFlags().Border(wxALL, 7));
        topsizer->Add(button_box, wxSizerFlags().Center());

        wxFlexGridSizer* statisticsGridsizer = new wxFlexGridSizer(4, 5, 5);
        statisticsGridsizer->Add(new wxStaticText(p, wxID_ANY, "files processed:"), wxSizerFlags().Align(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL));
        statisticsGridsizer->Add(new wxTextCtrl(p, wxID_ANY, "235434", wxDefaultPosition, wxDefaultSize, wxTE_RIGHT | wxTE_READONLY), wxSizerFlags().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL));
        statisticsGridsizer->Add(new wxStaticText(p, wxID_ANY, "data size reduced by:"), wxSizerFlags().Align(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL));
        statisticsGridsizer->Add(new wxTextCtrl(p, wxID_ANY, "235434", wxDefaultPosition, wxDefaultSize, wxTE_RIGHT | wxTE_READONLY), wxSizerFlags().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL));
        topsizer->Add(statisticsGridsizer, wxSizerFlags().Center().Border(wxALL, 10));

        topsizer->Add(
            new wxStaticLine(p, wxID_ANY, wxDefaultPosition, wxSize(3, 3), wxHORIZONTAL),
            wxSizerFlags().Proportion(0).Expand());

        this->log_text_ctrl_ = new wxTextCtrl(p, wxID_ANY, "", wxDefaultPosition, wxSize(100, 60), wxTE_MULTILINE | wxTE_READONLY);
        topsizer->Add(
            log_text_ctrl_,
            wxSizerFlags(1).Expand().Proportion(1).Border(wxALL, 5).Align(wxALIGN_TOP));

        // don't allow frame to get smaller than what the sizers tell it and also set
        // the initial size as calculated by the sizers
        topsizer->SetSizeHints(this);
    }
private:
    wxDECLARE_EVENT_TABLE();

    static std::string GetDisplayText(CompressionOptions::WhatToCompress value)
    {
        switch (value)
        {
        case CompressionOptions::WhatToCompress::kOnlyUncompressed:
            return "only uncompressed subblocks";
        case CompressionOptions::WhatToCompress::kUncompressedAndZstd:
            return "uncompressed & zstd compressed subblocks";
        case CompressionOptions::WhatToCompress::kUncompressedAndZstdAndJpgxr:
            return "uncompressed & zstd & JPGXR compressed subblocks";
        }

        return "";
    }

    CompressionOptions GetCompressionOptions()
    {
        CompressionOptions compression_options;
        int i = this->options_what_to_compress_ctrl_->GetSelection();
        compression_options.what_to_compress = static_cast<CompressionOptions::WhatToCompress>(i);
        compression_options.zstd_level = this->options_compression_level_ctrl_->GetValue();
        return compression_options;
    }

    void OnStartButton(wxCommandEvent& event)
    {
        auto compression_options = this->GetCompressionOptions();
        DoOperation::Parameters parameters;

        // Note: for dealing with Unicode -> https://docs.wxwidgets.org/trunk/overview_unicode.html
        parameters.source_folder = this->source_folder_ctrl_->GetValue().utf8_str();
        parameters.destination_folder = this->destination_folder_ctrl_->GetValue().utf8_str();
        parameters.compression_options = this->GetCompressionOptions();
        parameters.report_progress_functor = [this](const DoOperation::ProgressInformation& information)->void {this->ProgressEvent(information); };
        this->operation_.Start(parameters);
    }

    void ProgressEvent(const DoOperation::ProgressInformation& information)
    {
        wxCommandEvent event(wxEVT_PROGRESS_EVENT, PROGRESS_EVENT_ID);
        event.SetClientObject(new wxProgressInfoClientData(information));
        this->GetEventHandler()->AddPendingEvent(event);
    }

    void OnProgressEvent(wxCommandEvent& event)
    {
        // get the number sent along the event and use it to update the GUI
        //m_spinner->SetValue(evt.GetInt());
        const auto& progress_info = static_cast<wxProgressInfoClientData*>(event.GetClientObject())->GetProgressInformation();
        this->log_text_ctrl_->AppendText(progress_info.message);
    }

    void OnChooseSourceFolderButton(wxCommandEvent& event)
    {
        this->ChooseFolderHandler(this->source_folder_ctrl_);
    }

    void OnChooseDestinationFolderButton(wxCommandEvent& event)
    {
        this->ChooseFolderHandler(this->destination_folder_ctrl_);
    }

    void ChooseFolderHandler(wxTextCtrl* text_control)
    {
        wxWindowPtr<wxDirDialog> folderBrowserDialog(new wxDirDialog(this, wxEmptyString));
        if (text_control->GetValue().IsEmpty())
        {
            folderBrowserDialog->SetPath(wxStandardPaths::Get().GetDocumentsDir());
        }
        else
        {
            folderBrowserDialog->SetPath(text_control->GetValue());
        }

        folderBrowserDialog->Bind(
            wxEVT_WINDOW_MODAL_DIALOG_CLOSED,
            [text_control, folderBrowserDialog](wxWindowModalDialogEvent& event)
            {
                if (event.GetReturnCode() == wxID_OK)
                {
                    text_control->Clear();
                    text_control->AppendText(folderBrowserDialog->GetPath());
                }
            });
        folderBrowserDialog->ShowWindowModal();
    }
};

wxBEGIN_EVENT_TABLE(Frame, wxFrame)
EVT_BUTTON(Frame::Ids::ChooseSourceFolderButton, Frame::OnChooseSourceFolderButton)
EVT_BUTTON(Frame::Ids::ChooseDestinationFolderButton, Frame::OnChooseDestinationFolderButton)
EVT_BUTTON(Frame::Ids::StartButton, Frame::OnStartButton)
EVT_COMMAND(Frame::PROGRESS_EVENT_ID, wxEVT_PROGRESS_EVENT, Frame::OnProgressEvent)
wxEND_EVENT_TABLE()

#if 0
namespace Examples {
    class Frame : public wxFrame {
    public:
        Frame() : wxFrame(nullptr, wxID_ANY, "DirDialog (ShowWindowModal) example")
        {
            button->Bind(wxEVT_BUTTON,
                [&](wxCommandEvent& event)
                {
                    wxWindowPtr<DirDialog> folderBrowserDialog(new DirDialog(this, wxEmptyString));
                    folderBrowserDialog->SetPath(wxStandardPaths::Get().GetDocumentsDir());
                    folderBrowserDialog->ShowWindowModal();
                    folderBrowserDialog->Bind(wxEVT_WINDOW_MODAL_DIALOG_CLOSED, [this, folderBrowserDialog](wxWindowModalDialogEvent& event) {
                        if (event.GetReturnCode() == wxID_OK)
                            label->SetLabel(wxString::Format("Path = %s", folderBrowserDialog->GetPath()));
                        });
                });
        }

    private:
        wxPanel* panel = new wxPanel(this);
        wxButton* button = new wxButton(panel, wxID_ANY, "Folder...", { 10, 10 });
        wxStaticText* label = new wxStaticText(panel, wxID_ANY, "Path = ", { 10, 40 });
        wxTextCtrl* logTextXtrl = new wxTextCtrl(panel, wxID_ANY, wxString(), /*wxDefaultPosition*/{ -1,-1 }, { 300,100 } /* wxDefaultSize*/, wxTE_MULTILINE);
        //m_log->SetMinSize(wxSize(-1, 100));
    };

    class Application : public wxApp {
        bool OnInit() override {
            (new Frame())->Show();
            return true;
        }
    };
}
#endif

class CziCompressApplication : public wxApp
{
    bool OnInit() override
    {
        (new Frame())->Show();
        return true;
    }
};

wxIMPLEMENT_APP(CziCompressApplication);
//wxIMPLEMENT_APP(Examples::Application);
