/**
 * @file OpenDocumentCmd.cc
 *
 * Definition of OpenDocumentCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel@cs.tut.fi)
 */

#include <wx/docmdi.h>
#include <wx/msgdlg.h>

#include "OpenDocumentCmd.hh"
#include "ProDeConstants.hh"
#include "ProDe.hh"

using std::string;

/**
 * The Constructor.
 */
OpenDocumentCmd::OpenDocumentCmd():
    EditorCommand(ProDeConstants::CMD_NAME_OPEN_DOC) {

}


/**
 * The Destructor.
 */
OpenDocumentCmd::~OpenDocumentCmd() {}


/**
 * Executes the command.
 *
 * @return Always false. Returning true here would cause problems with the
 *         wxCommandProcessor, and the command is never undoable so it's
 *         safe to return false even if the command was succesfully executed.
 */
bool
OpenDocumentCmd::Do() {
            
    wxDocManager* docManager = wxGetApp().docManager();
    
    // Disable multiple tabs if we're not using wxWidgets 2.8 or above   
    if (wxCHECK_VERSION(2, 8, 0)) {        
        docManager->CreateDocument(_T(""), 0);        
        return false;
    }
    else {
        // close the active document if there is any
        wxDocument *doc = docManager->GetCurrentDocument();
        if (doc) {
            if (doc->Close()) {
                doc->DeleteAllViews();
            }
            else { // cancel was pressed -> cancel the command
                return false;
            }
        }
    }

    // Open the document
    docManager->CreateDocument(_T(""), 0);        
    return false;
}


/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
OpenDocumentCmd::id() const {
    return ProDeConstants::COMMAND_OPEN_DOC;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
OpenDocumentCmd*
OpenDocumentCmd::create() const {
    return new OpenDocumentCmd();
}


/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
OpenDocumentCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_OPEN_DOC;
}

/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
OpenDocumentCmd::icon() const {
    return ProDeConstants::CMD_ICON_OPEN_DOC;
}


/**
 * This command is always executable.
 *
 * @return Always true.
 */
bool
OpenDocumentCmd::isEnabled() {
    return true;
}
