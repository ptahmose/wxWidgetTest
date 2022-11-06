#include <memory>
#include <wx/dirdlg.h>

#include "wxWidgetsTest.h"
#include "wxProgressInfoClientData.h"
#include "compressionOptions.h"
#include "DoOperation.h"

using namespace std;

Frame::Frame() : wxFrame(nullptr, wxID_ANY, "wxWidget Demo")
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

    wxBoxSizer* statsizer = new wxStaticBoxSizer(new wxStaticBox(p, wxID_ANY, "Options"), wxVERTICAL);
    wxFlexGridSizer* optionsGridsizer = new wxFlexGridSizer(2, 5, 5);
    optionsGridsizer->Add(new wxStaticText(p, wxID_ANY, "recursive:"), wxSizerFlags().Align(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL));
    this->options_recursive_ctrl_ = new wxCheckBox(p, wxID_ANY, "");
    optionsGridsizer->Add(this->options_recursive_ctrl_);
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
    statisticsGridsizer->Add(new wxTextCtrl(p, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_RIGHT | wxTE_READONLY), wxSizerFlags().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL));
    statisticsGridsizer->Add(new wxStaticText(p, wxID_ANY, "data size reduced by:"), wxSizerFlags().Align(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL));
    statisticsGridsizer->Add(new wxTextCtrl(p, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_RIGHT | wxTE_READONLY), wxSizerFlags().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL));
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

/*static*/std::string Frame::GetDisplayText(CompressionOptions::WhatToCompress value)
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

CompressionOptions Frame::GetCompressionOptions()
{
    CompressionOptions compression_options;
    int i = this->options_what_to_compress_ctrl_->GetSelection();
    compression_options.what_to_compress = static_cast<CompressionOptions::WhatToCompress>(i);
    compression_options.zstd_level = this->options_compression_level_ctrl_->GetValue();
    return compression_options;
}

void Frame::OnStartButton(wxCommandEvent& event)
{
    auto compression_options = this->GetCompressionOptions();
    DoOperation::Parameters parameters;

    // Note: for dealing with Unicode -> https://docs.wxwidgets.org/trunk/overview_unicode.html
    parameters.recursive_folder_traversal = this->options_recursive_ctrl_->IsChecked();
    parameters.source_folder = this->source_folder_ctrl_->GetValue().utf8_str();
    parameters.destination_folder = this->destination_folder_ctrl_->GetValue().utf8_str();
    parameters.compression_options = this->GetCompressionOptions();
    parameters.report_progress_functor = [this](const DoOperation::ProgressInformation& information)->void {this->ProgressEvent(information); };
    this->operation_.Start(parameters);
}

void Frame::ProgressEvent(const DoOperation::ProgressInformation& information)
{
    // Note that this method is called from an arbitrary thread context
    wxCommandEvent* event = new  wxCommandEvent(wxEVT_PROGRESS_EVENT, PROGRESS_EVENT_ID);
    event->SetClientObject(new wxProgressInfoClientData(information));

    // The event-queue will take ownership of the event-object.
    // However - note that nobody seems to delete the "client-object", the command-object does not
    //  seem do this. This is kind of fishy. It would maybe more prudent if we would maintain our own
    //  queue here, and use the event just to notify the UI-thread.
    this->GetEventHandler()->QueueEvent(event);
}

void Frame::OnProgressEvent(wxCommandEvent& event)
{
    // get the number sent along with the event and use it to update the GUI
    wxProgressInfoClientData* progress_info_client_data = dynamic_cast<wxProgressInfoClientData*>(event.GetClientObject());

    const auto& progress_info = progress_info_client_data->GetProgressInformation();

    if (progress_info.message_valid)
    {
        if (progress_info.remove_characters_before_adding_message > 0)
        {
            const auto lastPosition = this->log_text_ctrl_->GetLastPosition();
            const wxTextPos startPosition = progress_info.remove_characters_before_adding_message < static_cast<wxTextPos>(lastPosition) ?
                                            (lastPosition - progress_info.remove_characters_before_adding_message) : 0;
            this->log_text_ctrl_->Remove(startPosition, lastPosition);
        }

        this->log_text_ctrl_->AppendText(progress_info.message);
    }

    //if (progress_info.)

    // this is quite fishy, see comment when adding the event
    event.SetClientObject(nullptr);
    delete progress_info_client_data;
}

void Frame::OnChooseSourceFolderButton(wxCommandEvent& event)
{
    this->ChooseFolderHandler(this->source_folder_ctrl_);
}

void Frame::OnChooseDestinationFolderButton(wxCommandEvent& event)
{
    this->ChooseFolderHandler(this->destination_folder_ctrl_);
}

void Frame::ChooseFolderHandler(wxTextCtrl* text_control)
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

wxBEGIN_EVENT_TABLE(Frame, wxFrame)
EVT_BUTTON(Frame::Ids::ChooseSourceFolderButton, Frame::OnChooseSourceFolderButton)
EVT_BUTTON(Frame::Ids::ChooseDestinationFolderButton, Frame::OnChooseDestinationFolderButton)
EVT_BUTTON(Frame::Ids::StartButton, Frame::OnStartButton)
EVT_COMMAND(Frame::PROGRESS_EVENT_ID, wxEVT_PROGRESS_EVENT, Frame::OnProgressEvent)
wxEND_EVENT_TABLE()


// ----------------------------------------------------------------------------

class WxWidgetsTestApplication : public wxApp
{
    bool OnInit() override
    {
        (new Frame())->Show();
        return true;
    }
};

wxIMPLEMENT_APP(WxWidgetsTestApplication);
