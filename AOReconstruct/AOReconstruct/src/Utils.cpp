#include "Utils.h"
#include "Windows.h"

namespace AOReconstruct
{
	namespace Utils
	{
		std::wstring string2wstring(const std::string &str)
		{
			if (str.empty())
				return L"";
			std::vector<wchar_t> res(str.size() * 2); //it should be enough

			int size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), &res.at(0), (int)res.size());
			if (size)
			{
				return std::wstring(&res.at(0), size);
			}
			return L"";
		}

		std::vector<std::wstring> split(const std::wstring& str, const std::wstring& sep)
		{
			std::vector<std::wstring> ret;
			for (size_t pos = 0; pos <= str.size();)
			{
				size_t new_pos = str.find(sep, pos);
				if (std::string::npos == new_pos)
					new_pos = str.size();
				ret.push_back(str.substr(pos, new_pos - pos));
				pos = new_pos + sep.size();
			}
			return ret;
		}
	}
}