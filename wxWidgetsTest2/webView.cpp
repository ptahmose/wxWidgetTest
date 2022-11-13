#include <wx/wx.h>
#include <wx/filesys.h>
#include <wx/fs_arc.h>
#include <wx/fs_mem.h>
#include <wx/webview.h>
#include <wx/msw/webview_edge.h>
#include <wx/msw/webview_ie.h>
#include <wx/webviewfshandler.h>
#include <wx/windowptr.h>
#include "wx/stdpaths.h"

#include <sstream>
#include <tuple>
#include <array>
#include <algorithm>
#include "webView.h"

#include "htmlpage.h"
#include "wxlogo.xpm"

using namespace std;

WebFrame::WebFrame() : wxFrame(nullptr, wxID_ANY, "wxWidget-WebView Demo")
{
    wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(topsizer);
    bool b = wxWebView::IsBackendAvailable(wxWebViewBackendEdge);
    this->web_view_ = wxWebView::New(wxWebViewBackendEdge);
    this->web_view_->Create(this, Ids::WebView, "", wxDefaultPosition, wxDefaultSize);
    //this->web_view_ = new wxWebViewEdge(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
    topsizer->Add(this->web_view_, wxSizerFlags().Expand().Proportion(1));
    this->web_view_->SetPage(
        html_page,
        "");

    // this will allow to call from Javascript into this class (c.f. https://docs.wxwidgets.org/trunk/classwx_web_view.html#a2597c3371ed654bf03262ec6d34a0126)
    if (!this->web_view_->AddScriptMessageHandler("wx_msg"))
    {
        wxLogError("Could not add script message handler");
    }

    this->Bind(wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED, &WebFrame::OnScriptWxMsg, this, this->web_view_->GetId());

    // add a button below the web-control (for testing)
    wxButton* button = new wxButton(this, wxID_ANY, "Test", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    topsizer->Add(button, wxSizerFlags(1).Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL, 1).Proportion(0));
    button->Bind(wxEVT_BUTTON, &WebFrame::ButtonOneClicked, this);
}

void WebFrame::OnScriptWxMsg(wxWebViewEvent& evt)
{
    string message = evt.GetString().ToUTF8();
    rapidjson::Document document;
    document.Parse<0>(message.c_str());
    if (!document.HasParseError())
    {
        string id;
        string arg;
        if (document.HasMember("id") && document["id"].IsString())
        {
            id = document["id"].GetString();
        }

        if (document.HasMember("arg") && document["arg"].IsString())
        {
            arg = document["arg"].GetString();
        }

        if (id == "sourcefolderinputtextbox")
        {
            // the "browse-button" of the "source-folder-selection" has been clicked
            this->ChooseFolderAndSetInWebsite(id, arg);
        }
        else if (id == "destinationfolderinputtextbox")
        {
            // the "browse-button" of the "destination-folder-selection" has been clicked
            this->ChooseFolderAndSetInWebsite(id, arg);
        }
        else if (id == "startbutton")
        {
            DoOperation::Parameters operation_parameters;
            TryParseJson(document, operation_parameters);
        }
    }
}

/*static*/bool WebFrame::TryParseJson(const rapidjson::Document& json_document, DoOperation::Parameters& operation_parameters)
{
    if (json_document.HasMember("arg") && json_document["arg"].IsObject())
    {
        auto object = json_document["arg"].GetObject();
        if (object.HasMember("recursive") && object["recursive"].IsBool())
        {
            operation_parameters.recursive_folder_traversal = object["recursive"].GetBool();
        }

        if (object.HasMember("whattocompress") && object["whattocompress"].IsString())
        {
            const char* what_to_compress_id = object["whattocompress"].GetString();

            static const array<tuple<const char*, CompressionOptions::WhatToCompress>, 3> htmlid_to_enum =
            {
                make_tuple("only_uncompressed", CompressionOptions::WhatToCompress::kOnlyUncompressed),
                make_tuple("uncompressed_and_zstd", CompressionOptions::WhatToCompress::kUncompressedAndZstd),
                make_tuple("uncompressed_and_zstd_and_jpgxr", CompressionOptions::WhatToCompress::kUncompressedAndZstdAndJpgxr)
            };

            const auto it = find_if(
                htmlid_to_enum.begin(), 
                htmlid_to_enum.end(), 
                [what_to_compress_id](const auto& i) -> bool {return strcmp(what_to_compress_id, get<0>(i)) == 0; });

            if (it != htmlid_to_enum.end())
            {
                operation_parameters.compression_options.what_to_compress = get<1>(*it);
            }
            //if (what_to_compress_id == "only_uncompressed")
            //{
            //    operation_parameters.compression_options.what_to_compress = CompressionOptions::WhatToCompress::kOnlyUncompressed;
            //}
            //else if (what_to_compress_id == "uncompressed_and_zstd")
            //{
            //    operation_parameters.compression_options.what_to_compress = CompressionOptions::WhatToCompress::kUncompressedAndZstd;
            //}
            //else if (what_to_compress_id == "uncompressed_and_zstd_and_jpgxr")
            //{
            //    operation_parameters.compression_options.what_to_compress = CompressionOptions::WhatToCompress::kUncompressedAndZstdAndJpgxr;
            //}
        }

        if (object.HasMember("compressionlevel") && object["compressionlevel"].IsInt())
        {
            operation_parameters.compression_options.zstd_level = object["compressionlevel"].GetInt();
        }

        return true;
    }

    return false;
}

