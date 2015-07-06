#ifndef _AXSONEHANDLER_H_
#define _AXSONEHANDLER_H_

#pragma once

#include <wx/string.h>
#include <wx/msw/registry.h>

#include <memory>

class wxEvtHandler;

namespace AOReconstruct
{
	// This class facilitates disabling AxsOne in Outlook
	class AxsOneHandler
	{
	public:
		AxsOneHandler() { };
		~AxsOneHandler() { };

		bool IsAxsOneDisabled();
		void DisableAxsOne();

	private:

		static const wxString aoCacheRegLocation;

	};
}

#endif