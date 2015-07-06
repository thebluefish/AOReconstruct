#include "AxsOneHandler.h"


namespace AOReconstruct
{
	const wxString AxsOneHandler::aoCacheRegLocation = wxT("SOFTWARE\Wow6432Node\Microsoft\Office\Outlook\Addins\Aocache.aocacheAddin");

	bool AxsOneHandler::IsAxsOneDisabled()
	{
		auto aoCacheKey = std::make_shared<wxRegKey>(wxRegKey::HKLM, wxT("SOFTWARE\Wow6432Node\Microsoft\Office\Outlook\Addins\Aocache.aocacheAddin"));
		
		return aoCacheKey->Exists();
	}


	void AxsOneHandler::DisableAxsOne()
	{
		auto aoCacheKey = std::make_shared<wxRegKey>(wxRegKey::HKLM, wxT("SOFTWARE\Wow6432Node\Microsoft\Office\Outlook\Addins\Aocache.aocacheAddin"));

		if (aoCacheKey->Exists())
			return;

		aoCacheKey->DeleteSelf();
	}
}