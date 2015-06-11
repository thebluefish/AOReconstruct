#pragma once

#include <string>
#include "PSTTypes.h"

namespace PstWriter
{
    class CPstStorage
    {
    public:
        CPstStorage(const std::string &path, const std::wstring &storeDisplayName);
        ~CPstStorage();
        MsgStore& GetMsgStore() {return m_pIMsgStore;};
    private:
        CPstStorage(const CPstStorage&);
        CPstStorage& operator=(const CPstStorage&);
    private:
        IPF_t m_IPF;
        char m_profileName[100];
        MapiSession m_pses;
        ProfAdmin m_pIProfAdmin;
        MsgServiceAdmin m_pSvcAdmin;        
        MsgStore m_pIMsgStore;        
        MapiTable m_ptblSvc;
        MAPIUID m_MsgStoreUID;
    };
}