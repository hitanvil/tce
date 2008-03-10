/**
 * @file HDBEditorAboutDialog.hh
 * 
 * Declaration of HDBEditorAboutDialog class.
 * 
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_HDB_EDITOR_ABOUT_DIALOG_HH
#define TTA_HDB_EDITOR_ABOUT_DIALOG_HH

#include <wx/wx.h>
#include <wx/statline.h>

/**
 * About dialog for HDBEditor.
 */
class HDBEditorAboutDialog : public wxDialog {
public:
    HDBEditorAboutDialog(wxWindow* parent);
    virtual ~HDBEditorAboutDialog();
   
private: 
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// Widget IDs.
    enum {
	ID_TEXT_PROGRAM_NAME = 10000,
	ID_TEXT_PROGRAM_VERSION,
    ID_TEXT_PROGRAM_COPYRIGHT,
	ID_LINE
    };
};
#endif
