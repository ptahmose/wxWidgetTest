#pragma once

#include <wx/webview.h>
#include <rapidjson/document.h>
#include <DoOperation.h>

class WebFrame : public wxFrame
{
private:
    wxWebView* web_view_;

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

    /*Test*/
    void ButtonOneClicked(wxCommandEvent& event);
};