void WebFrame::ChooseFolderAndSetInWebsite(const std::string& id, const std::string& current_folder)
{
    wxWindowPtr<wxDirDialog> folderBrowserDialog(new wxDirDialog(this, wxEmptyString));
    if (current_folder.empty())
    {
        folderBrowserDialog->SetPath(wxStandardPaths::Get().GetDocumentsDir());
    }
    else
    {
        folderBrowserDialog->SetPath(wxString::FromUTF8(current_folder));
    }

    folderBrowserDialog->Bind(
        wxEVT_WINDOW_MODAL_DIALOG_CLOSED,
        [this, id, folderBrowserDialog](wxWindowModalDialogEvent& event)
        {
            if (event.GetReturnCode() == wxID_OK)
            {
                const wstring folder = folderBrowserDialog->GetPath().ToStdWstring();
                ostringstream ss;
                ss << "document.getElementById(\"" << id << "\").value='";
                ss << EscapeForJavascript(folder) << "';";
                this->web_view_->RunScriptAsync(ss.str());
            }
        });

    folderBrowserDialog->ShowWindowModal();
}

/*static*/std::string WebFrame::EscapeForJavascript(const std::wstring& text)
{
    ostringstream result;
    for (const auto c : text)
    {
        if ((c >= 'a' && c < 'z') || (c >= 'A' && c < 'Z') || (c >= '0' && c < '9'))
        {
            result << static_cast<char>(c);
        }
        else
        {
            // this is using "unicode code point escapes" -> https://exploringjs.com/es6/ch_unicode.html#:~:text=There%20are%20three%20parameterized%20escape%20sequences%20for%20representing,%7B%C2%B7%C2%B7%C2%B7%7D%20%3E%20%27%5Cu%20%7B7A%7D%27%20%3D%3D%3D%20%27z%27%20%20true
            result << "\\u{" << hex << static_cast<uint32_t>(c) << "}";
        }
    }

    return result.str();
}

void WebFrame::ButtonOneClicked(wxCommandEvent& event)
{
    //this->m_browser->RunScript("document.write('Hello from wxWidgets!')");
    //this->m_browser->RunScript("document.getElementById(\"sourcefolderinputtextbox\").setAttribute('value','HELLO WORLD');");
    //this->RunAsyncScript("function f(){return document.getElementById(\"destinationfolderinputtextbox\").value;}f();");

    //string s = this->GetSourceFolderTextFieldText();
    ostringstream ss;
    ss << "document.getElementById(\"logtextbox\").value+=\"";
    ss << EscapeForJavascript(L"ABC\n");
    ss << "\";";
    //static const char* js = "document.getElementById(\"logtextbox\").value+=\"XYZ\n\"";
    this->web_view_->RunScriptAsync(ss.str());
    return;
}

wxBEGIN_EVENT_TABLE(WebFrame, wxFrame)
//EVT_BUTTON(Frame::Ids::ChooseSourceFolderButton, Frame::OnChooseSourceFolderButton)
//EVT_BUTTON(Frame::Ids::ChooseDestinationFolderButton, Frame::OnChooseDestinationFolderButton)
//EVT_BUTTON(Frame::Ids::StartButton, Frame::OnStartButton)
//EVT_BUTTON(Frame::Ids::StopButton, Frame::OnStopButton)
//EVT_COMMAND(Frame::PROGRESS_EVENT_ID, wxEVT_PROGRESS_EVENT, Frame::OnProgressEvent)
wxEND_EVENT_TABLE()

#if 0


