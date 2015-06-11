#include <Mapidefs.h>
#include "PstWriter.h"
#include "PstStorage.h"
#include "FolderImpl.h"
#include "Utils.h"

namespace PstWriter
{
    class MapiInitializer
    {
    public:
        static void Init()
        {
            ++m_refCounter;
            if (m_refCounter == 1)
            {
                MAPIINIT init = { 0, MAPI_MULTITHREAD_NOTIFICATIONS};    
                HRESULT hr = MAPIInitialize(&init);
                if(hr != S_OK)
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

    CPstWriter::CPstWriter(const std::string &path, ILog * log) : m_log(log)
    {
        MapiInitializer::Init();
        m_storage.reset(new CPstStorage(path, L"Pst Store"));
    }
    CPstWriter::~CPstWriter()
    {
        m_root.reset();
        m_storage.reset();
        MapiInitializer::Uninit();
    }
    bool CPstWriter::WriteMessage(CPstMessage * msg, const std::wstring &path)
    {
        try
        {
            HRESULT hr = NOERROR;
            COM::AutoPtr<::IMessage,ComEraser> pMessage;

            if(!m_root.get())
                m_root.reset(FolderImpl::CreateRootFolder(m_storage.get(), L"Export folder"));

            FolderImpl* fld = m_root->FindChild(path);
            if(!fld)
            {
                fld = FolderImpl::CreateFolder(m_storage.get(), m_root.get(), path);
            }            
            hr = fld->GetFolder()->CreateMessage(0,0,&pMessage);
            if (FAILED(hr))
            {
                WriteLog(L"CreateMessage failed");
                return false;
            }            
            hr = WriteMessageData(msg, pMessage.get());
            if (FAILED(hr))
            {
                WriteLog(L"Errors while writing message data occured");
            }
            hr = fld->GetFolder()->SaveChanges(KEEP_OPEN_READWRITE);
            if (FAILED(hr))
            {
                WriteLog(L"Errors while saving changes");
            }
            return  hr == NOERROR;
        }
        catch(const std::exception &ex)
        {
            WriteLog(utils::string2wstring(ex.what()));
        }
        catch(...)
        {
            WriteLog(L"Unknown exception occured");
        }
        return false;
    }

    long CPstWriter::WriteMessageData(CPstMessage *pMsg, IMessage *pMessage)
    {   
        HRESULT hr = NOERROR;

        hr = WriteMessageProps(pMsg, pMessage);
        if (FAILED(hr))
        {
            WriteLog(L"WriteMessageDates.WriteMessageProps failed");
        }
        hr = WriteMessageDates(pMsg, pMessage);
        if (FAILED(hr))
        {
            WriteLog(L"WriteMessageDates.WriteMessageDates failed");
        }
        hr = WriteMessageRecipients(pMsg, pMessage);
        if (FAILED(hr))
        {
            WriteLog(L"WriteMessageDates.WriteMessageRecipients failed");
        }
        hr = WriteMessageBodies(pMsg, pMessage);
        if (FAILED(hr))
        {
            WriteLog(L"WriteMessageDates.WriteMessageBodies failed");
        }

        hr = pMessage->SaveChanges(FORCE_SAVE);
        if (FAILED(hr))
        {
            WriteLog(L"WriteMessageDates.SaveChanges failed");
        }
        return hr;
    }

    long CPstWriter::WriteMessageProps(CPstMessage * pMsg, IMessage * pMessage)
    {
        std::vector<SPropValue> propArray;
        SPropValue currentProp;

        currentProp.ulPropTag = PR_MESSAGE_CLASS_W; 
        currentProp.Value.lpszW = (LPWSTR)pMsg->szMsgClass.c_str();
        propArray.push_back(currentProp);

        // message subject            
        std::wstring subjectPrefix = pMsg->subjectPrefix;
        std::wstring normalizedSubject = pMsg->normalizedSubject;
        std::wstring subject = subjectPrefix + normalizedSubject;

        if ( !subjectPrefix.empty() )
        {
            currentProp.ulPropTag = PR_SUBJECT_PREFIX_W;
            currentProp.Value.lpszW = (LPWSTR)subjectPrefix.c_str();
            propArray.push_back(currentProp);
        }

        if ( !normalizedSubject.empty() ) {
            currentProp.ulPropTag = PR_NORMALIZED_SUBJECT_W;
            currentProp.Value.lpszW = (LPWSTR)normalizedSubject.c_str();
            propArray.push_back(currentProp);
        }

        if ( !subject.empty() ) {
            currentProp.ulPropTag = PR_SUBJECT_W;
            currentProp.Value.lpszW = (LPWSTR)subject.c_str();
            propArray.push_back(currentProp);
        }

        //Message flags
        currentProp.ulPropTag = PR_MESSAGE_FLAGS;
        currentProp.Value.ul = pMsg->flags;
        propArray.push_back(currentProp);
        currentProp.ulPropTag = PR_IMPORTANCE;
        currentProp.Value.ul = pMsg->importance;
        propArray.push_back(currentProp);
        currentProp.ulPropTag = PR_PRIORITY;
        currentProp.Value.ul = pMsg->priority;
        propArray.push_back(currentProp);
        currentProp.ulPropTag = PR_SENSITIVITY;
        currentProp.Value.ul = pMsg->sensitivity;
        propArray.push_back(currentProp);

        // sender name
        if (!pMsg->szSenderName.empty()) 
        {
            currentProp.ulPropTag = PR_SENDER_NAME_W;
            currentProp.Value.lpszW = (LPWSTR)pMsg->szSenderName.c_str();
            propArray.push_back(currentProp);
            currentProp.ulPropTag = PR_SENT_REPRESENTING_NAME_W;
            currentProp.Value.lpszW = (LPWSTR)pMsg->szSenderName.c_str();
            propArray.push_back(currentProp);
        }

        // sender e-mail
        if (!pMsg->szSenderEMail.empty())
        {
            currentProp.ulPropTag = PR_SENDER_EMAIL_ADDRESS_W;
            currentProp.Value.lpszW = (LPWSTR)pMsg->szSenderEMail.c_str();
            propArray.push_back(currentProp);
            currentProp.ulPropTag = PR_SENT_REPRESENTING_EMAIL_ADDRESS_W;
            currentProp.Value.lpszW = (LPWSTR)pMsg->szSenderEMail.c_str();
            propArray.push_back(currentProp);
        }          

        if (propArray.size() != 0)
        {
            COM::AutoPtr<SPropProblemArray,MapiEraser> Problems;
            return pMessage->SetProps((ULONG)propArray.size(), &propArray.at(0), &Problems); 
        }
        return S_OK;
    }

    long CPstWriter::WriteMessageDates(CPstMessage * pMsg, IMessage * pMessage)
    {
        HRESULT hr = S_OK;

        std::vector<SPropValue> propArray;
        SPropValue currentProp;

        if ( (pMsg->ftCreationTime.dwHighDateTime | pMsg->ftCreationTime.dwLowDateTime) != 0 )
        {
            currentProp.ulPropTag      = PR_CREATION_TIME;
            currentProp.Value.ft       = pMsg->ftCreationTime;
            propArray.push_back(currentProp);
        }
        if ( (pMsg->ftReceivedTime.dwHighDateTime | pMsg->ftReceivedTime.dwLowDateTime) != 0 )
        {
            currentProp.ulPropTag      = PR_MESSAGE_DELIVERY_TIME;
            currentProp.Value.ft       = pMsg->ftReceivedTime;
            propArray.push_back(currentProp);
        }
        if ( (pMsg->ftSentTime.dwHighDateTime | pMsg->ftSentTime.dwLowDateTime) != 0 )
        {
            currentProp.ulPropTag      = PR_CLIENT_SUBMIT_TIME;
            currentProp.Value.ft       = pMsg->ftSentTime;
            propArray.push_back(currentProp);
        }
        if ( (pMsg->ftLastModificationTime.dwHighDateTime | pMsg->ftLastModificationTime.dwLowDateTime) != 0 )
        {
            currentProp.ulPropTag      = PR_LAST_MODIFICATION_TIME;
            currentProp.Value.ft       = pMsg->ftLastModificationTime;
            propArray.push_back(currentProp);
        }        

        if ( (pMsg->ftDeletedOnTime.dwHighDateTime | pMsg->ftDeletedOnTime.dwLowDateTime) != 0 )
        {
            // Save deletion date as named property
            MAPINAMEID name = { 0 };
            name.ulKind = MNID_STRING;

            CLSID DCLSID = { 0x6BEA739E, 0x218F, 0x43f7, 0x22, 0xA1, 0x5B, 0xCF, 0xBA, 0xED, 0x05, 0x24 };
            name.lpguid = &DCLSID;
            name.Kind.lpwstrName = L"PR_DELETED_ON";

            COM::AutoPtr<SPropTagArray,MapiEraser> TagArray;
            LPMAPINAMEID pName = (MAPINAMEID*)&name;
            hr = pMessage->GetIDsFromNames( 1, &pName, MAPI_CREATE, &TagArray );
            if (FAILED(hr))
            {
                WriteLog(L"WriteMessageDates.GetIDsFromNames failed");
                return hr;
            }

            currentProp.ulPropTag = CHANGE_PROP_TYPE( TagArray->aulPropTag[0], PT_SYSTIME );
            currentProp.Value.ft = pMsg->ftDeletedOnTime;
            propArray.push_back(currentProp);
        }

        if (propArray.size() > 0)
        {
            hr = pMessage->SetProps((ULONG)propArray.size(), &propArray.at(0), NULL);
        }
        if (FAILED(hr))
        {
            WriteLog(L"WriteMessageDates.SetProps failed");
        }
        return hr;
    }

    long CPstWriter::WriteMessageRecipients(CPstMessage * pMsg, IMessage * pMessage)
    {             
        HRESULT hr = S_OK;
        if ( pMsg->recipients.size() == 0 )
            return hr;

        for( std::vector<RecipientInfo>::const_iterator itRecip = pMsg->recipients.begin(); itRecip != pMsg->recipients.end(); ++itRecip)
        {
            if ( itRecip->address.empty()  && itRecip->name.empty() )
                continue;            

            std::vector<SPropValue> propArray;
            SPropValue currentProp;      

            std::wstring recipientDisplayName = !itRecip->name.empty() ? itRecip->name : itRecip->address;
            if ( !recipientDisplayName.empty() ) 
            {
                currentProp.ulPropTag = PR_DISPLAY_NAME_W;
                currentProp.Value.lpszW = (LPWSTR)recipientDisplayName.c_str();
                propArray.push_back(currentProp);
            }

            if ( !itRecip->address.empty() )
            {
                currentProp.ulPropTag = PR_EMAIL_ADDRESS_W;
                currentProp.Value.lpszW = (LPWSTR)itRecip->address.c_str();
                propArray.push_back(currentProp);
            }

            currentProp.ulPropTag = PR_ADDRTYPE;
            currentProp.Value.lpszA = "SMTP";
            propArray.push_back(currentProp);
            currentProp.ulPropTag = PR_RECIPIENT_TYPE;
            currentProp.Value.ul = itRecip->type;
            propArray.push_back(currentProp);

            ADRLIST adrList = { 0 };
            adrList.cEntries = 1;
            adrList.aEntries[0].rgPropVals = &propArray.front();
            adrList.aEntries[0].ulReserved1 = 0;
            adrList.aEntries[0].cValues = (ULONG)propArray.size();

            hr = pMessage->ModifyRecipients( MODRECIP_ADD, &adrList );
            if ( E_INVALIDARG == hr ) 
            {
                WriteLog(L"Invalid address format");                
            } 
            else
            {
                if (FAILED(hr)) 
                {
                    WriteLog(L"Writing recipient failed");
                }
            }
        }
        return hr;
    }

    long CPstWriter::WriteMessageBodies(CPstMessage * pMsg, IMessage * pMessage)
    {
        HRESULT hr = S_OK;
        // Write message Unicode body
        if (!pMsg->textBody.empty())
        {
            hr = WriteMessageStream(pMessage, (char *)pMsg->textBody.c_str(), pMsg->textBody.size()*2, PR_BODY_W );
            if (FAILED(hr)) 
            {
                WriteLog(L"Failed to write message UNICODE body");
            }
        }
        return hr;
    }

    long CPstWriter::WriteMessageStream (IMessage * pMessage, const char * data, size_t dataSize, unsigned long streamTag)
    {
        if(pMessage == 0)
            return E_FAIL;

        MapiStream pStream;
        HRESULT hResult = pMessage->OpenProperty (streamTag, &IID_IStream, STGM_WRITE, 
            MAPI_CREATE | MAPI_MODIFY, ( LPUNKNOWN* ) & pStream);

        if (hResult != S_OK) 
            return hResult;

        ULONG ulWritten = 0; 
        hResult = pStream->Write (data, (ULONG)dataSize, &ulWritten);

        if (hResult != S_OK) 
            return hResult;

        hResult = pStream->Commit( STGC_DEFAULT );
        return hResult;
    }

    void CPstWriter::WriteLog(const std::wstring &text)
    {
        if (m_log != 0)
        {
            m_log->OnError(text);
        }
    }

}