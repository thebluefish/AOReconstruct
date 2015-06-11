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

	class AOCFile
	{
	public:
		static wxMutex s_fileMutex;

	public:
		AOCFile(const wxString& path) : _fileName(path) { Process(); };

		const wxFileName& GetName() { return _fileName; };

	public:

		void Scan(wxEvtHandler* eventReceiver = 0);
		void Process(wxEvtHandler* eventReceiver = 0);

	protected:

		wxFileName _fileName;

	};

	class AOCVolume
	{
	public:
		static wxMutex s_volumeMutex;

	public:
		AOCVolume(const wxString& path) : _volumeName(path) { Process(); };

		const wxFileName& GetName() { return _volumeName; };

		const std::deque<AOCFile>& GetAOCFiles() { return _files; };

	public:

		void Scan(wxEvtHandler* eventReceiver = 0);
		void Process(wxEvtHandler* eventReceiver = 0);

	protected:

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
		const wxString& GetAOUsersFolde() { return _aoUsersFolder; };
		const wxString& GetAOCurrentUserFolde() { return _aoCurrentUserFolder; };
		const wxString& GetAOUserCacheFolder() { return _aoUserCacheFolder; };
		const wxString& GetAOCookieFile() { return _aoCookieFile; };


		// List of volumes
		const std::deque<AOCVolume>& GetAOCVolumes() { return _volumes; };

		void Scan();

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
		wxString _aoUsersFolder;
		wxString _aoCurrentUserFolder;
		wxString _aoUserCacheFolder;
		wxString _aoCookieFile;

		std::deque<AOCVolume> _volumes;

	};

	class AOProcessingThread : public wxThread
	{
	public:
		AOProcessingThread(wxEvtHandler* eventReceiver, AOHandler* aoHandler) : 
			_eventReceiver(eventReceiver),
			_aoHandler(aoHandler)
			{ };
		virtual ~AOProcessingThread() { };

		// thread execution starts here
		virtual void* Entry();

		virtual void OnExit();

	protected:
		AOHandler* _aoHandler;

		// Event Handler
		wxEvtHandler* _eventReceiver;
	};
}

#endif