class WebFrame : public wxFrame
{
private:
    wxWebView* m_browser;
public:
    WebFrame() : wxFrame(nullptr, wxID_ANY, "wxWidget-WebView Demo")
    {
        wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(topsizer);

        // Check if a fixed version of edge is present in
   // $executable_path/edge_fixed and use it
        wxFileName edgeFixedDir(wxStandardPaths::Get().GetExecutablePath());
        edgeFixedDir.SetFullName("");
        edgeFixedDir.AppendDir("edge_fixed");
        if (edgeFixedDir.DirExists())
        {
            wxWebViewEdge::MSWSetBrowserExecutableDir(edgeFixedDir.GetFullPath());
            wxLogMessage("Using fixed edge version");
        }


        bool b = wxWebView::IsBackendAvailable(wxWebViewBackendEdge);

        /*this->m_browser = wxWebView::New(wxWebViewBackendEdge);
        this->m_browser->Create(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);*/

        this->m_browser = new wxWebViewEdge(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
        //this->m_browser = new wxWebViewIE(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
        topsizer->Add(this->m_browser, wxSizerFlags().Expand().Proportion(1));

        this->m_browser->RegisterHandler(wxSharedPtr<wxWebViewHandler>(new wxWebViewFSHandler("memory")));
        // this->m_browser->LoadURL("memory:page1.htm");
        this->m_browser->SetPage(
            /*"<html><head><title>File System Example</title>"
            "<link rel='stylesheet' type='text/css' href='memory:test.css'>"
            "</head><body><h1>Page 1</h1>"
            "<p><img src='memory:logo.png'></p>"
            "<p><button  onclick=\"window.wx_msg.postMessage('Hello from HTML')\">Click</button>"
            "<p>Some text about <a href='memory:page2.htm'>Page 2</a>.</p></body>",*/
            html_page,
            "");
        // this->m_browser->LoadURL("https://docs.wxpython.org/wx.html2.WebViewHandler.html");

        //this->m_browser->AddScriptMessageHandler("wx_msg");

         //Bind(wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED, &WebFrame::OnScriptMessage, this, m_browser->GetId());

        if (!m_browser->AddScriptMessageHandler("wx_msg"))
        {
            wxLogError("Could not add script message handler");
        }

        this->m_browser->Bind(wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED, [this](wxWebViewEvent& evt) {this->OnWxMsg(evt); });

        wxButton* button = new wxButton(this, wxID_ANY, " ... ", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
        topsizer->Add(button, wxSizerFlags(1).Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL, 1).Proportion(0));
        button->Bind(wxEVT_BUTTON, &WebFrame::ButtonOneClicked, this);

        //Set a more sensible size for web browsing
        SetSize(FromDIP(wxSize(800, 600)));
    }
private:
    static std::string EscapeForJavascript(const std::wstring& text)
    {
        ostringstream result;
        for (auto c : text)
        {
            if ((c >= 'a' && c < 'z') || (c >= 'A' && c < 'Z') || (c >= '0' && c < '9'))
            {
                result << ((char)c);
            }
            else
            {
                result << "\\x" << hex << (uint64_t)c;
            }
        }

        return result.str();
    }

    void ButtonOneClicked(wxCommandEvent& event)
    {
        //this->m_browser->RunScript("document.write('Hello from wxWidgets!')");
        //this->m_browser->RunScript("document.getElementById(\"sourcefolderinputtextbox\").setAttribute('value','HELLO WORLD');");
        //this->RunAsyncScript("function f(){return document.getElementById(\"destinationfolderinputtextbox\").value;}f();");

        string s = this->GetSourceFolderTextFieldText();
        return;
    }

    void OnWxMsg(wxWebViewEvent& evt)
    {
        if (evt.GetString() == "destinationfolderinputtextbox_clicked")
        {
            wxWindowPtr<wxDirDialog> folderBrowserDialog(new wxDirDialog(this, wxEmptyString));
            string currentText = this->GetDestinationFolderTextFieldText();
            /* if (currentText.empty())
             {
                 folderBrowserDialog->SetPath(wxStandardPaths::Get().GetDocumentsDir());
             }
             else
             {
                 folderBrowserDialog->SetPath(currentText);
             }

             folderBrowserDialog->Bind(
                 wxEVT_WINDOW_MODAL_DIALOG_CLOSED,
                 [this, folderBrowserDialog](wxWindowModalDialogEvent& event)
                 {
                     if (event.GetReturnCode() == wxID_OK)
                     {
                         wstring folder = folderBrowserDialog->GetPath().ToStdWstring();
                         ostringstream ss;
                         ss << "document.getElementById(\"destinationfolderinputtextbox\").value='";
                         ss << EscapeForJavascript(folder) << "';";
                         this->m_browser->RunScriptAsync(ss.str());
                     }
                 });

             folderBrowserDialog->ShowWindowModal();*/
        }
        else  if (evt.GetString() == "sourcefolderinputtextbox_clicked")
        {
            wxWindowPtr<wxDirDialog> folderBrowserDialog(new wxDirDialog(this, wxEmptyString));
            folderBrowserDialog->SetPath(wxStandardPaths::Get().GetDocumentsDir());

            folderBrowserDialog->Bind(
                wxEVT_WINDOW_MODAL_DIALOG_CLOSED,
                [this, folderBrowserDialog](wxWindowModalDialogEvent& event)
                {
                    if (event.GetReturnCode() == wxID_OK)
                    {
                        wstring folder = folderBrowserDialog->GetPath().ToStdWstring();
                        ostringstream ss;
                        ss << "document.getElementById(\"sourcefolderinputtextbox\").value='";
                        ss << EscapeForJavascript(folder) << "';";
                        this->m_browser->RunScriptAsync(ss.str());
                    }
                });

            folderBrowserDialog->ShowWindowModal();
        }
        //  wxLogMessage("Script message received; value = %s, handler = %s", evt.GetString(), evt.GetMessageHandler());
    }

    void OnScriptMessage(wxWebViewEvent& evt)
    {
        return;
        //wxUSE_WEBVIEW_EDGE
    }

    int m_asyncScriptResult;
    wxString m_asyncScriptString;

    void RunAsyncScript(const wxString& javascript)
    {
        m_browser->Bind(wxEVT_WEBVIEW_SCRIPT_RESULT, &WebFrame::OnScriptResult, this);
        m_asyncScriptResult = -1;
        m_browser->RunScriptAsync(javascript);
        while (m_asyncScriptResult == -1)
            wxYield();
        m_browser->Unbind(wxEVT_WEBVIEW_SCRIPT_RESULT, &WebFrame::OnScriptResult, this);
    }

    void OnScriptResult(const wxWebViewEvent& evt)
    {
        m_asyncScriptResult = (evt.IsError()) ? 0 : 1;
        m_asyncScriptString = evt.GetString();
    }

    string GetSourceFolderTextFieldText()
    {
        return this->GetTextOfControl("sourcefolderinputtextbox");
    }

    string GetDestinationFolderTextFieldText()
    {
        return this->GetTextOfControl("destinationfolderinputtextbox");
    }

    struct RunScriptAsyncResult
    {
        volatile bool isDone{ false };
        std::string result;
    };

    void GetTextFromControlAndContinueWith(const std::string& ctrlId, std::function<void(std::string&)>& continueWith)
    {

    }


    string GetTextOfControl(const std::string& ctrlId)
    {
        ostringstream ss;
        ss << "function f(){return document.getElementById(\"" << ctrlId << "\").value;}f();";
        m_browser->Bind(wxEVT_WEBVIEW_SCRIPT_RESULT, &WebFrame::OnAsyncRunScriptEvent, this);
        RunScriptAsyncResult* result = new RunScriptAsyncResult();
        this->m_browser->RunScriptAsync(ss.str(), result);


        return "";
        //while (!result->isDone)
        //{
        //    wxYield();
        //}

        //this->m_browser->Unbind(wxEVT_WEBVIEW_SCRIPT_RESULT, &WebFrame::OnAsyncRunScriptEvent, this);

        //string text_of_control = result->result;
        //delete result;
        //return text_of_control;
    }

    void OnAsyncRunScriptEvent(const wxWebViewEvent& evt)
    {
        RunScriptAsyncResult* result = (RunScriptAsyncResult*)evt.GetClientData();

        result->result = evt.GetString().ToUTF8();
        result->isDone = true;
    }

};
#endif

// ----------------------------------------------------------------------------

class WebApp : public wxApp
{
public:
    WebApp()
    {
    }

