/**
 * @file OSEdInfoView.cc
 *
 * Definition of OSEdInfoView class.
 *
 * @author Jussi Nyk�nen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include <vector>
#include <boost/format.hpp>
#include <set>

#include "OSEdInfoView.hh"
#include "WxConversion.hh"
#include "OperationContainer.hh"
#include "OSEdConstants.hh"
#include "OperationModule.hh"
#include "Application.hh"
#include "OSEdTextGenerator.hh"
#include "Environment.hh"
#include "WidgetTools.hh"
#include "DropDownMenu.hh"
#include "OSEd.hh"
#include "Operation.hh"
#include "OperationIndex.hh"

using std::string;
using std::vector;
using std::set;
using boost::format;

BEGIN_EVENT_TABLE(OSEdInfoView, wxListCtrl)
    EVT_LIST_ITEM_SELECTED(OSEdConstants::CMD_INFO_VIEW, OSEdInfoView::onSelection)
    EVT_RIGHT_DOWN(OSEdInfoView::onDropDownMenu)
END_EVENT_TABLE()

/**
 * Constructor.
 *
 * @param parent Parent window.
 */
OSEdInfoView::OSEdInfoView(wxWindow* parent) :
    wxListCtrl(
        parent, OSEdConstants::CMD_INFO_VIEW, wxDefaultPosition,
        wxSize(400,400),
        wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER), mode_(MODE_NOMODE) {
}


/**
 * Destructor.
 */
OSEdInfoView::~OSEdInfoView() {
}

/**
 * Clears the info screen.
 */
void
OSEdInfoView::clear() {
    int columns = GetColumnCount();
    for (int i = 0; i < columns; i++) {
        DeleteColumn(0);
    }
    DeleteAllItems();
}

/**
 * Shows the search path view.
 */
void
OSEdInfoView::pathView() {
    
    clear();
    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    format fmt = texts.text(OSEdTextGenerator::TXT_COLUMN_SEARCH_PATHS);
    InsertColumn(
        0, WxConversion::toWxString(fmt.str()), wxLIST_FORMAT_LEFT, 
        OSEdConstants::DEFAULT_COLUMN_WIDTH * 2);
    
    vector<string> paths = Environment::osalPaths();
    for (size_t i = 0; i < paths.size(); i++) {
        wxListItem item;
        if (FileSystem::fileExists(paths[i])) {
            // path exists, let's write it bold
            item.SetFont(*OSEdConstants::BOLD);
        }
        item.SetId(i);
        int index = InsertItem(item);
        SetItemText(index, WxConversion::toWxString(paths[i]));
    } 
    
    mode_ = MODE_PATH;
}

/**
 * Shows the module view.
 *
 * @param The name of the path.
 */
void
OSEdInfoView::moduleView(const std::string& name) {
   
    OperationIndex& index = OperationContainer::operationIndex();
    clear();
    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    format fmt = texts.text(OSEdTextGenerator::TXT_COLUMN_MODULES);
    InsertColumn(
        0, WxConversion::toWxString(fmt.str()), wxLIST_FORMAT_LEFT, 
        OSEdConstants::DEFAULT_COLUMN_WIDTH);

    int modules = 0;
    try {
        modules = index.moduleCount(name);
    } catch (const PathNotFound& p) {
        // path not found
    }
   
    for (int i = 0; i < modules; i++) {
        OperationModule& module = index.module(i, name);
        InsertItem(i, WxConversion::toWxString(module.name()));
    }

    mode_ = MODE_MODULE;
}

/**
 * Shows the operation view.
 *
 * @param path The path of the module.
 * @param mod The name of the module.
 */
