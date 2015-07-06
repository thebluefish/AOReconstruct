#include "MainWindow.h"


#include "Utils.h"
#include "PSTHandler.h"
#include "AxsOneHandler.h"
#include "AOHandler.h"

namespace AOReconstruct
{
	wxDEFINE_EVENT(wxEVT_STATUS_UPDATE, wxThreadEvent);

	wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
		EVT_THREAD(wxEVT_STATUS_UPDATE, MainWindow::OnAOScanUpdate)
		EVT_THREAD(wxEVT_AOC_SCAN_FINISHED, MainWindow::OnAOScanFinished)
		wxEND_EVENT_TABLE()
		MainWindow::MainWindow()
		: MainWindowBase(NULL, wxID_ANY, "AXS-One \"Get me out of this mess\" Tool", wxPoint(0, 0), wxSize(500, 420)),
		_scanThread(0)
	{


		_aoHandler = std::make_shared<AOHandler>(this);
		_pstHandler = std::make_shared<PSTHandler>(this);
		_axsOneHandler = std::make_shared<AxsOneHandler>();

		_aocTree->AddRoot("Volumes");

		// Setup Logging
		SetActiveTarget(this);

		// Open log file
		_logFileHandle = std::make_shared<wxFile>();

		// Make sure log directory exists
		auto logDir = std::make_shared<wxDir>();

		wxString fileNameBuilder;
		wxFileName filePath(wxStandardPaths::Get().GetExecutablePath());
		fileNameBuilder << filePath.GetPath() + "\\Logs";

		if(!logDir->Exists(fileNameBuilder))
		{
			logDir->Make(fileNameBuilder);
		}

		fileNameBuilder << "\\AOReconstruct_";

		wxString fullFileName;
		for (int i = 0; i < MAXINT; i++)
		{
			fullFileName = "";
			fullFileName << fileNameBuilder << i << ".log";

			if (!_logFileHandle->Exists(fullFileName))
			{
				_logFileHandle->Create(fullFileName);
				break;
			}
		}

		wxLogMessage(L"Opened log file: " + fullFileName);
		wxLogMessage(L"Application Started");
	}

	MainWindow::~MainWindow()
	{
		wxLogMessage(L"Application Closed");

		SetActiveTarget(NULL);

		EndReconstituteThreadSafely();
		EndRebuildThreadSafely();
	}

	void MainWindow::SetReconstituteTabEnabled(bool enabled)
	{
		_newPSTPicker->Enable(enabled);
		_newPSTPicker->SetPath(L"");

		_reconstituteButton->Enable(false);

	}
	void MainWindow::SetRebuildTabEnabled(bool enabled)
	{
		_pickerPST->Enable(enabled);
		_pickerPST->SetPath(L"");

		_buttonRebuild->Enable(false);
	}

	void MainWindow::EndReconstituteThreadSafely()
	{
		{
			wxCriticalSectionLocker enter(_scanThreadCS);

			if (_scanThread)
			{
				_scanThread->Delete();
			}
		}

		while (1)
		{
			{ // was the ~MyThread() function executed?
				wxCriticalSectionLocker enter(_scanThreadCS);
				if (!_scanThread) break;
			}
			// wait for thread completion
			wxThread::This()->Sleep(1);
		}
	}

	void MainWindow::EndRebuildThreadSafely()
	{
		{
			wxCriticalSectionLocker enter(_scanThreadCS);

			if (_scanThread)
			{
				_scanThread->Delete();
			}
		}

		while (1)
		{
			{ // was the ~MyThread() function executed?
				wxCriticalSectionLocker enter(_scanThreadCS);
				if (!_scanThread) break;
			}
			// wait for thread completion
			wxThread::This()->Sleep(1);
		}
	}

	void MainWindow::OnAOScanUpdate(wxThreadEvent& event)
	{
		auto aoUpdateEvent = static_cast<StatusUpdateEvent&>(event);

		_statusBar->SetStatusText(aoUpdateEvent.updateString, 1);
	}

