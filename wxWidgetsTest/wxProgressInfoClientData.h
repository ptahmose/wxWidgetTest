#pragma once

#include <memory>
#include <wx/event.h>
#include "DoOperation.h"

// Declare a custom event. C.f https://wiki.wxwidgets.org/Inter-Thread_and_Inter-Process_communication#Sending_events_to_the_main_thread_-_wxWidgets_3_only
DECLARE_LOCAL_EVENT_TYPE(wxEVT_PROGRESS_EVENT, -1)

/// This custom event type serves the purpose of "transporting" the DoOperation::ProgressInformation - structure.
class wxProgressInfoClientData : public wxClientData
{
private:
    std::unique_ptr<DoOperation::ProgressInformation> progress_information_;
public:
    wxProgressInfoClientData() = delete;

    /// Constructor which makes a copy of the specified progress-information-structure.
    ///
    /// \param  progress_information    The progress-information.
    wxProgressInfoClientData(const DoOperation::ProgressInformation& progress_information)
        : progress_information_{std::make_unique<DoOperation::ProgressInformation>(progress_information)}
    {}

    /// Gets the progress information (which is being "transported" by this custom event).
    ///
    /// \returns    The progress information.
    [[nodiscard]] const DoOperation::ProgressInformation& GetProgressInformation() const { return *this->progress_information_; }
};
