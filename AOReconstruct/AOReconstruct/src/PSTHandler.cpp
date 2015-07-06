

#include "Utils.h"
#include "PSTTypes.h"

#include <mapitags.h>
#include <mapiutil.h>
#include <Mapidefs.h>
#include <mspst.h>

#include "PSTHandler.h"


namespace AOReconstruct
{
	class MapiInitializer
	{
	public:
		static void Init()
		{
			++m_refCounter;
			if (m_refCounter == 1)
			{
				MAPIINIT init = { 0, MAPI_MULTITHREAD_NOTIFICATIONS };
				HRESULT hr = MAPIInitialize(&init);
				if (hr != S_OK)
				{
					char msgBuff[512];
					sprintf_s(msgBuff, sizeof(msgBuff), "MAPIInitError: 0x%.8X", hr);
					throw std::runtime_error(msgBuff);
				}
			}
		}
		static void Uninit()
		{
			--m_refCounter;
			if (m_refCounter == 0)
			{
				MAPIUninitialize();
			}
		}
	private:
		static int m_refCounter;
	};

	int MapiInitializer::m_refCounter = 0;

	/*PSTHandler::PSTHandler(const std::string &path, const std::string& password, const std::wstring &storeDisplayName, wxEvtHandler* eventReceiver) :
		_eventReceiver(eventReceiver)
	{
		MapiInitializer::Init();

		sprintf_s(m_profileName, 100, "AOReconstruct Temp");

		memset(&m_IPF, 0, sizeof(m_IPF));
		HRESULT hr = NOERROR;

		// Get a profile administration object.
		hr = MAPIAdminProfiles(0, &m_pIProfAdmin);
		if (FAILED(hr))
		{
			//auto event = new StatusUpdateEvent("Can't AdminProfiles", 1);
			//wxQueueEvent(_eventReceiver, event);
		}

		// Creating profile
		(void)m_pIProfAdmin->DeleteProfile((LPTSTR)&m_profileName, 0);
		hr = m_pIProfAdmin->CreateProfile((LPTSTR)&m_profileName, NULL, 0, 0);
		if (FAILED(hr))
		{
			//auto event = new StatusUpdateEvent("Can't CreateProfile", 1);
			//wxQueueEvent(_eventReceiver, event);
		}

		// Get a service administration object.
		hr = m_pIProfAdmin->AdminServices((LPTSTR)&m_profileName, 0, 0, 0, &m_pSvcAdmin);
		if (FAILED(hr))
		{
			//auto event = new StatusUpdateEvent("Can't AdminServices", 1);
			//wxQueueEvent(_eventReceiver, event);
		}
		char *pszMsgService = "";

		if (!FAILED(hr = m_pSvcAdmin->CreateMsgService("MSUPST MS", "MAPI Pst Msg Store", 0, 0)))
		{
			pszMsgService = "MSUPST MS";
		}
		else
		{
			//auto event = new StatusUpdateEvent("Error while creating Message Service (MSUPST MS). Probably Outllok 2007 or later was not installed", 1);
			//wxQueueEvent(_eventReceiver, event);
		}

		// Configure the MS Personal Information Store per NewPST entries.
		hr = m_pSvcAdmin->GetMsgServiceTable(0, &m_ptblSvc);
		if (FAILED(hr))
		{
			//auto event = new StatusUpdateEvent("GetMsgServiceTable failed", 1);
			//wxQueueEvent(_eventReceiver, event);
		}

		COM::AutoPtr<SRowSet, RowEraser> prows;
		COM::AutoPtr<SRowSet, RowEraser> pRows;

		enum { iSvcName, iSvcUID, iEnrtyID, cptaSvc };

		// We don't need to worry about checking all the columns; there should
		// only be one since we just created the profile and added one service.
		SizedSPropTagArray(cptaSvc, ptaSvc) = { cptaSvc, { PR_SERVICE_NAME, PR_SERVICE_UID, PR_ENTRYID } };
		hr = HrQueryAllRows(m_ptblSvc, (LPSPropTagArray)&ptaSvc, NULL, NULL, 0, &pRows);
		if (FAILED(hr))
		{
			//auto event = new StatusUpdateEvent("HrQueryAllRows failed", 1);
			//wxQueueEvent(_eventReceiver, event);
		}

		int cRows = pRows->cRows;

		for (LPSRow pRow = pRows->aRow; pRow < pRows->aRow + cRows; ++pRow)
		{
			if (pRow->lpProps[iSvcUID].ulPropTag != PR_SERVICE_UID)
			{
				//auto event = new StatusUpdateEvent("Wrong property tag for service ID", 1);
				//wxQueueEvent(_eventReceiver, event);
			}
			m_MsgStoreUID = *((LPMAPIUID)pRow->lpProps[iSvcUID].Value.bin.lpb);
			if (strcmp(pRow->lpProps[iSvcName].Value.lpszA, pszMsgService) == 0)
			{
				// Configuring PST Message Store
				ULONG count = 0;
				const ULONG nMAXProps = 2;
				SPropValue   rgval[nMAXProps];

				rgval[count].ulPropTag = PR_DISPLAY_NAME_W;
				rgval[count].Value.lpszW = const_cast<wchar_t*>(storeDisplayName.c_str());
				++count;

				rgval[count].ulPropTag = PR_PST_PATH;
				rgval[count].Value.lpszA = const_cast<char*>(path.c_str());

				//rgval[count].ulPropTag = PR_PST_PW_SZ_OLD;
				//rgval[count].Value.lpszA = const_cast<char*>(password.c_str());
				//++count;

				//rgval[count].ulPropTag = PR_PST_PW_SZ_NEW;
				//rgval[count].Value.lpszA = const_cast<char*>(password.c_str());
				//++count;

				//rgval[count].ulPropTag = PR_PST_REMEMBER_PW;
				//rgval[count].Value.b = TRUE;
				//++count;

				hr = m_pSvcAdmin->ConfigureMsgService(&m_MsgStoreUID, 0, 0, count, rgval);

				if (hr == MAPI_E_FAILONEPROVIDER)
				{
					LPMAPIERROR lpMAPIError;
					if (m_pSvcAdmin->GetLastError(hr, 0, &lpMAPIError) == S_OK)
					{
						if (lpMAPIError->ulContext == MAPI_E_FAILONEPROVIDER || lpMAPIError->ulContext == 0x30060401)
						{
							//auto event = new StatusUpdateEvent("Failed to logon - Incorrect password", 1);
							//wxQueueEvent(_eventReceiver, event);
						}
						else
						{
							//auto event = new StatusUpdateEvent("Failed to logon - Unknown reason", 1);
							//wxQueueEvent(_eventReceiver, event);
						}
					}
					else
					{
						//auto event = new StatusUpdateEvent("Can't configure PST message store (ConfigureMsgService)", 1);
						//wxQueueEvent(_eventReceiver, event);
					}
				}
			}
		}

		hr = MAPILogonEx(0, m_profileName, NULL, MAPI_NEW_SESSION | MAPI_EXTENDED | MAPI_NO_MAIL | MAPI_TIMEOUT_SHORT, &m_pses);
		if (FAILED(hr))
		{
			//auto event = new StatusUpdateEvent("MAPILogonEx failed", 1);
			//wxQueueEvent(_eventReceiver, event);
		}

		MapiTable ptable;
		hr = m_pses->GetMsgStoresTable(0, &ptable);
		if (FAILED(hr))
		{
			//auto event = new StatusUpdateEvent("GetMsgStoresTable failed", 1);
			//wxQueueEvent(_eventReceiver, event);
		}
	}*/

	PSTHandler::PSTHandler(wxEvtHandler* eventReceiver) :
		_eventReceiver(eventReceiver)
	{

	}
	PSTHandler::~PSTHandler()
	{

	}
}