#ifndef _FOLDER_IMPL_H_
#define _FOLDER_IMPL_H_

#include <map>
#include "PSTTypes.h"

namespace PstWriter
{
    class CPstStorage;
    class FolderImpl
    {
    public:
        static FolderImpl* CreateFolder(CPstStorage* store, FolderImpl* root, const std::wstring &path);
        static FolderImpl* CreateRootFolder(CPstStorage* store, const std::wstring &rootFolder);
        virtual ~FolderImpl();
        std::wstring GetName();
        void AddChild(FolderImpl* child);
        FolderImpl* FindChild(const std::wstring& path);
        MapiFolder& GetFolder();
    private:
        FolderImpl();
        FolderImpl(const FolderImpl&);
        FolderImpl& operator=(FolderImpl&);
        static FolderImpl* CreateMapiFolder(IMsgStore* pIMsgStore, FolderImpl* parent, const std::wstring& folderName);
    private:
        std::map<std::wstring,FolderImpl*> m_lists;
        std::wstring m_name;
        MapiFolder m_folder;
    };
}

#endif