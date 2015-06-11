#include "PstStorage.h"
#include <mapitags.h>
#include <mapiutil.h>
#include <Mapidefs.h>
#include <mspst.h>

namespace PstWriter
{
    CPstStorage::CPstStorage(const std::string &path, const std::wstring &storeDisplayName)
    {
        if(path.empty())
            throw std::runtime_error("File name is empty");

        memset(&m_IPF, 0, sizeof(m_IPF));
        HRESULT hr = NOERROR;

        // Get a profile administration object.
        hr = MAPIAdminProfiles(0, &m_pIProfAdmin);
        if (FAILED(hr))
        {                
            throw std::runtime_error("Can't AdminProfiles");
        }

        DWORD dwStart = GetTickCount();
        sprintf_s( m_profileName, 100,  "Temp%lX", dwStart );

        // Creating profile
        (void) m_pIProfAdmin->DeleteProfile( (LPTSTR)&m_profileName, 0 );
        hr = m_pIProfAdmin->CreateProfile( (LPTSTR)&m_profileName, NULL, 0, 0 );
        if (FAILED(hr))
        {                
            throw std::runtime_error("Can't CreateProfile");
        }

        // Get a service administration object.
        hr = m_pIProfAdmin->AdminServices( (LPTSTR)&m_profileName, 0, 0, 0, &m_pSvcAdmin);
        if (FAILED(hr))
        {                
            throw std::runtime_error("Can't AdminServices");
        }
        char *pszMsgService="";

        if (!FAILED(hr = m_pSvcAdmin->CreateMsgService( "MSUPST MS", "MAPI Pst Msg Store", 0, 0 )))
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

        COM::AutoPtr<SRowSet,RowEraser> prows;        
        COM::AutoPtr<SRowSet,RowEraser> pRows;

        enum {iSvcName, iSvcUID, iEnrtyID, cptaSvc};

        // We don't need to worry about checking all the columns; there should
        // only be one since we just created the profile and added one service.
        SizedSPropTagArray (cptaSvc, ptaSvc) = { cptaSvc, { PR_SERVICE_NAME, PR_SERVICE_UID, PR_ENTRYID} };
        hr = HrQueryAllRows(m_ptblSvc, (LPSPropTagArray)&ptaSvc, NULL, NULL, 0, &pRows);
        if (FAILED(hr))
        {                
            throw std::runtime_error("HrQueryAllRows failed");
        }

        int cRows = pRows->cRows;

        for(LPSRow pRow = pRows->aRow; pRow < pRows->aRow + cRows; ++pRow)
        {
            if (pRow->lpProps[iSvcUID].ulPropTag != PR_SERVICE_UID)
            {                    
                throw std::exception("Wrong property tag for service ID");
            }
            m_MsgStoreUID = *((LPMAPIUID)pRow->lpProps[iSvcUID].Value.bin.lpb);
            if(strcmp(pRow->lpProps[iSvcName].Value.lpszA, pszMsgService) == 0)
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

                hr = m_pSvcAdmin->ConfigureMsgService( &m_MsgStoreUID, 0, 0, count, rgval);

                if (FAILED(hr))
                {
                    throw std::runtime_error("Can't configure PST message store (ConfigureMsgService)");
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

        SizedSPropTagArray(3, columns) = { 3, { PR_DEFAULT_STORE, PR_ENTRYID, PR_DISPLAY_NAME } };

        hr = HrQueryAllRows(ptable, (LPSPropTagArray) &columns, NULL/*&restDefStore*/, NULL, 0, &prows);

        IMsgStore * tmpPtr = 0; 
        hr = m_pses->OpenMsgStore(0, // window handle
            prows->aRow[0].lpProps[1].Value.bin.cb, // cbEid,    // # of bytes in entry ID
            (LPENTRYID)prows->aRow[0].lpProps[1].Value.bin.lpb, // lpEid,    // pointer to entry ID
            NULL,   // interface ID pointer
            /*0x80*/ MDB_WRITE | MAPI_DEFERRED_ERRORS | MAPI_BEST_ACCESS | MDB_NO_MAIL,  // flags
            &tmpPtr); // output ptr to store
        if (FAILED(hr))
        {
            throw std::runtime_error("OpenMsgStore failed");
        }
        m_pIMsgStore.reset(tmpPtr);
    }
    CPstStorage::~CPstStorage()
    {
        if (m_pses.get())
        {
            m_pses->Logoff(0, 0, 0);
        }
        if (m_pIProfAdmin.get()) 
        {
            m_pIProfAdmin->DeleteProfile( m_profileName, 0 );
        }
    }
}