#ifndef _AOHANDLER_H_
#define _AOHANDLER_H_

#include <wx/string.h>
#include <wx/memory.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/thread.h>
#include <wx/event.h> 

#include <wx/msw/registry.h>

#include <memory>
#include <deque>

namespace AOReconstruct
{
	wxDECLARE_EVENT(wxEVT_AOC_SCAN_FINISHED, wxThreadEvent);

	class MainWindow;

	class AOCFile
	{
	public:
		friend class AOScanningThread;
		//wxCriticalSection _fileCS;

	public:
		AOCFile(const wxString& path) : _fileName(path) { };

		const wxFileName& GetName() { return _fileName; };

	public:

	protected:

		wxFileName _fileName;

	};

	class AOCVolume
	{
	public:
		friend class AOScanningThread;
		

	public:
		AOCVolume(const wxString& path) : _volumeName(path) { };

		const wxFileName& GetName() { return _volumeName; };

		const std::deque<AOCFile>& GetAOCFiles() { return _files; };

	public:

	protected:
		//wxCriticalSection _volumeCS;
		std::deque<AOCFile> _files;

		wxFileName _volumeName;
	};

	class AOHandler
	{
	public:

		enum WXEvents
		{
			WXE_FIRST = 100,

			WXE_AO_SCAN_UPDATE,

			WXE_LAST
		};

		friend class AOScanningThread;

	public:
		AOHandler(wxEvtHandler* eventReceiver);
		~AOHandler();

		// User Information
		const wxString& GetUserName() { return _userName; };
		const wxString& GetUserProfile() { return _userProfile; };
		const wxString& GetUserDomain() { return _userDomain; };

		// AXSOne Information
		const wxString& GetAOUserString() { return _aoUserString; };
		const wxString& GetAOUserCookie() { return _aoUserCookie; };

		// Folder information
		const wxFileName& GetAOUsersFolder() { return _aoUsersFolder; };


		// List of volumes
		const std::deque<AOCVolume>& GetAOCVolumes() { return _volumes; };

	protected:

		void Initialize();

	protected:
		std::shared_ptr<wxRegKey> _keyVolatileEnvironment;

		// Event Handler
		wxEvtHandler* _eventReceiver;

		// User information
		wxString _userName;
		wxString _userProfile;
		wxString _userDomain;

		// AXSOne information
		wxString _aoUserString;
		wxString _aoUserCookie;

		// Folder information
		wxFileName _aoUsersFolder;
		
		wxFileName _aoUserCacheFolder;

		std::deque<AOCVolume> _volumes;

	};

	class AOScanningThread : public wxThread
	{
	public:
		AOScanningThread(wxFileName& pstFileName, wxEvtHandler* eventReceiver, AOHandler* aoHandler) :
			wxThread(wxTHREAD_DETACHED),
			_pstFileName(pstFileName),
			_eventReceiver(eventReceiver),
			_aoHandler(aoHandler)
			{ };
		virtual ~AOScanningThread();

		// thread execution starts here
		virtual void* Entry();

		virtual void OnExit();

		

	protected:
		AOHandler* _aoHandler;

		// Event Handler
		wxEvtHandler* _eventReceiver;

		wxFileName _pstFileName;
	};
}

#endif