void
OSEdInfoView::operationView(const std::string& path, const std::string& mod) {
    
    OperationIndex& index = OperationContainer::operationIndex();
    OperationModule& module = OperationContainer::module(path, mod);
    clear();
    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    format fmt = texts.text(OSEdTextGenerator::TXT_COLUMN_OPERATIONS);
    InsertColumn(
        0, WxConversion::toWxString(fmt.str()), wxLIST_FORMAT_LEFT,
        OSEdConstants::DEFAULT_COLUMN_WIDTH);

    int operations = 0;
    try {
        operations = index.operationCount(module);
    } catch (const Exception& e) {
        return;
    }
    for (int j = 0; j < operations; j++) {
        string name = index.operationName(j, module);
        wxListItem item;
        if (OperationContainer::isEffective(module, name)) {
            // operation is effective, let's put it with bold font
            item.SetFont(*OSEdConstants::BOLD);
        }
        item.SetId(j);
        int index = InsertItem(item);
        SetItemText(index, WxConversion::toWxString(name));
    }

    mode_ = MODE_OPERATION;
}

/**
 * Shows the operation properties of an operation.
 *
 * @param opName The name of the operation.
 * @param modName The name of the module.
 * @param pathName The name of the path.
 */
void
OSEdInfoView::operationPropertyView(
    const std::string& opName,
    const std::string& modName,
    const std::string& pathName) {
    
    clear();
	
    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    format yesFmt = texts.text(OSEdTextGenerator::TXT_ROW_YES);
    format noFmt = texts.text(OSEdTextGenerator::TXT_ROW_NO);
    format trueFmt = texts.text(OSEdTextGenerator::TXT_ROW_TRUE);
    format falseFmt = texts.text(OSEdTextGenerator::TXT_ROW_FALSE);

    Operation* op = OperationContainer::operation(pathName, modName, opName);
	
    // insert columns
    insertOperationPropertyColumns();
    
    if (op == NULL) {
        // operation properties can not be loaded
        return;
    }

    // write "static" properties
    writeStaticPropertiesOfOperation(op);
    
    // counter for the row
    int i = 7;
    
    format fmt;
    
    // affected by
    for (int k = 0; k < op->affectedByCount(); k++) {
        if (k == 0) {
            fmt = texts.text(OSEdTextGenerator::TXT_ROW_AFFECTED_BY);
            InsertItem(i, WxConversion::toWxString(fmt.str()));
        } else {
            InsertItem(i, _T(""));
        }
        SetItem(i, 1, WxConversion::toWxString(op->affectedBy(k)));
        i++;
    }
    
    // affects
    for (int k = 0; k < op->affectsCount(); k++) {
        if (k == 0) {
            fmt = texts.text(OSEdTextGenerator::TXT_ROW_AFFECTS);
            InsertItem(i, WxConversion::toWxString(fmt.str()));
        } else {
            InsertItem(i, _T(""));
        }
        SetItem(i, 1, WxConversion::toWxString(op->affects(k)));
        i++;
    }
	
    fmt = texts.text(OSEdTextGenerator::TXT_ROW_INPUT_OPERANDS);
    InsertItem(i, WxConversion::toWxString(fmt.str()));
    i++;
    
    // input operands
    for (int j = 1; j <= op->numberOfInputs(); j++) {
        Operand& operand = op->input(j);
        InsertItem(i, _T(""));
        wxString index = WxConversion::toWxString(j);
        fmt = texts.text(OSEdTextGenerator::TXT_ROW_ID);
        index.Prepend(WxConversion::toWxString(fmt.str()));
        SetItem(i, 1, index);
        i++;

        InsertItem(i, _T(""));
        fmt = texts.text(OSEdTextGenerator::TXT_ROW_TYPE);
        SetItem(i, 1, WxConversion::toWxString(fmt.str()));
        SetItem(i, 2, WxConversion::toWxString(operand.typeString()));
        i++;
        
        InsertItem(i, _T(""));
        fmt = texts.text(OSEdTextGenerator::TXT_ROW_MEMORY_ADDRESS);
        SetItem(i, 1, WxConversion::toWxString(fmt.str()));
        
        // is operand address?
        if (operand.isAddress()) {
            SetItem(i, 2, WxConversion::toWxString(yesFmt.str()));
        } else {
            SetItem(i, 2, WxConversion::toWxString(noFmt.str()));
        }
        i++;
        
        InsertItem(i, _T(""));
        fmt = texts.text(OSEdTextGenerator::TXT_ROW_MEMORY_DATA);
        SetItem(i, 1, WxConversion::toWxString(fmt.str()));
        
        // is operand memory data?
        if (operand.isMemoryData()) {
            SetItem(i, 2, WxConversion::toWxString(yesFmt.str()));
        } else {
            SetItem(i, 2, WxConversion::toWxString(noFmt.str()));
        }
        
        i++;
        set<int> canSwap = operand.swap();
        set<int>::iterator it = canSwap.begin();
        
        // can swap list
        while (it != canSwap.end()) {
            wxString opId = WxConversion::toWxString(*it);
            opId.Prepend(_T("id: "));
            if (it == canSwap.begin()) {
                InsertItem(i, _T(""));
                fmt = texts.text(OSEdTextGenerator::TXT_ROW_CAN_SWAP);
                SetItem(i, 1, WxConversion::toWxString(fmt.str()));
                SetItem(i, 2, opId);
				
            } else {
                InsertItem(i, _T(""));
                SetItem(i, 2, opId);
            }
            i++;
            it++;
        }
    }

    fmt = texts.text(OSEdTextGenerator::TXT_ROW_OUTPUT_OPERANDS);
    InsertItem(i, WxConversion::toWxString(fmt.str()));
    i++;

    // outputs
    for (int j = op->numberOfInputs() + 1;
         j <= op->numberOfOutputs() + op->numberOfInputs(); j++) {
		
        Operand& operand = op->output(j);
        InsertItem(i, _T(""));
        wxString opId = WxConversion::toWxString(j);
        fmt = texts.text(OSEdTextGenerator::TXT_ROW_ID);
        opId.Prepend(WxConversion::toWxString(fmt.str()));
        SetItem(i, 1, opId);
        i++;

        InsertItem(i, _T(""));
        fmt = texts.text(OSEdTextGenerator::TXT_ROW_TYPE);
        SetItem(i, 1, WxConversion::toWxString(fmt.str()));
        SetItem(i, 2, WxConversion::toWxString(operand.typeString()));
        i++;
        
        InsertItem(i, _T(""));
        fmt = texts.text(OSEdTextGenerator::TXT_ROW_MEMORY_DATA);
        SetItem(i, 1, WxConversion::toWxString(fmt.str()));
        
        // is operand memory data?
        if (operand.isMemoryData()) {
            SetItem(i, 2, WxConversion::toWxString(yesFmt.str()));
        } else {
            SetItem(i, 2, WxConversion::toWxString(noFmt.str()));
        }
        i++;
    }

    fmt = texts.text(OSEdTextGenerator::TXT_ROW_HAS_BEHAVIOR);
    InsertItem(i, WxConversion::toWxString(fmt.str()));
    OperationModule& module = OperationContainer::module(pathName, modName);
    
    // operation behavior module
    if (OperationContainer::hasBehavior(*op, module)) {
        SetItem(i, 1, WxConversion::toWxString(yesFmt.str()));
    } else {
        SetItem(i, 1, WxConversion::toWxString(noFmt.str()));
    }

    delete op;
    mode_ = MODE_PROPERTY;
}

