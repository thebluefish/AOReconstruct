#include "AOHandler.h"
#include "MainWindow.h"

#include <wx/string.h>
#include <wx/memory.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/thread.h>
#include <wx/event.h> 
#include <wx/msw/registry.h>

#include "zlib.h"

namespace AOReconstruct
{
	wxDEFINE_EVENT(wxEVT_AOC_SCAN_FINISHED, wxThreadEvent);

	AOHandler::AOHandler(wxEvtHandler* eventReceiver) :
		_eventReceiver(eventReceiver)
	{
		Initialize();
	}

	AOHandler::~AOHandler()
	{

	}

	void AOHandler::Initialize()
	{
		// Get user info
		_keyVolatileEnvironment = std::make_shared<wxRegKey>(wxRegKey::HKCU, wxT("Volatile Environment"));

		_keyVolatileEnvironment->QueryValue("USERNAME", _userName);
		_keyVolatileEnvironment->QueryValue("USERPROFILE", _userProfile);
		_keyVolatileEnvironment->QueryValue("USERDOMAIN", _userDomain);

		// Construct AXSOne information
		_aoUserString = _userName + "@" + _userDomain;
		_aoUserCookie = _aoUserString + ".cookie";
		
		// Construct folder information
		_aoUsersFolder = "C:\\AXSOne\\OutlookAddin\\Users\\";
		
	}

	void* AOScanningThread::Entry()
	{
		_aoHandler->_volumes.clear();
		_aoHandler->_aoUserCacheFolder = _pstFileName.GetPathWithSep() + "Cache" + _pstFileName.GetPathSeparator();

		// Open Cache folder
		wxDir cacheDir(_aoHandler->_aoUserCacheFolder.GetFullPath());

		// Unable to open cache folder, abort
		if (!cacheDir.IsOpened())
		{
			return 0;
		}

		wxString outDir = _aoHandler->GetAOUsersFolder().GetFullPath() + "\\" + _aoHandler->GetAOUserName() + "\\Temp\\";

		// Scan for volumes
		wxString volumeFolderName;
		bool volumeFolderExists = cacheDir.GetFirst(&volumeFolderName, "*.vol", wxDIR_DIRS);
		while (volumeFolderExists && !TestDestroy())
		{
			// Convert relative path to absolute path
			volumeFolderName = _aoHandler->_aoUserCacheFolder.GetFullPath() + volumeFolderName;

			// Open folder
			AOCVolume volumeFolder(volumeFolderName);
			{
				auto dirHandler = std::make_shared<wxDir>(volumeFolder._volumeName.GetFullPath());

				if (_eventReceiver)
				{
					auto event = new StatusUpdateEvent("Scanning Volume: " + volumeFolder._volumeName.GetFullName());
					wxQueueEvent(_eventReceiver, event);
				}

				// Scan for AOC files
				wxString aocFileName;
				bool aocFileExists = dirHandler->GetFirst(&aocFileName, "*.aoc", wxDIR_FILES);
				while (aocFileExists && !TestDestroy())
				{
					// Convert relative path to absolute path
					aocFileName = dirHandler->GetNameWithSep() + aocFileName;

					// Create file handler
					AOCFile aocFile(aocFileName);
					//auto fileHandler = std::make_shared<wxFile>(aocFile._fileName.GetFullPath());
					//auto outFileHandler = std::make_shared<wxFile>(outDir + aocFile._fileName.GetName() + ".msg", wxFile::write);

					volumeFolder._files.push_back(aocFile);

					aocFileExists = dirHandler->GetNext(&aocFileName);
				}
				wxString logMessage;
				logMessage << "Finished scanning volume " << volumeFolder._volumeName.GetFullName() << " (" << volumeFolder.GetAOCFiles().size() << " items)";
				wxLogMessage(logMessage);
			}
			_aoHandler->_volumes.push_back(volumeFolder);

			volumeFolderExists = cacheDir.GetNext(&volumeFolderName);
		}

		return 0;
	}

	AOScanningThread::~AOScanningThread()
	{
		MainWindow* mainWindow = static_cast<MainWindow*>(_eventReceiver);

		wxCriticalSectionLocker enter(mainWindow->_scanThreadCS);

		mainWindow->_scanThread = 0;

	};

	void AOScanningThread::OnExit()
	{
		auto event3 = new wxThreadEvent(wxEVT_THREAD, wxEVT_AOC_SCAN_FINISHED);
		wxQueueEvent(_eventReceiver, event3);

	}

}