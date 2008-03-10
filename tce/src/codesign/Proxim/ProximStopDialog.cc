/**
 * @file ProximStopDialog.cc
 *
 * Implementation of ProximStopDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/statline.h>
#include <boost/format.hpp>
#include "ProximStopDialog.hh"
#include "WxConversion.hh"
#include "SimulatorFrontend.hh"

const int ProximStopDialog::UPDATE_INTERVAL = 100;

BEGIN_EVENT_TABLE(ProximStopDialog, wxDialog)
    EVT_CLOSE(ProximStopDialog::onClose)
    EVT_BUTTON(ID_STOP, ProximStopDialog::onStop)
    EVT_TIMER(0, ProximStopDialog::onUpdate)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param id Window identifier.
 * @param frontend Simulator frontend to stop.
 */
ProximStopDialog::ProximStopDialog(
    wxWindow* parent, wxWindowID id, SimulatorFrontend& frontend):
    wxDialog(parent, id, _T("Simulation running..."), wxDefaultPosition),
    frontend_(frontend), time_(0) {

    createContents(this, true, true);

    cyclesCtrl_ = dynamic_cast<wxStaticText*>(FindWindow(ID_CYCLES));
    timeCtrl_ = dynamic_cast<wxStaticText*>(FindWindow(ID_TIME));
    cpsCtrl_ = dynamic_cast<wxStaticText*>(FindWindow(ID_CPS));
    cpsCtrl_->SetLabel(_T(""));

    timer_ = new wxTimer(this, 0);
    watch_ = new wxStopWatch();

    cycles_ = frontend_.cycleCount();
    timer_->Start(UPDATE_INTERVAL);
}


/**
 * The Destructor.
 */
ProximStopDialog::~ProximStopDialog() {
    delete timer_;
    delete watch_;
}

/**
 * Event handler for the dialog update timer.
 *
 * Updates the cycle count and simulation time. Also checks if the simulation
 * is still running. If the simulation is no longer running, the dialog is
 * closed automatically.
 */
void
ProximStopDialog::onUpdate(wxTimerEvent&) {

    if (!frontend_.isSimulationRunning()) {
        // simualtion not running anymore
        EndModal(wxID_OK);
    }

    ClockCycleCount cycles = frontend_.cycleCount();
    cyclesCtrl_->SetLabel(WxConversion::toWxString((boost::format("%.0f") %
                                                    cycles).str()));
    long time = watch_->Time();
    unsigned seconds = (time / 1000) % 60;
    unsigned minutes = (time / 1000) / 60;
    wxString timeStr = WxConversion::toWxString(seconds);
    if (seconds < 10) {
        timeStr.Prepend(_T(":0"));
    } else {
        timeStr.Prepend(_T(":"));
    }
    timeStr.Prepend(WxConversion::toWxString(minutes));
    timeCtrl_->SetLabel(timeStr);

    if (time - time_ > 1000) {
        int cps = (1000 * (cycles - cycles_)) / (time - time_);
        wxString cpsStr = WxConversion::toWxString(cps);
        cpsCtrl_->SetLabel(cpsStr);
        time_ = time;
        cycles_ = cycles;
    }
}

/**
 * Event handler for the Stop button.
 *
 * Request simulator stop and closes the dialog.
 */
void
ProximStopDialog::onStop(wxCommandEvent&) {
    frontend_.prepareToStop(SRE_USER_REQUESTED);
    EndModal(wxID_OK);
}

/**
 * Event handler for the windowmanager dialog close button.
 *
 * Requests simulator stop and closes the dialog.
 */
void
ProximStopDialog::onClose(wxCloseEvent&) {
    frontend_.prepareToStop(SRE_USER_REQUESTED);
    EndModal(wxID_OK);
}


/**
 * Creates the dialog widgets.
 *
 * Code generated by wxDesigner. Do not modify manually.
 *
 * @param parent Parent window of the dialog.
 * @param call_fit If true, the parent window is resized to fit the widgets.
 * @param set_sizer If true, Sets the created widgets as parent window
 *                  contents.
 */
wxSizer*
ProximStopDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    item0->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item1 = new wxStaticText( parent, ID_TEXT, wxT("Simulation running..."), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxFlexGridSizer *item2 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *item3 = new wxStaticText( parent, ID_LABEL_CYCLES, wxT("Cycles:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item4 = new wxStaticText( parent, ID_CYCLES, wxT("???????????????????"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
    item2->Add( item4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item5 = new wxStaticText( parent, ID_LABEL_TIME, wxT("Time:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item5, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item6 = new wxStaticText( parent, ID_TIME, wxT("???????????????????"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
    item2->Add( item6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item7 = new wxStaticText( parent, ID_LABEL_CPS, wxT("Avg. cycles/sec:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item7, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item8 = new wxStaticText( parent, ID_CPS, wxT("???????????????????"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
    item2->Add( item8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item9 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(300,-1), wxLI_HORIZONTAL );
    item0->Add( item9, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item10 = new wxButton( parent, ID_STOP, wxT("&Stop"), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
