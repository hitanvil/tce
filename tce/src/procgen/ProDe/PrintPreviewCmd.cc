/**
 * @file PrintPreviewCmd.cc
 *
 * Definition of PrintPreviewCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel@cs.tut.fi)
 */

#include <wx/docview.h>

#include "config.h"
#include "PrintPreviewCmd.hh"
#include "ProDeConstants.hh"
#include "ProDe.hh"
#include "ErrorDialog.hh"

using std::string;


/**
 * The Constructor.
 */
PrintPreviewCmd::PrintPreviewCmd():
    EditorCommand(ProDeConstants::CMD_NAME_PRINT_PREVIEW) {

}


/**
 * The Destructor.
 */
PrintPreviewCmd::~PrintPreviewCmd() {
}


/**
 * Executes the command.
 *
 * If the wxWidgets printing framework was not compiled in the wxWidgets
 * library, print previeving is not possible. An error message will be
 * displayed instead of previewing.
 *
 * @return Always false. The command is not undoable.
 */
bool
PrintPreviewCmd::Do() {

#if WX_PRINTING_DISABLED

    // wxWidgets was compiled without printing framework, display an
    // error message.

    wxString message =
        wxString(_T("Printing framework is disabled in the wxWidgets\n"));

    message.Append(_T("library. Print previewing is not possible."));

    ErrorDialog error(parentWindow(), message);
    error.ShowModal();
    return false;

#else

    // wxWidgets was compiled with the printing framework.
    wxCommandEvent dummy;
    wxGetApp().docManager()->OnPreview(dummy);
    return false;

#endif // WX_PRINTING_ENABLED

}


/**
 * Return id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
PrintPreviewCmd::id() const {
    return ProDeConstants::COMMAND_PRINT_PREVIEW;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
PrintPreviewCmd*
PrintPreviewCmd::create() const {
    return new PrintPreviewCmd();
}


/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
PrintPreviewCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_PRINT_PREVIEW;
}


/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
PrintPreviewCmd::icon() const {
    return ProDeConstants::CMD_ICON_PRINT_PREVIEW;
}


/**
 * Returns true when command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if the command is executable.
 */
bool
PrintPreviewCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
        return true;
    }
    return false;
}
