#include "AOHandler.h"

#include "MainWindow.h"

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
					auto fileHandler = std::make_shared<wxFile>(aocFile._fileName.GetFullPath());
					auto outFileHandler = std::make_shared<wxFile>(aocFile._fileName.GetPathWithSep() + aocFile._fileName.GetName() + ".msg", wxFile::write);

					if (_eventReceiver)
					{
						auto event = new StatusUpdateEvent("AOC: " + aocFile._fileName.GetFullName(), 1);
						wxQueueEvent(_eventReceiver, event);
					}

#define CHUNK 16384

					int ret;
					unsigned have;
					z_stream strm;
					unsigned char in[CHUNK];
					unsigned char out[CHUNK];

					/* allocate inflate state */
					strm.zalloc = Z_NULL;
					strm.zfree = Z_NULL;
					strm.opaque = Z_NULL;
					strm.avail_in = 0;
					ret = inflateInit2(&strm, 56);
					if (ret == Z_OK)
					{
						/* decompress until deflate stream ends or end of file */
						do {
							strm.avail_in = fileHandler->Read(in, CHUNK);
							if (strm.avail_in == 0)
								break;
							strm.next_in = in;

							/* run inflate() on input until output buffer not full */
							do 
							{
								strm.avail_out = CHUNK;
								strm.next_out = out;
								ret = inflate(&strm, Z_NO_FLUSH);
								assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

								wxString error;

								switch (ret) 
								{
								case Z_NEED_DICT:
									ret = Z_DATA_ERROR;     /* and fall through */
									error = wxT("inflate error - Z_NEED_DICT");
								case Z_DATA_ERROR:
									error = wxT("inflate error - Z_DATA_ERROR");
								case Z_MEM_ERROR:
									error = wxT("inflate error - Z_MEM_ERROR");

									auto event_Z_MEM_ERROR = new StatusUpdateEvent(error, 1);
									wxQueueEvent(_eventReceiver, event_Z_MEM_ERROR);

									(void)inflateEnd(&strm);
									break;
								}
								have = CHUNK - strm.avail_out;
								outFileHandler->Write(out, have);

							} while (strm.avail_out == 0);

							/* done when inflate() says it's done */
						} while (ret != Z_STREAM_END);

						/* clean up and return */
						(void)inflateEnd(&strm);
					}
					else
					{
						volumeFolder._files.push_back(aocFile);
					}

					// Add file to list
					//volumeFolder._files.push_back(aocFile);

					aocFileExists = dirHandler->GetNext(&aocFileName);
				}
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