/**
 * @file ProximExecuteFileCmd.cc
 *
 * Implementation of ProximExecuteFileCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <fstream>
#include "ProximExecuteFileCmd.hh"
#include "ProximConstants.hh"
#include "ProximToolbox.hh"
#include "ProximLineReader.hh"
#include "WxConversion.hh"
#include "ErrorDialog.hh"
#include "TracedSimulatorFrontend.hh"

/**
 * The Constructor.
 */
ProximExecuteFileCmd::ProximExecuteFileCmd():
    GUICommand(ProximConstants::COMMAND_NAME_EXECUTE_FILE, NULL) {

    simulator_ = ProximToolbox::frontend();
}

/**
 * The Destructor.
 */
ProximExecuteFileCmd::~ProximExecuteFileCmd() {
}


/**
 * Executes the command.
 */
bool
ProximExecuteFileCmd::Do() {
    wxString message = _T("Choose a file.");
    wxString wildcard = _T("Command history logs (*.log)|*.log|All files|*.*");
    wxFileDialog dialog(
        parentWindow(), message, _T(""), _T(""), wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_CANCEL) {
        return false;
    }

    std::string filename = WxConversion::toString(dialog.GetPath());
    std::ifstream file(filename.c_str());

    if (file.bad()) {
        wxString message = _T("Error opening file '");
        message.Append(dialog.GetPath());
        message.Append(_T("'."));
        ErrorDialog dialog(parentWindow(), message);
        dialog.ShowModal();
        return false;
    }

    ProximLineReader& lineReader = ProximToolbox::lineReader();

    while (!file.eof()) {
        std::string command;
        getline(file, command);
        lineReader.input(command);
    }

    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximExecuteFileCmd::icon() const {
    return "exec.png";
}


/**
 * Returns ID of this command.
 */
int
ProximExecuteFileCmd::id() const {
    return ProximConstants::COMMAND_EXECUTE_FILE;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximExecuteFileCmd*
ProximExecuteFileCmd::create() const {
    return new ProximExecuteFileCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return True if the simulation is initialized or stopped.
 */
bool
ProximExecuteFileCmd::isEnabled() {

    if (simulator_ != NULL) {
	return true;
    } else {
	return false;
    }
}
