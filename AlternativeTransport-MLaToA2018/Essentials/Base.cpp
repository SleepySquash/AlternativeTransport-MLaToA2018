//
//  Base.cpp
//  AlternativeTransport-MLaToA2018
//
//  Created by Никита Исаенко on 15/11/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#include "Base.hpp"

std::wstring ParseUntil(std::wstring line, const char& until, unsigned int from)
{
    std::wstring result = L"";
    bool found{ false };
    
    while (!found && from < line.length())
        if (!(found = (line[from] == until)))
        {
            if (line[from] != 13)
                result += line[from];
            from++;
        }
    
    return result;
}

std::string utf8(const std::wstring& wstr)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(wstr);
}
std::wstring utf16(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.from_bytes(str);
}
