/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file AddSocketCmd.cc
 *
 * Definition of AddSocketCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#include <wx/wx.h>
#include <wx/docview.h>

#include "AddSocketCmd.hh"
#include "Socket.hh"
#include "SocketDialog.hh"
#include "Model.hh"
#include "ModelConstants.hh"
#include "MDFDocument.hh"
#include "ProDeConstants.hh"
#include "ErrorDialog.hh"
#include "ProDe.hh"
#include "Machine.hh"
#include "ObjectState.hh"

using std::string;
using namespace TTAMachine;

/**
 * The Constructor.
 */
AddSocketCmd::AddSocketCmd() :
    EditorCommand(ProDeConstants::CMD_NAME_ADD_SOCKET) {
}



/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
AddSocketCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);

    Model* model = dynamic_cast<MDFDocument*>(
        view()->GetDocument())->getModel();

    model->pushToStack();

    // Generate name for the new socket.
    Machine::SocketNavigator navigator =
        model->getMachine()->socketNavigator();
    int i = 1;
    string newName = ProDeConstants::COMP_NEW_NAME_PREFIX_SOCKET +
        Conversion::toString(i);
    while (navigator.hasItem(newName)) {
        newName = ProDeConstants::COMP_NEW_NAME_PREFIX_SOCKET +
            Conversion::toString(i);
        i++;
    }

    Socket* socket = new Socket(newName);
    socket->setMachine(*(model->getMachine()));

    SocketDialog dialog(parentWindow(), socket);

    if (dialog.ShowModal() == wxID_OK) {
	model->notifyObservers();
	return true;
    } else {
	model->popFromStack();
	return false;
    }
    return false;
}


/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
AddSocketCmd::id() const {
    return ProDeConstants::COMMAND_ADD_SOCKET;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
AddSocketCmd*
AddSocketCmd::create() const {
    return new AddSocketCmd();
}


/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
AddSocketCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_ADD_SOCKET;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
AddSocketCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
