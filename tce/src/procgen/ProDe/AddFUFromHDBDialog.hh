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
 * @file AddFUFromHDBDialog.hh
 *
 * Declaration of AddFUFromHDBDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_FU_FROM_HDB_DIALOG_HH
#define TTA_ADD_FU_FROM_HDB_DIALOG_HH

#include <map>
#include <vector>
#include <string>

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/timer.h>

class Model;
class wxSearchCtrl;

struct ListItemData {
    int id;
    int latency;
    wxString operations;
    int hdbId;
    wxString path;
};

namespace HDB {
    class FUArchitecture;
    class HDBManager;
}

/**
 * Dialog for adding register file architectures directly from HDB to the
 * current machine.
 */
class AddFUFromHDBDialog : public wxDialog {
public:
    AddFUFromHDBDialog(wxWindow* parent, Model* model);
    virtual ~AddFUFromHDBDialog();

private:
    virtual bool TransferDataToWindow();
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void onListSelectionChange(wxListEvent& event);
    void onAdd(wxCommandEvent& event);
    void onClose(wxCommandEvent& event);
    bool loadHDB(const HDB::HDBManager& manager);
    bool acceptToList(
        const std::string hdbFilePath, 
        const HDB::FUArchitecture& arch,
        const std::vector<std::string>& filterList);
    void onFilterChange(wxCommandEvent& event);
    void onFilterTimeOut(wxTimerEvent& event);
    void onColumnClick(wxListEvent& event);
    void setColumnImage(int col, int image);

    /// Model of the current adf file.
    Model* model_;
    /// Immediate slot list widget.
    wxListCtrl* list_;
    /// Map of iu architectures displayed in the dialog list.
    std::map<int, HDB::FUArchitecture*> fuArchitectures_;
    /// The list filter text control.
    wxSearchCtrl* filterCtrl_ = nullptr;
    /// Keywords to filter HDB entries.
    std::vector<std::string> filterPatterns_;
    /// Timer to postpone filtering while typing filter patterns.
    wxTimer filterTimer_;

    int sortColumn_;
    bool sortASC_;

    enum {
        ID_LIST = 10000,
        ID_FILTER_LABEL,
        ID_FILTER_TEXTCTRL,
        ID_ADD,
        ID_CLOSE,
        ID_LINE,
        ID_FILTER_TIMER
    };

    /// File filter for HDB files.
    static const wxString HDB_FILE_FILTER;

    DECLARE_EVENT_TABLE()
};
#endif
