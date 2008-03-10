/**
 * @file ProximControlWindow.cc
 *
 * Definition of ProximControlWindow class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */


#include <string>
#include <wx/statline.h>
#include <wx/listctrl.h>

#include "ProximControlWindow.hh"
#include "Proxim.hh"
#include "WxConversion.hh"
#include "ProximConstants.hh"


/**
 * Constructor.
 *
 * @param parent Parent window of the window.
 * @param id Window identifier.
 */
ProximControlWindow::ProximControlWindow(
    ProximMainFrame* parent, int id):
    ProximSimulatorWindow(parent, id) {

    createContents(this, true, true);
}


/**
 * Destructor.
 */
ProximControlWindow::~ProximControlWindow() {
}


/**
 * Called when the simulator program, memory and machine models are reset.
 */
void
ProximControlWindow::reset() {
    // Do nothing.
}

/**
 * Creates the window contents.
 *
 * Code generated by wxDesigner.
 */
wxSizer*
ProximControlWindow::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxButton *item1 = new wxButton( parent, ProximConstants::COMMAND_RUN, wxT("Run"), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxGridSizer *item2 = new wxGridSizer( 2, 0, 0 );

    wxButton *item3 = new wxButton( parent, ProximConstants::COMMAND_STEPI, wxT("Stepi"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item4 = new wxButton( parent, ProximConstants::COMMAND_NEXTI, wxT("Nexti"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item2, 0, wxALIGN_CENTER, 5 );

    wxButton *item5 = new wxButton( parent, ProximConstants::COMMAND_KILL, wxT("Kill"), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item6 = new wxButton( parent, ProximConstants::COMMAND_RESUME, wxT("Resume"), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
