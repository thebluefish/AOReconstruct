#include <mapiutil.h>
#include <vector>
#include "FolderImpl.h"
#include "PstStorage.h"
#include "Utils.h"

namespace PstWriter
{
    FolderImpl::FolderImpl(){}

    FolderImpl::~FolderImpl()
    {
        for (std::map<std::wstring,FolderImpl*>::iterator it = m_lists.begin(); it != m_lists.end(); ++it)
            delete it->second;
    }

    std::wstring FolderImpl::GetName()
    {
        return m_name;
    }

    void FolderImpl::AddChild(FolderImpl * child)
    {
        m_lists.insert(std::pair<std::wstring,FolderImpl*>(child->GetName(), child));
    }

    MapiFolder& FolderImpl::GetFolder()
    {
        return m_folder;
    }

    FolderImpl* FolderImpl::FindChild(const std::wstring& path)
    {   
        if(path.empty() || m_lists.empty())
            return 0;

        std::vector<std::wstring> dirs = utils::split(path, L"\\");
        FolderImpl* curFld = this;
        for(size_t i = 0; i < dirs.size(); ++i)
        {
            if (dirs[i].size() == 0)
                continue;
            if(curFld == this)
            {
                std::map<std::wstring,FolderImpl*>::const_iterator it = m_lists.find(dirs[i]);
                if(it != m_lists.end())	
                    curFld = it->second;
                else 
                    return 0;
            }
            else
            {
                FolderImpl* newFld = 0;
                newFld = curFld->FindChild(dirs[i]);
                if(newFld) 
                    curFld = newFld;
                else 
                    return 0;
            }
        }
        return curFld;
    }

    FolderImpl* FolderImpl::CreateFolder(CPstStorage* store, FolderImpl* root, const std::wstring &path)
    {   
        if(!store || !root) 
            return 0;   

        std::vector<std::wstring> dirs = utils::split(path, L"\\");
        FolderImpl* curFld = root;

        for(size_t i = 0; i < dirs.size(); ++i)
        { 
            if (dirs[i].size() == 0)
                continue;
            FolderImpl* p = 0;
            p = curFld->FindChild(dirs[i]);
            if(p)
                curFld = p;
            else
            {
                p = CreateMapiFolder(store->GetMsgStore(), curFld ,dirs[i]);
                if(p) 
                {                   
                    curFld->AddChild(p);
                    curFld = p;
                }
                else
                    return 0;
            }
        }
        return curFld;
    }

    FolderImpl* FolderImpl::CreateMapiFolder(IMsgStore* pIMsgStore, FolderImpl* parent, const std::wstring& folderName)
    {
        MapiFolder pIMAPISubFolder;
        HRESULT  hr =S_OK;    

        // create subfolder
        hr = parent->GetFolder()->CreateFolder(FOLDER_GENERIC,(LPTSTR)folderName.c_str(),NULL,NULL,1|OPEN_IF_EXISTS | MAPI_UNICODE,&pIMAPISubFolder);
        if(FAILED(hr)) 
            throw std::exception("Can't create/open folder");

        pIMAPISubFolder->SaveChanges(KEEP_OPEN_READWRITE);
        std::auto_ptr<FolderImpl> fldNew(new FolderImpl());    
        fldNew->m_folder.reset(pIMAPISubFolder.release());
        fldNew->m_name = folderName;
        return fldNew.release();
    }

    FolderImpl* FolderImpl::CreateRootFolder(CPstStorage * store, const std::wstring& rootFolder) 
    {
        COM::AutoPtr<SPropValue,MapiEraser> pVal_EID;  
        HRESULT hr = ::HrGetOneProp(store->GetMsgStore(),PR_IPM_SUBTREE_ENTRYID,&pVal_EID);

        std::vector<std::wstring> dirs = utils::split(rootFolder, L"\\");

        // Open the IPM subtree folder
        ULONG ulObjType = 0;
        MapiFolder currentFolder;
        hr = store->GetMsgStore()->OpenEntry(pVal_EID->Value.bin.cb,(LPENTRYID)pVal_EID->Value.bin.lpb,NULL,0x10,&ulObjType,reinterpret_cast<IUnknown**>(&currentFolder));
        if(FAILED(hr)) 
            return 0;

        std::auto_ptr<FolderImpl> root(new FolderImpl());
        for (size_t i = 0; i < dirs.size(); ++i)
        {
            if (dirs[i].size() == 0)
                continue;
            MapiFolder mapiRoot;

            hr = currentFolder->CreateFolder(FOLDER_GENERIC,(LPTSTR)dirs[i].c_str(),NULL,NULL,1|OPEN_IF_EXISTS | MAPI_UNICODE,&mapiRoot);

            if(FAILED(hr)) 
                throw std::exception("Can't create root folder");
            currentFolder.reset(mapiRoot.release());
            root->m_name = dirs[i];
        }
        currentFolder->SaveChanges(KEEP_OPEN_READWRITE);
        root->m_folder.reset(currentFolder.release());
        return root.release();
    }
}