/**
 * Inserts columns in operation property view.
 */
void
OSEdInfoView::insertOperationPropertyColumns() {
    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    format fmt = texts.text(OSEdTextGenerator::TXT_COLUMN_PROPERTY);
    InsertColumn(
        0, WxConversion::toWxString(fmt.str()), wxLIST_FORMAT_LEFT, 
        OSEdConstants::DEFAULT_COLUMN_WIDTH);
    
    fmt = texts.text(OSEdTextGenerator::TXT_COLUMN_VALUE);
    InsertColumn(
        1, WxConversion::toWxString(fmt.str()), wxLIST_FORMAT_LEFT,
        OSEdConstants::DEFAULT_COLUMN_WIDTH);
    
    fmt = texts.text(OSEdTextGenerator::TXT_COLUMN_OPERAND_VALUE);
    InsertColumn(
        2, WxConversion::toWxString(fmt.str()), wxLIST_FORMAT_LEFT,
        OSEdConstants::DEFAULT_COLUMN_WIDTH);
}

/**
 * Writes the "static" operation properties in operation property view
 *
 * @param op Operation which static properties are written.
 */
void
OSEdInfoView::writeStaticPropertiesOfOperation(Operation* op) {
    
    OSEdTextGenerator& texts = OSEdTextGenerator::instance();

    format trueFmt = texts.text(OSEdTextGenerator::TXT_ROW_TRUE);
    format falseFmt = texts.text(OSEdTextGenerator::TXT_ROW_FALSE);
    // name of the operation
    format fmt = texts.text(OSEdTextGenerator::TXT_ROW_NAME);
    InsertItem(0, WxConversion::toWxString(fmt.str()));
    SetItem(0, 1, WxConversion::toWxString(op->name()));
	
    // number of inputs
    fmt = texts.text(OSEdTextGenerator::TXT_ROW_INPUTS);
    InsertItem(1, WxConversion::toWxString(fmt.str()));
    SetItem(1, 1, WxConversion::toWxString(op->numberOfInputs()));
	
    // number of outputs
    fmt = texts.text(OSEdTextGenerator::TXT_ROW_OUTPUTS);
    InsertItem(2, WxConversion::toWxString(fmt.str()));
    SetItem(2, 1, WxConversion::toWxString(op->numberOfOutputs()));

    // reads memory
    fmt = texts.text(OSEdTextGenerator::TXT_ROW_READS_MEMORY);
    InsertItem(3, WxConversion::toWxString(fmt.str()));
    if (op->readsMemory()) {
        SetItem(3, 1, WxConversion::toWxString(trueFmt.str()));
    } else {
        SetItem(3, 1, WxConversion::toWxString(falseFmt.str()));
    }
	
    // writes memory
    fmt = texts.text(OSEdTextGenerator::TXT_ROW_WRITES_MEMORY);
    InsertItem(4, WxConversion::toWxString(fmt.str()));
    if (op->writesMemory()) {
        SetItem(4, 1, WxConversion::toWxString(trueFmt.str()));
    } else {
        SetItem(4, 1, WxConversion::toWxString(falseFmt.str()));
    }
	
    // can trap
    fmt = texts.text(OSEdTextGenerator::TXT_ROW_CAN_TRAP);
    InsertItem(5, WxConversion::toWxString(fmt.str()));
    if (op->canTrap()) {
        SetItem(5, 1, WxConversion::toWxString(trueFmt.str()));
    } else {
        SetItem(5, 1, WxConversion::toWxString(falseFmt.str()));
    }
	
    // has side effects
    fmt = texts.text(OSEdTextGenerator::TXT_ROW_HAS_SIDE_EFFECTS);
    InsertItem(6, WxConversion::toWxString(fmt.str()));
    if (op->hasSideEffects()) {
        SetItem(6, 1, WxConversion::toWxString(trueFmt.str()));
    } else {
        SetItem(6, 1, WxConversion::toWxString(falseFmt.str()));
    }
}

