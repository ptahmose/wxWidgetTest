#include <wx/wx.h>
#include <wx/filesys.h>
#include <wx/fs_arc.h>
#include <wx/fs_mem.h>
#include <wx/webview.h>
#include <wx/webviewfshandler.h>
#include <wx/windowptr.h>
#include "wx/stdpaths.h"

#include <sstream>
#include <tuple>
#include <array>
#include <algorithm>
#include "webView.h"

#include "htmlpage.h"
#include "utilities.h"
#include "wxlogo.xpm"

using namespace std;

WebFrame::WebFrame() : wxFrame(nullptr, wxID_ANY, "wxWidget-WebView Demo")
{
    wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(topsizer);
    if (wxWebView::IsBackendAvailable(wxWebViewBackendEdge))
    {
        this->web_view_ = wxWebView::New(/*wxWebViewBackendIE*/ wxWebViewBackendEdge);
    }
    else if (wxWebView::IsBackendAvailable(wxWebViewBackendWebKit))
    {
        this->web_view_ = wxWebView::New(wxWebViewBackendWebKit);
    }
    else
    {
        // TODO:
    }

    this->web_view_->Create(this, Ids::WebView, "", wxDefaultPosition, wxDefaultSize);
    //this->web_view_ = new wxWebViewEdge(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
    topsizer->Add(this->web_view_, wxSizerFlags().Expand().Proportion(1));
    //this->web_view_->LoadURL("file:///D:/dev/MyGitHub/wxWidgetTest/wxWidgetsTest2/page.html");

    wxString str = GetHtmlPage();
    this->web_view_->SetPage(str, "");

    // this will allow to call from Javascript into this class (c.f. https://docs.wxwidgets.org/trunk/classwx_web_view.html#a2597c3371ed654bf03262ec6d34a0126)
    if (!this->web_view_->AddScriptMessageHandler("wx_msg"))
    {
        wxLogError("Could not add script message handler");
    }

    this->Bind(wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED, &WebFrame::OnScriptWxMsg, this, this->web_view_->GetId());

    /*
    // add a button below the web-control (for testing)
    wxButton* button = new wxButton(this, wxID_ANY, "Test", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    topsizer->Add(button, wxSizerFlags(1).Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL, 1).Proportion(0));
    button->Bind(wxEVT_BUTTON, &WebFrame::ButtonOneClicked, this);
    */
}

void WebFrame::OnScriptWxMsg(wxWebViewEvent& evt)
{
    string message = string{ evt.GetString().ToUTF8() };
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

        if (id == "sourcefolderinputtextbox" || id == "destinationfolderinputtextbox")
        {
            // the "browse-button" of the "source-folder-selection" has been clicked
            BrowseForFolderEvent event(BROWSEFORFOLDER_EVENT_TYPE, BROWSEFORFOLDER_EVENT_ID);
            event.SetHtmlElementId(id);
            event.SetCurrentFolder(arg);

            // Note: Maybe it is not really required, but here we just put an event (or: a custom event) into the
            //        event-queue. It works without this detour, but there are ominous warnings being emitted 
            //        (by WebView2 on Windows), warning about re-entrancy issues. The browse-for-folder-dialog of
            //        course is running its own message-pump, maybe this is asking for trouble. So - we try to play
            //        nice and go with the message-queue.
            wxPostEvent(this, event);
        }
        else if (id == "startbutton")
        {
            DoOperation::Parameters operation_parameters;
            const bool parsed_ok = TryParseJson(document, operation_parameters);
            operation_parameters.report_progress_functor = [this](const DoOperation::ProgressInformation& information)->void {this->ProgressEvent(information); };
            this->operation_.Start(operation_parameters);
        }
        else if (id == "stopbutton")
        {
            this->operation_.RequestCancel();
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
            static const array<tuple<const char*, CompressionOptions::WhatToCompress>, 3> htmlid_to_enum =
            {
                make_tuple("only_uncompressed", CompressionOptions::WhatToCompress::kOnlyUncompressed),
                make_tuple("uncompressed_and_zstd", CompressionOptions::WhatToCompress::kUncompressedAndZstd),
                make_tuple("uncompressed_and_zstd_and_jpgxr", CompressionOptions::WhatToCompress::kUncompressedAndZstdAndJpgxr)
            };

            const char* what_to_compress_id = object["whattocompress"].GetString();
            const auto it = find_if(
                htmlid_to_enum.begin(),
                htmlid_to_enum.end(),
                [what_to_compress_id](const auto& i) -> bool {return strcmp(what_to_compress_id, get<0>(i)) == 0; });
            if (it != htmlid_to_enum.end())
            {
                operation_parameters.compression_options.what_to_compress = get<1>(*it);
            }
        }

        if (object.HasMember("compressionlevel") && object["compressionlevel"].IsInt())
        {
            operation_parameters.compression_options.zstd_level = object["compressionlevel"].GetInt();
        }

        if (object.HasMember("sourcefolder") && object["sourcefolder"].IsString())
        {
            operation_parameters.source_folder = object["sourcefolder"].GetString();
        }

        if (object.HasMember("destinationfolder") && object["destinationfolder"].IsString())
        {
            operation_parameters.destination_folder = object["destinationfolder"].GetString();
        }

        return true;
    }

    return false;
}

