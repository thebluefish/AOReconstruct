#include "AOHandler.h"

#include "MainWindow.h"

namespace AOReconstruct
{
	wxMutex AOCFile::s_fileMutex;
	wxMutex AOCVolume::s_volumeMutex;


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
		_aoCurrentUserFolder = _aoUsersFolder + _userName + "\\" + _aoUserString + "\\";
		_aoUserCacheFolder = _aoCurrentUserFolder + "Cache\\";
		_aoCookieFile = _aoCurrentUserFolder + _aoUserCookie;
	}

	void AOHandler::Scan()
	{
		// Open Cache folder
		wxDir cacheDir(_aoUserCacheFolder);

		// Unable to open cache folder, abort
		if (!cacheDir.IsOpened())
		{
			return;
		}

		// Scan for volumes
		wxString volumeFolderName;
		bool volumeFolderExists = cacheDir.GetFirst(&volumeFolderName, "*.vol", wxDIR_DIRS);
		while (volumeFolderExists)
		{
			// Convert relative path to absolute path
			volumeFolderName = _aoUserCacheFolder + volumeFolderName;

			// Open folder
			AOCVolume volumeFolder(volumeFolderName);
			volumeFolder.Scan(_eventReceiver);
			_volumes.push_back(volumeFolder);

			volumeFolderExists = cacheDir.GetNext(&volumeFolderName);
		}
	}


	void AOCFile::Scan(wxEvtHandler* eventReceiver)
	{
		auto fileHandler = std::make_shared<wxFile>(_fileName.GetFullPath());

		wxMutexLocker lock(s_fileMutex);

		if (eventReceiver)
		{
			auto event = new StatusUpdateEvent("AOC: " + _fileName.GetFullName(), 1);
			wxQueueEvent(eventReceiver, event);
		}
	}

	void AOCFile::Process(wxEvtHandler* eventReceiver)
	{
		auto fileHandler = std::make_shared<wxFile>(_fileName.GetFullPath());

		wxMutexLocker lock(s_fileMutex);
	}

	void AOCVolume::Scan(wxEvtHandler* eventReceiver)
	{
		auto dirHandler = std::make_shared<wxDir>(_volumeName.GetFullPath());

		wxMutexLocker lock(s_volumeMutex);

		if (eventReceiver)
		{
			auto event = new StatusUpdateEvent("Scanning Volume: " + _volumeName.GetFullName());
			wxQueueEvent(eventReceiver, event);
		}

		// Scan for AOC files
		wxString aocFileName;
		bool aocFileExists = dirHandler->GetFirst(&aocFileName, "*.aoc", wxDIR_FILES);
		while (aocFileExists)
		{
			// Convert relative path to absolute path
			aocFileName = dirHandler->GetNameWithSep() + aocFileName;

			// Create file handler
			AOCFile aocFile(aocFileName);
			aocFile.Scan(eventReceiver);

			// Add file to list
			_files.push_back(aocFile);


			aocFileExists = dirHandler->GetNext(&aocFileName);
		}
	}

	void AOCVolume::Process(wxEvtHandler* eventReceiver)
	{
		auto dirHandler = std::make_shared<wxDir>(_volumeName.GetFullPath());

		wxMutexLocker lock(s_volumeMutex);

	}

	void* AOProcessingThread::Entry()
	{
		_aoHandler->Scan();

		return 0;
	}


	void AOProcessingThread::OnExit()
	{
		auto event1 = new StatusUpdateEvent("Finished Scanning Volumes");
		wxQueueEvent(_eventReceiver, event1);
		
		auto event2 = new StatusUpdateEvent("Finished Scanning Volumes", 1);
		wxQueueEvent(_eventReceiver, event2);

	}

}