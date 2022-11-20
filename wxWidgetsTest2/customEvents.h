#pragma once

#include <string>
#include <wx/event.h>
#include "DoOperation.h"

// c.f. https://wiki.wxwidgets.org/Custom_Events for documentation about "custom events"

class BrowseForFolderEvent;
wxDECLARE_EVENT(BROWSEFORFOLDER_EVENT_TYPE, BrowseForFolderEvent);

class BrowseForFolderEvent : public wxCommandEvent
{
public:
  BrowseForFolderEvent(wxEventType commandType = BROWSEFORFOLDER_EVENT_TYPE, int id = 0)
    : wxCommandEvent(commandType, id)
  {
  }

  // You *must* copy here the data to be transported
  BrowseForFolderEvent(const BrowseForFolderEvent& event)
    : wxCommandEvent(event)
  {
    this->SetHtmlElementId(event.GetHtmlElementId());
    this->SetCurrentFolder(event.GetCurrentFolder());
  }

  // Required for sending with wxPostEvent()
  BrowseForFolderEvent* Clone() const { return new BrowseForFolderEvent(*this); }

  const std::string& GetHtmlElementId() const { return this->id_; }
  void SetHtmlElementId(const std::string& id) { this->id_ = id; }
  const std::string& GetCurrentFolder() const { return this->current_folder_; }
  void SetCurrentFolder(const std::string& current_folder) { this->current_folder_ = current_folder; }

private:
  std::string id_;
  std::string current_folder_;
};

typedef void (wxEvtHandler::* BrowseForFolderEventFunction)(BrowseForFolderEvent&);

#define BrowseForFolderEventHandler(func) wxEVENT_HANDLER_CAST(BrowseForFolderEventFunction, func)                    

// Optional: define an event table entry
#define EVT_BROWSEFORFOLDER(id, func) \
 	wx__DECLARE_EVT1(BROWSEFORFOLDER_EVENT_TYPE, id, BrowseForFolderEventHandler(func))

//--------------------------------------------------------------------------------------------------------

class OperationProgressInfoEvent;
wxDECLARE_EVENT(OPERATIONPROGRESSINFO_EVENT_TYPE, OperationProgressInfoEvent);

class OperationProgressInfoEvent : public wxCommandEvent
{
public:
  OperationProgressInfoEvent(wxEventType commandType = OPERATIONPROGRESSINFO_EVENT_TYPE, int id = 0)
    : wxCommandEvent(commandType, id)
  {
  }

  // You *must* copy here the data to be transported
  OperationProgressInfoEvent(const OperationProgressInfoEvent& event)
    : wxCommandEvent(event)
  {
    this->SetProgressInformation(this->GetProgressInformation());
  }

  // Required for sending with wxPostEvent()
  OperationProgressInfoEvent* Clone() const { return new OperationProgressInfoEvent(*this); }

  const DoOperation::ProgressInformation& GetProgressInformation() const { return this->progress_information_; }
  void SetProgressInformation(const DoOperation::ProgressInformation& progress_information) { this->progress_information_ = progress_information; }
private:
  DoOperation::ProgressInformation progress_information_;
};

typedef void (wxEvtHandler::* OperationProgressInfoEventFunction)(OperationProgressInfoEvent&);

#define OperationProgressInfoEventHandler(func) wxEVENT_HANDLER_CAST(OperationProgressInfoEventFunction, func)                    

// Optional: define an event table entry
#define EVT_OPERATIONPROGRESSINFO(id, func) \
 	wx__DECLARE_EVT1(OPERATIONPROGRESSINFO_EVENT_TYPE, id, OperationProgressInfoEventHandler(func))
