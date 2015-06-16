#include "PSTHandler.h"

#include "Utils.h"
#include "PSTTypes.h"

#include <mapitags.h>
#include <mapiutil.h>
#include <Mapidefs.h>
#include <mspst.h>

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

	PSTHandler::PSTHandler(const std::string &path, const std::string& password, const std::wstring &storeDisplayName)
	{
		MapiInitializer::Init();

		sprintf_s(m_profileName, 100, "AOReconstruct Temp");

		memset(&m_IPF, 0, sizeof(m_IPF));
		HRESULT hr = NOERROR;

		// Get a profile administration object.
		hr = MAPIAdminProfiles(0, &m_pIProfAdmin);
		if (FAILED(hr))
		{
			throw std::runtime_error("Can't AdminProfiles");
		}

		// Creating profile
		(void)m_pIProfAdmin->DeleteProfile((LPTSTR)&m_profileName, 0);
		hr = m_pIProfAdmin->CreateProfile((LPTSTR)&m_profileName, NULL, 0, 0);
		if (FAILED(hr))
		{
			throw std::runtime_error("Can't CreateProfile");
		}

		// Get a service administration object.
		hr = m_pIProfAdmin->AdminServices((LPTSTR)&m_profileName, 0, 0, 0, &m_pSvcAdmin);
		if (FAILED(hr))
		{
			throw std::runtime_error("Can't AdminServices");
		}
		char *pszMsgService = "";

		if (!FAILED(hr = m_pSvcAdmin->CreateMsgService("MSUPST MS", "MAPI Pst Msg Store", 0, 0)))
		{
			pszMsgService = "MSUPST MS";
		}
		else
		{
			throw std::runtime_error("Error while creating Message Service (MSUPST MS). Probably Outllok 2007 or later was not installed");
		}

		// Configure the MS Personal Information Store per NewPST entries.
		hr = m_pSvcAdmin->GetMsgServiceTable(0, &m_ptblSvc);
		if (FAILED(hr))
		{
			throw std::runtime_error("GetMsgServiceTable failed");
		}

		//hr = MAPILogonEx(0, m_profileName, NULL, MAPI_NEW_SESSION | MAPI_NO_MAIL | MAPI_EXTENDED | MAPI_TIMEOUT_SHORT, &m_pses);
		//if (FAILED(hr))
		//{
		//	throw std::runtime_error("MAPILogonEx failed");
		//}

		//LPSRowSet pRows = NULL;
		//const int nProperties = 2;
		//SizedSPropTagArray(nProperties, Columns) = { nProperties, { PR_DISPLAY_NAME_A, PR_RESOURCE_TYPE } };

		//IMAPITable*	pStatusTable;
		//if (m_pses->GetStatusTable(0, &pStatusTable) == S_OK)
		//{
		//	if (pStatusTable->SetColumns((LPSPropTagArray)&Columns, 0) == S_OK)
		//	{
		//		while (pStatusTable->QueryRows(1, 0, &pRows) == S_OK)
		//		{
		//			//if (pRows->cRows != 1) FreeProws(pRows);
		//			//else if (pRows->aRow[0].lpProps[1].Value.ul == MAPI_SUBSYSTEM)
		//			//{
		//			//	
		//			if (pRows->aRow->lpProps[1].ulPropTag == PR_SERVICE_UID)
		//			{
		//				// Configuring PST Message Store
		//				ULONG count = 0;
		//				const ULONG nMAXProps = 2;
		//				SPropValue   rgval[nMAXProps];

		//				rgval[count].ulPropTag = PR_DISPLAY_NAME_W;
		//				rgval[count].Value.lpszW = const_cast<wchar_t*>(storeDisplayName.c_str());
		//				++count;

		//				rgval[count].ulPropTag = PR_PST_PATH;
		//				rgval[count].Value.lpszA = const_cast<char*>(path.c_str());
		//				++count;

		//				hr = m_pSvcAdmin->ConfigureMsgService(&m_MsgStoreUID, 0, 0, count, rgval);

		//				if (FAILED(hr))
		//				{
		//					throw std::runtime_error("Can't configure PST message store (ConfigureMsgService)");
		//				}

		//				FreeProws(pRows);
		//			}
		//			else
		//			{
		//				FreeProws(pRows);
		//				continue;
		//			}
		//		}
		//	}
		//	pStatusTable->Release();
		//}

		COM::AutoPtr<SRowSet, RowEraser> prows;
		COM::AutoPtr<SRowSet, RowEraser> pRows;

		enum { iSvcName, iSvcUID, iEnrtyID, cptaSvc };

		// We don't need to worry about checking all the columns; there should
		// only be one since we just created the profile and added one service.
		SizedSPropTagArray(cptaSvc, ptaSvc) = { cptaSvc, { PR_SERVICE_NAME, PR_SERVICE_UID, PR_ENTRYID } };
		hr = HrQueryAllRows(m_ptblSvc, (LPSPropTagArray)&ptaSvc, NULL, NULL, 0, &pRows);
		if (FAILED(hr))
		{
			throw std::runtime_error("HrQueryAllRows failed");
		}

		int cRows = pRows->cRows;

		for (LPSRow pRow = pRows->aRow; pRow < pRows->aRow + cRows; ++pRow)
		{
			if (pRow->lpProps[iSvcUID].ulPropTag != PR_SERVICE_UID)
			{
				throw std::exception("Wrong property tag for service ID");
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
				++count;
				long thang = PR_PST_PW_SZ_OLD;
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
							throw std::runtime_error("Failed to logon - Incorrect password");
						}
						else
						{
							throw std::runtime_error("Failed to logon - Unknown reason");
						}
					}
					else
					{
						throw std::runtime_error("Can't configure PST message store (ConfigureMsgService)");
					}
				}
			}
		}

		hr = MAPILogonEx(0, m_profileName, NULL, MAPI_NEW_SESSION | MAPI_EXTENDED | MAPI_NO_MAIL | MAPI_TIMEOUT_SHORT, &m_pses);
		if (FAILED(hr))
		{
			throw std::runtime_error("MAPILogonEx failed");
		}

		MapiTable ptable;
		hr = m_pses->GetMsgStoresTable(0, &ptable);
		if (FAILED(hr))
		{
			throw std::runtime_error("GetMsgStoresTable failed");
		}

		
	}


	PSTHandler::~PSTHandler()
	{

	}
}