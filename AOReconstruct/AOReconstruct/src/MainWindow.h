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
#include <wx/treectrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/frame.h>
#include <wx/filename.h>

#include <memory>

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

	class MainWindow : public wxFrame
	{
		

	public:

		enum WXEvents
		{
			WXE_FIRST = 1,



			WXE_LAST
		};

	public:
		MainWindow();
		~MainWindow();

		void OnAOScanUpdate(wxThreadEvent& event);

	protected:

		void CreateLayout();
		void Initialize();

	protected:

		

	private:
		// Handlers
		AOHandler* _aoHandler;

		// UI Elements

		///////////////////////////
		//BEGIN AUTO-GENERATED CODE
		
		wxStatusBar* _statusBar;
		wxNotebook* _mainNotebook;
		wxPanel* _generalPanel;
		wxPanel* _aocTreePanel;
		wxTreeCtrl* _aocTree;

		//END AUTO-GENERATED CODE
		///////////////////////////

		wxTreeItemId _rootItem;

		// Threading
		wxThread* _scanThread;

	protected:
		wxDECLARE_EVENT_TABLE();
	};
}

#endif