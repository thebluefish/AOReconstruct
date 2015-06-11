#pragma once

#include <string>
#include <vector>

namespace utils
{
    std::wstring string2wstring(const std::string &str);
    std::vector<std::wstring> split(const std::wstring& str, const std::wstring& sep);
}