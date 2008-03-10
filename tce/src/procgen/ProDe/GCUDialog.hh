/**
 * @file GCUDialog.hh
 *
 * Declaration of GCUDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_GCU_DIALOG_HH
#define TTA_GCU_DIALOG_HH

#include <wx/wx.h>
#include <wx/listctrl.h>

namespace TTAMachine {
    class ControlUnit;
    class HWOperation;
    class BaseFUPort;
}

/**
 * Dialog for adding a new Global Control Unit to the Machine and
 * editing existing Global Control Unit attributes.
 */
class GCUDialog : public wxDialog {
 public:
    GCUDialog(wxWindow* parent, TTAMachine::ControlUnit* gcu);
    virtual ~GCUDialog();

 private:
    virtual bool TransferDataToWindow();
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void updateAddressSpaceChoice();
    void updateSocketChoicers();
    void updateOperationList();
    void updatePortList();
    void updateRAPortChoice();
    TTAMachine::HWOperation* selectedOperation();
    TTAMachine::BaseFUPort* selectedPort();
    void onName(wxCommandEvent& event);

    void onOperationSelection(wxListEvent& event);
    void onAddOperation(wxCommandEvent& event);
    void onEditOperation(wxCommandEvent& event);
    void onDeleteOperation(wxCommandEvent& event);
    void onActivateOperation(wxListEvent& event);
    void onOperationRightClick(wxListEvent& event);

    void onAddFUPort(wxCommandEvent& event);
    void onAddSRPort(wxCommandEvent& event);
    void onEditPort(wxCommandEvent& event);
    void onDeletePort(wxCommandEvent& event);
    void onPortSelection(wxListEvent& event);
    void onActivatePort(wxListEvent& event);
    void onPortRightClick(wxListEvent& event);

    void onRAPortChoice(wxCommandEvent& event);

    void onHelp(wxCommandEvent& event);
    void onOK(wxCommandEvent& event);
    void setLabels();

    /// The Global Control Unit to be modified with the dialog.
    TTAMachine::ControlUnit* gcu_;
    /// Name of the global control unit.
    wxString name_;
    /// Number of delay slots of the global control unit.
    int delaySlots_;
    /// Guard latency of the gcu.
    int guardLatency_;

    /// Address Space choice control.
    wxChoice* addressSpaceChoice_;
    /// Return address port choice  control.
    wxChoice* raPortChoice_;

    /// Operation list widget.
    wxListCtrl* operationList_;
    /// Port list widget.
    wxListCtrl* portList_;

    /// Box sizer for the operations list controls.
    wxStaticBoxSizer* operationsSizer_;
    /// Box sizer for the port list controls.
    wxStaticBoxSizer* portListSizer_;

    /// enumerated IDs for dialog controls
    enum {
        ID_NAME = 10000,
        ID_TEXT,
        ID_DELAY_SLOTS,
        //ID_GUARD_LATENCY,
        ID_ADDRESS_SPACE,
        ID_OPERATION_LIST,
        ID_PORT_LIST,
	ID_RA_CHOICE,
	ID_OP_CHOICE,
        ID_LINE,
        ID_ADD_OPERATION,
        ID_EDIT_OPERATION,
        ID_DELETE_OPERATION,
        ID_ADD_FU_PORT,
        ID_ADD_SR_PORT,
        ID_EDIT_PORT,
        ID_DELETE_PORT,
        ID_HELP,
        ID_TEXT_NAME,
        ID_TEXT_DS,
        ID_TEXT_AS,
        ID_TEXT_INPUT_SOCKET,
        ID_TEXT_OUTPUT_SOCKET,
        ID_TEXT_RA_WIDTH,
	ID_LABEL_RA_CHOICE,
	ID_LABEL_OP_CHOICE
        //ID_LABEL_GLOBAL_GUARD_LATENCY
    };
    DECLARE_EVENT_TABLE()
};

#endif