void WebFrame::ProgressEvent(const DoOperation::ProgressInformation& information)
{
    // Note that this method is called from an arbitrary thread context
    auto event = new OperationProgressInfoEvent(OPERATIONPROGRESSINFO_EVENT_TYPE, PROGRESS_EVENT_ID);
    event->SetProgressInformation(information);

    // The event-queue will take ownership of the event-object. Note that there is no pre-caution for dealing
    //  with an event-flood - so we rely on that the rate this event is raised is reasonable.
    wxQueueEvent(this, event);
}

void WebFrame::OnProgressInfoEvent(OperationProgressInfoEvent& event)
{
    ostringstream javascript_command;   // in this stringstream we construct the code to execute "in" the website

    const auto& progress_info = event.GetProgressInformation();
    if (progress_info.message_valid)
    {
        javascript_command << "add_to_log(" << progress_info.remove_characters_before_adding_message << ",\"" << EscapeForJavascript(convertUtf8ToWide(progress_info.message)) << "\");";
    }

    if (progress_info.no_of_files_processed_valid ||
        progress_info.data_size_of_files_processed_before_compression_valid ||
        progress_info.data_size_of_files_processed_after_compression_valid)
    {
        ostringstream statistics_object;
        statistics_object << "{";
        if (progress_info.no_of_files_processed_valid)
        {
            statistics_object << "'files_processed':" << progress_info.no_of_files_processed << ",";
        }

        if (progress_info.data_size_of_files_processed_before_compression_valid)
        {
            statistics_object << "'data_size_of_files_processed_before_compression':" << progress_info.data_size_of_files_before_compression_processed << ",";
        }

        if (progress_info.data_size_of_files_processed_after_compression_valid)
        {
            statistics_object << "'data_size_of_files_processed_after_compression':" << progress_info.data_size_of_files_processed_after_compression << ",";
        }

        statistics_object << "}";

        javascript_command << "set_statistics(" << statistics_object.str() << ");";
    }

    if (javascript_command.tellp() > 0) // test if "some commands" have been added, and only if so, execute the javascript-code
    {
        javascript_command << "set_operational_state(" << (progress_info.operation_ongoing ? "true" : "false") << ");";
        this->web_view_->RunScriptAsync(javascript_command.str());
    }
}

void WebFrame::OnBrowseForFolderEvent(BrowseForFolderEvent& event)
{
    this->ChooseFolderAndSetInWebsite(event.GetHtmlElementId(), event.GetCurrentFolder());
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
    // The idea is to convert a "wide-string" into a string which is "safe" to be used as
    //  a string-constant in Javascript. What we do is very simple - only "plain characters"
    //  are copied verbatim, everything else is converted to a "unicode code point".
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
EVT_BROWSEFORFOLDER(WebFrame::BROWSEFORFOLDER_EVENT_ID, WebFrame::OnBrowseForFolderEvent)
EVT_OPERATIONPROGRESSINFO(WebFrame::PROGRESS_EVENT_ID, WebFrame::OnProgressInfoEvent)
wxEND_EVENT_TABLE()

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
        {
            return false;
        }

        /*
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
        */
        WebFrame* frame = new WebFrame();
        frame->Show();
        return true;
    }
};

wxIMPLEMENT_APP(WebApp);