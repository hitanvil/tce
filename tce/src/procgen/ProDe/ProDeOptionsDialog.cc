/**
 * @file ProDeOptionsDialog.cc
 *
 * Implementation of ProDeOptionsDialog class.
 *
 * @author Veli-Pekka Jääskeläinen (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <wx/notebook.h>
#include "ProDeOptionsDialog.hh"
#include "ProDeOptions.hh"
#include "WxConversion.hh"
#include "ProDe.hh"
#include "Conversion.hh"
#include "UserManualCmd.hh"
#include "WidgetTools.hh"
#include "GUITextGenerator.hh"
#include "ProDeTextGenerator.hh"
#include "ProDeConstants.hh"

using boost::format;
using std::string;
using std::vector;


BEGIN_EVENT_TABLE(ProDeOptionsDialog, OptionsDialog)
    EVT_BUTTON(wxID_OK, ProDeOptionsDialog::onOK)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param options Options to be modified.
 */
ProDeOptionsDialog::ProDeOptionsDialog(
    wxWindow* parent, ProDeOptions& options, CommandRegistry& registry):
    OptionsDialog(parent, options, registry),
    parent_(parent), options_(options) {

    wxPanel* generalPage = new wxPanel(notebook_, -1);
    createGeneralPage(generalPage, true, true);
    addPage(generalPage, _T("General"));

    undoStackSize_ = dynamic_cast<wxSpinCtrl*>(FindWindow(ID_UNDO_LEVELS));

    readProDeOptions();

}


/**
 * The Destructor.
 */
ProDeOptionsDialog::~ProDeOptionsDialog() {
}

/**
 * Reads the options in options_ to dialog attributes.
 */
void
ProDeOptionsDialog::readProDeOptions() {
    undoStackSize_->SetValue(options_.undoStackSize());
}


/**
 * Writes the options from dialog attributes to the current options object.
 */
void
ProDeOptionsDialog::writeProDeOptions() {
    options_.setUndoStackSize(undoStackSize_->GetValue());
}


/**
  * Validates input in the controls, and updates the options.
  */
void
ProDeOptionsDialog::onOK(wxCommandEvent&) {

    if (!Validate()) {
        return;
    }

    if (!TransferDataFromWindow()) {
        return;
    }

    writeOptions();
    writeProDeOptions();

    EndModal(wxID_OK);
}


/**
 * Creates the 'General' page for the dialog.
 *
 * This function was initially generated by wxDesigner.
 * @return Main sizer of the created contents.
 * @param parent The dialog window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 */
wxSizer*
ProDeOptionsDialog::createGeneralPage(
    wxWindow *parent, bool call_fit, bool set_sizer ) {

    wxBoxSizer *item0 = new wxBoxSizer( wxHORIZONTAL );

    item0->Add( 50, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item1 = new wxStaticText( parent, ID_LABEL_UNDO_LEVELS, wxT("Undo levels:"), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxSpinCtrl *item2 = new wxSpinCtrl( parent, ID_UNDO_LEVELS, wxT("1"), wxDefaultPosition, wxSize(100,-1), 0, 1, 100, 0 );
    item0->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }

    return item0;
}
