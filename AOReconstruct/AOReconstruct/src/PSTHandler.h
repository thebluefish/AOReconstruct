#ifndef _PSTHANDLER_H_
#define _PSTHANDLER_H_

#pragma once

#include <string>
#include <memory>

#include "PSTTypes.h"

namespace AOReconstruct
{
	class PSTHandler
	{
	public:

	public:
		PSTHandler(const std::string& path, const std::string& password, const std::wstring& storeDisplayName);
		~PSTHandler();


	protected:

		void Initialize();

	protected:
		std::wstring _filePath;

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

#endif