    virtual bool OnInit() override
    {
        if (!wxApp::OnInit())
            return false;

        //Required for virtual file system archive and memory support
        wxFileSystem::AddHandler(new wxArchiveFSHandler);
        wxFileSystem::AddHandler(new wxMemoryFSHandler);

        // Create the memory files
        wxImage::AddHandler(new wxPNGHandler);
        wxMemoryFSHandler::AddFile("logo.png",
            wxBitmap(wxlogo_xpm), wxBITMAP_TYPE_PNG);
        wxMemoryFSHandler::AddFile("page1.htm",
            "<html><head><title>File System Example</title>"
            "<link rel='stylesheet' type='text/css' href='memory:test.css'>"
            "</head><body><h1>Page 1</h1>"
            "<p><img src='memory:logo.png'></p>"
            "<p><button  onclick=\"window.wx_msg.postMessage('Hello from HTML')\">Click</button>"
            "<p>Some text about <a href='memory:page2.htm'>Page 2</a>.</p></body>");
        wxMemoryFSHandler::AddFile("page2.htm",
            "<html><head><title>File System Example</title>"
            "<link rel='stylesheet' type='text/css' href='memory:test.css'>"
            "</head><body><h1>Page 2</h1>"
            "<p><a href='memory:page1.htm'>Page 1</a> was better.</p></body>");
        wxMemoryFSHandler::AddFile("test.css", "h1 {color: red;}");

        WebFrame* frame = new WebFrame(/*m_url*/);
        frame->Show();
        return true;
    }
};

wxIMPLEMENT_APP(WebApp);