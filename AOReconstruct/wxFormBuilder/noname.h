///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  5 2014)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __NONAME_H__
#define __NONAME_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/statusbr.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/filepicker.h>
#include <wx/statline.h>
#include <wx/checklst.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/gauge.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/treectrl.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/textctrl.h>
#include <wx/notebook.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class MainWindowBase
///////////////////////////////////////////////////////////////////////////////
class MainWindowBase : public wxFrame 
{
	private:
	
	protected:
		wxStatusBar* _statusBar;
		wxNotebook* m_notebook1;
		wxPanel* _reconstituteTab;
		wxStaticText* _labelStep1;
		wxFilePickerCtrl* _newPSTPicker;
		wxStaticLine* _lineSeparator1;
		wxStaticText* _labelStep2;
		wxCheckListBox* _archivesCheckList;
		wxStaticText* _labelStep3;
		wxButton* _disableAxsOneButton;
		wxGauge* _scanGauge;
		wxButton* _scanButton;
		wxPanel* _rebuildTab;
		wxStaticText* _labelSelectPST;
		wxFilePickerCtrl* _pickerPST;
		wxStaticLine* _lineSeparator;
		wxTreeCtrl* _aocTree;
		wxPanel* _panelOptions;
		wxCheckBox* _backupArchiveCheckbox;
		wxCheckBox* _disableAxsOneCheckbox;
		wxGauge* _rebuildGauge;
		wxButton* _buttonRebuild;
		wxPanel* _logTab;
		wxTextCtrl* _logTextbox;
		
		// Virtual event handlers, overide them in your derived class
		virtual void _pickerPSTOnFileChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		
	
	public:
		
		MainWindowBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,424 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		
		~MainWindowBase();
	
};

#endif //__NONAME_H__
