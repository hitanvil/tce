/**
 * @file ProximDisassemblyWindow.cc
 *
 * Declaration of ProximDisassemblyWindow class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */


#ifndef TTA_PROXIM_DISASSEMBLY_WINDOW_HH
#define TTA_PROXIM_DISASSEMBLY_WINDOW_HH

#include <map>
#include <wx/wx.h>
#include <wx/grid.h>
#include "SimulatorEvent.hh"
#include "ProximSimulatorWindow.hh"
#include "Program.hh"

class ProximMainFrame;
class ProximDisassemblyGridTable;
class ProximDisasmAttrProvider;

/**
 * A window which can be utilized to display disassembly of a program object
 * model.
 *
 * This class listens to SimulatorEvents and updates the window information
 * automatically.
 */
class ProximDisassemblyWindow : public ProximSimulatorWindow {
public:
    ProximDisassemblyWindow(ProximMainFrame* parent, int id);
    virtual ~ProximDisassemblyWindow();
    virtual void reset();
    void loadProgram(const TTAProgram::Program& program);
    void showAddress(unsigned address);
    void setMoveAttrProvider(ProximDisasmAttrProvider* attrProvider);

private:
    void initialize();
    void resetGrid();
    void onProgramLoaded(const SimulatorEvent& event);
    void onSimulatorStop(const SimulatorEvent& event);
    void onSimulatorCommand(SimulatorEvent& event);
    void onRightClick(wxGridEvent& event);
    void onSetBreakpoint(wxCommandEvent& event);
    void onSetTempBp(wxCommandEvent& event);
    void onRunUntil(wxCommandEvent& event);
    void onMappedMenuCommand(wxCommandEvent& event);

    /// The grid disapleying the disassembly.
    wxGrid* codeGrid_;
    /// Grid table which handles on-the-fly disassembly of the loaded program.
    ProximDisassemblyGridTable* codeTable_;
    
    /// A map which contains command strings for context menu items.
    std::map<unsigned, std::string> menuCommand_;

    /// Width of the column displaying the arrow pointign current instruction.
    static const unsigned INFO_COLUMN_WIDTH;
    /// Width of the instruction address column.
    static const unsigned ADDRESS_COLUMN_WIDTH;
    /// Width of the breakpoint info column.
    static const unsigned BP_COLUMN_WIDTH;
    /// Width of the instruction disassembly column.
    static const unsigned DISASSEMBLY_COLUMN_WIDTH;

    /// Context menu command IDs.
    enum {
	MENU_ID_SET_BP = 10000,
        MENU_ID_SET_TEMP_BP,
        MENU_ID_RUN_UNTIL,
	MENU_ID_FIRST_MAPPED // Keep this as the last menu item.
    };

    DECLARE_EVENT_TABLE()
};

#endif
