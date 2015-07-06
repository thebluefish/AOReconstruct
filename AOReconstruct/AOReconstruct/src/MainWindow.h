#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include "MainWindowBase.h"

#include <wx/datetime.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/stdpaths.h>


#include <memory>
#include <iostream>
#include <iomanip>
#include <sstream>


namespace AOReconstruct
{
	wxDECLARE_EVENT(wxEVT_STATUS_UPDATE, wxThreadEvent);

	class AOHandler;
	class PSTHandler;
	class AxsOneHandler;

	class StatusUpdateEvent : public wxThreadEvent
	{
	public:
		StatusUpdateEvent(wxString updateString) :
			wxThreadEvent(wxEVT_THREAD, wxEVT_STATUS_UPDATE),
			updateString(updateString)
		{ };

		virtual wxEvent *Clone() const { return new StatusUpdateEvent(*this); }

		wxString updateString;
	};

	class AOCTreeItemData : public wxTreeItemData
	{
	public:
		AOCTreeItemData() : wxTreeItemData(),
			hasBeenPopulated(false) { };
		~AOCTreeItemData() { };

		bool hasBeenPopulated;
	};

	class MainWindow : public MainWindowBase, public wxLog
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

	// Internal functions
	protected:

		void SetReconstituteTabEnabled(bool enabled);
		void SetRebuildTabEnabled(bool enabled);

		void EndReconstituteThreadSafely();
		void EndRebuildThreadSafely();

	// Our events
	protected:

		void OnAOScanUpdate(wxThreadEvent& event);
		void OnAOScanFinished(wxThreadEvent& event);

		void _aocTreeOnTreeItemExpanding(wxTreeEvent& event);

	// Overidden events
	protected:

		// Reconstitute Tab
		virtual void _newPSTPickerOnFileChanged(wxFileDirPickerEvent& event) override;
		virtual void _disableAxsOneButtonOnButtonClick(wxCommandEvent& event) override;
		virtual void _reconstituteButtonOnButtonClick(wxCommandEvent& event) override;

		// Rebuild Tab
		virtual void _pickerPSTOnFileChanged(wxFileDirPickerEvent& event) override;
		virtual void _buttonRebuildOnButtonClick(wxCommandEvent& event) override;

		// Logging
		virtual void DoLogRecord(wxLogLevel level, const wxString& msg, const wxLogRecordInfo& info) override;


	private:
		// Handlers
		std::shared_ptr<AOHandler> _aoHandler;
		std::shared_ptr<PSTHandler> _pstHandler;
		std::shared_ptr<AxsOneHandler> _axsOneHandler;

		std::shared_ptr<wxFile> _logFileHandle;

		// Threading
		AOScanningThread* _scanThread;
		
		// UI Elements
		wxTreeItemId _rootItem;

		

	protected:
		wxDECLARE_EVENT_TABLE();
	};
}

#endif