/**
 * Returns the selected path.
 *
 * If path is not selected returns an empty string.
 *
 * @return The selected path.
 */
string
OSEdInfoView::selectedPath() {
    
    if (mode_ != MODE_PATH || GetSelectedItemCount() != 1) {
        return "";
    }

    return WidgetTools::lcStringSelection(this, 0);
}

/**
 * Returns the selected module.
 *
 * If no module is selected, returns an empty string.
 *
 * @return The selected module.
 */
string
OSEdInfoView::selectedModule() {

    if (mode_ != MODE_MODULE || GetSelectedItemCount() != 1) {
        return "";
    }
    return WidgetTools::lcStringSelection(this, 0);
}

/**
 * Returns the selected operation.
 *
 * If no operation is selected, return empty string.
 *
 * @return The selected operation.
 */
string
OSEdInfoView::selectedOperation() {
    
    if(mode_ != MODE_OPERATION || GetSelectedItemCount() != 1) {
        return "";
    }
    return WidgetTools::lcStringSelection(this, 0);
}

/**
 * Handles the event when list item is selected.
 */
void
OSEdInfoView::onSelection(wxListEvent&) {
    if (GetSelectedItemCount() == 1) {
        OSEdTextGenerator& texts = OSEdTextGenerator::instance();
        OSEdMainFrame* mainFrame = wxGetApp().mainFrame();
        mainFrame->updateMenuBar();
        switch (mode_) {
        case MODE_PATH: {
            format fmt = 
                texts.text(OSEdTextGenerator::TXT_STATUS_PATH_SELECTED);
            mainFrame->statusBar()->
                SetStatusText(WxConversion::toWxString(fmt.str()));
        }
            break;
        case MODE_MODULE: {
            format fmt =
                texts.text(OSEdTextGenerator::TXT_STATUS_MODULE_SELECTED);
            mainFrame->statusBar()->
                SetStatusText(WxConversion::toWxString(fmt.str()));
        }
            break;
        case MODE_OPERATION: {
            format fmt =
                texts.text(OSEdTextGenerator::TXT_STATUS_OPERATION_SELECTED);
            mainFrame->statusBar()->
                SetStatusText(WxConversion::toWxString(fmt.str()));
        }
            break;
        default:
            break;
        }
    }
}

