#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_



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
#include <wx/treectrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/frame.h>


#include <wx/filename.h>

#include <memory>

#include "PSTHandler.h"

namespace AOReconstruct
{
	wxDECLARE_EVENT(wxEVT_STATUS_UPDATE, wxThreadEvent);

	class AOHandler;

	class StatusUpdateEvent : public wxThreadEvent
	{
	public:
		StatusUpdateEvent(wxString updateString, int index = 0) :
			wxThreadEvent(wxEVT_THREAD, wxEVT_STATUS_UPDATE),
			updateString(updateString),
			statusBarIndex(index)
		{ };

		virtual wxEvent *Clone() const { return new StatusUpdateEvent(*this); }

		wxString updateString;
		int statusBarIndex;
	};

	class AOCTreeItemData : public wxTreeItemData
	{
	public:
		AOCTreeItemData() : wxTreeItemData(),
			hasBeenPopulated(false)
		{

		};

		~AOCTreeItemData() { };

		bool hasBeenPopulated;
	};

	class MainWindow : public wxFrame
	{
	public:

		enum WXEvents
		{
			WXE_FIRST = 1,



			WXE_LAST
		};

		friend class AOScanningThread;
		wxCriticalSection _scanThreadCS;

	public:
		MainWindow();
		~MainWindow();

	protected:

		void EndScanThreadSafely();

		void CreateLayout();
		void Initialize();

	protected:

		void OnAOScanUpdate(wxThreadEvent& event);
		void OnAOScanFinished(wxThreadEvent& event);

		void _pickerPSTOnFileChanged(wxFileDirPickerEvent& event);
		void _aocTreeOnTreeItemExpanding(wxTreeEvent& event);

	private:
		// Handlers
		std::shared_ptr<AOHandler> _aoHandler;
		std::shared_ptr<PSTHandler> _pstHandler;
		// Threading
		AOScanningThread* _scanThread;
		
		// UI Elements

		///////////////////////////
		//BEGIN AUTO-GENERATED CODE
		
		wxStatusBar* _statusBar;
		wxStaticText* _labelSelectPST;
		wxFilePickerCtrl* _pickerPST;
		wxStaticLine* _lineSeparator;
		wxTreeCtrl* _aocTree;
		wxPanel* _panelOptions;
		wxCheckBox* m_checkBox1;
		wxButton* _buttonRebuild;

		//END AUTO-GENERATED CODE
		///////////////////////////

		wxTreeItemId _rootItem;

		

	protected:
		wxDECLARE_EVENT_TABLE();
	};
}

#endif