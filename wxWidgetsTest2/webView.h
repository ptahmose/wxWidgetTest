#pragma once

#include <wx/webview.h>
#include <rapidjson/document.h>
#include <DoOperation.h>

class WebFrame : public wxFrame
{
private:
    static constexpr int PROGRESS_EVENT_ID = 100000;

    wxWebView* web_view_;

    DoOperation operation_;

    // IDs for the controls.
    enum Ids
    {
        WebView,
    };

public:
    WebFrame();

private:
    wxDECLARE_EVENT_TABLE();

    void OnScriptWxMsg(wxWebViewEvent& evt);

    static std::string EscapeForJavascript(const std::wstring& text);
    void ChooseFolderAndSetInWebsite(const std::string& id, const std::string& current_folder);

    static bool TryParseJson(const rapidjson::Document& json_document, DoOperation::Parameters& operation_parameters);

    void ProgressEvent(const DoOperation::ProgressInformation& information);
    void OnProgressEvent(wxCommandEvent& event);

    /*Test*/
    void ButtonOneClicked(wxCommandEvent& event);
};