	void MainWindow::OnAOScanFinished(wxThreadEvent& event)
	{
		//EndScanThreadSafely();

		wxLogMessage(L"Volume scan finished");

		_statusBar->SetStatusText("Parsing data");
		_statusBar->SetStatusText("Updating...", 1);

		auto treeRoot = _aocTree->GetRootItem();

		auto& volumes = _aoHandler->GetAOCVolumes();
		for each (AOCVolume vol in volumes)
		{
			wxFileName volumeName = vol.GetName();

			wxString itemName = volumeName.GetFullName();
			itemName << " (" << vol.GetAOCFiles().size() << ")";

			auto treeVolItem = _aocTree->AppendItem(treeRoot, itemName);
			_aocTree->SetItemData(treeVolItem, new AOCTreeItemData());

			if (vol.GetAOCFiles().size() > 0)
			{
				_aocTree->SetItemHasChildren(treeVolItem, true);
			}
			//for each (AOCFile file in aocFiles)
			//{
			//	wxFileName name = file.GetName();
			//	auto treeAOCItem = _aocTree->AppendItem(treeVolItem, name.GetFullName());
			//}
		}

		_statusBar->SetStatusText("Finished Scan");
		_statusBar->SetStatusText("", 1);
	}

	void MainWindow::_aocTreeOnTreeItemExpanding(wxTreeEvent& event)
	{
		auto item = event.GetItem();
		auto label = _aocTree->GetItemText(item);

		auto treeRoot = _aocTree->GetRootItem();

		auto itemData = static_cast<AOCTreeItemData*>(_aocTree->GetItemData(item));

		// Don't populate list if this volume has already been populated
		if (itemData->hasBeenPopulated)
			return;

		auto& volumes = _aoHandler->GetAOCVolumes();
		for each (AOCVolume vol in volumes)
		{
			wxFileName name = vol.GetName();

			if (name.GetFullName() == label)
			{
				// Populate expanded volume
				auto& aocFiles = vol.GetAOCFiles();
				for each (AOCFile file in aocFiles)
				{
					wxFileName name = file.GetName();
					auto treeAOCItem = _aocTree->AppendItem(item, name.GetFullName());
				}

				break;
			}
		}

		itemData->hasBeenPopulated = true;
	}

	void MainWindow::_newPSTPickerOnFileChanged(wxFileDirPickerEvent& event)
	{
		EndReconstituteThreadSafely();

		wxFileName pstFileName = event.GetPath();

		_reconstituteButton->Enable(true);
	}

	void MainWindow::_disableAxsOneButtonOnButtonClick(wxCommandEvent& event)
	{
		
	}

	void MainWindow::_reconstituteButtonOnButtonClick(wxCommandEvent& event)
	{
		_newPSTPicker->Enable(false);
		_reconstituteButton->Enable(false);
		SetRebuildTabEnabled(false);

	}

	void MainWindow::_pickerPSTOnFileChanged(wxFileDirPickerEvent& event)
	{
		EndRebuildThreadSafely();

		wxFileName pstFileName = event.GetPath();

		// Clear the TreeView
		auto treeRoot = _aocTree->GetRootItem();
		if (treeRoot)
		{
			_aocTree->DeleteChildren(treeRoot);
		}
		

		_buttonRebuild->Enable(true);

		_scanThread = new AOScanningThread(pstFileName, this, _aoHandler.get());
		_scanThread->Run();

		//std::string path(pstFileName.GetFullPath());
		//std::string password("skauf1skauf1@NIKEskauf1_Cache.pst");
		//std::string storeName("skauf1_Cache");

		//_pstHandler = std::make_shared<PSTHandler>(path, password, Utils::string2wstring(storeName));
	}

	void MainWindow::_buttonRebuildOnButtonClick(wxCommandEvent& event)
	{
		_pickerPST->Enable(false);
		_buttonRebuild->Enable(false);
		SetReconstituteTabEnabled(false);
		
	}


	void MainWindow::DoLogRecord(wxLogLevel level, const wxString& msg, const wxLogRecordInfo& info)
	{
		wxString logRecord;

		wxDateTime time(info.timestamp);
		
		// Build log
		// Format: [DD-MM-YYYY] LEVEL: MESSAGE
		logRecord << time.Format("[%Y-%m-%d %H:%M:%S] ");

		switch (level)
		{
		case wxLOG_FatalError:
			logRecord << L"Fatal Error: ";
			break;
		case wxLOG_Error:
			logRecord << L"Error: ";
			break;
		case wxLOG_Warning:
			logRecord << L"Warning: ";
			break;
		case wxLOG_Message:
			logRecord << L"Message: ";
			break;
		case wxLOG_Status:
			logRecord << L"Status: ";
			break;
		case wxLOG_Info:
			logRecord << L"Info: ";
			break;
		case wxLOG_Debug:
			logRecord << L"Debug: ";
			break;
		case wxLOG_Trace:
			logRecord << L"Trace: ";
			break;
		}
		logRecord << msg << "\r\n";

		// Send the log to the log textbox
		_logTextbox->AppendText(logRecord);

		// Send the log to our log file
		if (_logFileHandle->IsOpened())
		{
			_logFileHandle->Write(logRecord);
		}
	}
}