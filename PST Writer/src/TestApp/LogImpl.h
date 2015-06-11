#pragma once

#include <iostream>
#include "Interfaces.h"

namespace TestApp
{
    class CLogImpl : public PstWriter::ILog
    {
    public:
        void OnError(const std::wstring &text)
        {
            std::wcout<<text<<L"\r\n";
        }
    };
}