#pragma once

#include <vector>
#include <windows.h>

namespace PstWriter
{
    class ILog
    {
    public:
        virtual ~ILog(){}
        virtual void OnError(const std::wstring &text) = 0;
    };
}