/**
 * Handles the event when list item is clicked with right mouse button.
 *
 * Then the drop down menu is shown.
 *
 * @param event Event to be handled.
 */
void
OSEdInfoView::onDropDownMenu(wxMouseEvent& event) {

    if (GetSelectedItemCount() <= 1 && mode_ != MODE_PROPERTY) {
        // deselect the selected item
        long item = -1;
        item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (item != -1) {
            SetItemState(item, 0, wxLIST_STATE_SELECTED);
        }
        
        wxPoint pos = event.GetPosition();
        int flags = wxLIST_HITTEST_ONITEMLABEL;
        item = -1;
        item = HitTest(pos, flags);

        OSEdTextGenerator& texts = OSEdTextGenerator::instance();
        OSEdMainFrame* mainFrame = wxGetApp().mainFrame();
        DropDownMenu* menu = NULL;
        if (item != -1) {
            // select the item that was clicked
            SetItemState(item, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            switch(mode_) {
            case MODE_PATH: {
                format fmt =
                    texts.text(OSEdTextGenerator::TXT_STATUS_PATH_SELECTED);
                mainFrame->statusBar()->
                    SetStatusText(WxConversion::toWxString(fmt.str()));
                menu = new DropDownMenu(DropDownMenu::MENU_PATH);
                PopupMenu(menu, pos);
            }
                break;
            case MODE_MODULE: {
                format fmt =
                    texts.text(OSEdTextGenerator::TXT_STATUS_MODULE_SELECTED);
                mainFrame->statusBar()->
                    SetStatusText(WxConversion::toWxString(fmt.str()));
                menu = new DropDownMenu(DropDownMenu::MENU_MODULE);
                PopupMenu(menu, pos);
            }
                break;
            case MODE_OPERATION: {
                format fmt =
                    texts.text(OSEdTextGenerator::TXT_STATUS_OPERATION_SELECTED);
                mainFrame->statusBar()->
                    SetStatusText(WxConversion::toWxString(fmt.str()));
                menu = new DropDownMenu(DropDownMenu::MENU_OPERATION);
                PopupMenu(menu, pos);
            }
                break;
            default:
                break;
            }
        }
        delete menu;
    }
}
