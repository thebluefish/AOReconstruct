#pragma once

#include <string>
#include <memory>
#include "Interfaces.h"

struct IMessage;
namespace PstWriter
{
    struct RecipientInfo
    {
        std::wstring name;
        std::wstring address;
        long type;
    };

    class CPstMessage
    {
    public:
        virtual ~CPstMessage(){}
        std::wstring subjectPrefix;
        std::wstring normalizedSubject;
        
        std::vector<RecipientInfo> recipients;

        ULONG flags;

        int  importance;  // 0 - LOW, 1 - NORMAL, 2 - HIGH
        int  priority;    // -1 - NONURGENT, 0 - NORMAL, 1 - URGENT
        int  sensitivity; // 0 - NONE, 1 - PERSONAL, 2 - PRIVATE, 3 - COMPANY_CONFIDENTIAL

        std::wstring szMsgClass;

        std::wstring szSenderName;
        std::wstring szSenderEMail;

        std::wstring szSentRepresentingName;
        std::wstring szSentRepresentingEMail;

        FILETIME ftCreationTime;
        FILETIME ftSentTime;
        FILETIME ftReceivedTime;
        FILETIME ftLastModificationTime;
        FILETIME ftDeletedOnTime;

        std::wstring textBody;

        std::wstring szTo;
        std::wstring szCC;
        std::wstring szBCC;
    };

    class CPstStorage;
    class FolderImpl;
    class CPstWriter
    {
    public:
        CPstWriter(const std::string &path, ILog * log);
        ~CPstWriter();
        bool WriteMessage(CPstMessage * msg, const std::wstring &path);        
    private:
        void WriteLog(const std::wstring &text);
        long WriteMessageData(CPstMessage *pMsg, IMessage *pMessage);
        long WriteMessageStream(IMessage * pMessage, const char * data, size_t dataSize, unsigned long streamTag);
        long WriteMessageProps(CPstMessage * pMsg, IMessage * pMessage);
        long WriteMessageDates(CPstMessage * pMsg, IMessage * pMessage);
        long WriteMessageRecipients(CPstMessage * pMsg, IMessage * pMessage);
        long WriteMessageBodies(CPstMessage * pMsg, IMessage * pMessage);
    private:
        std::wstring m_path;
        ILog * m_log;
        std::auto_ptr<CPstStorage> m_storage;
        std::auto_ptr<FolderImpl> m_root;
    };
}