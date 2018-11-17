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

#ifdef _WIN32
	#include <direct.h>
	std::wstring GetCurrentWorkingDir(void)
	{
		wchar_t buff[FILENAME_MAX];
		_wgetcwd(buff, FILENAME_MAX);
		std::wstring current_working_dir(buff);
		return current_working_dir + L'\\';
	}
#else
	#include <unistd.h>
	std::wstring GetCurrentWorkingDir(void)
	{
		char buff[FILENAME_MAX];
		getcwd(buff, FILENAME_MAX);
		std::string current_working_dir(buff);

		//TODO: change the way it converts
		std::wstring wcurrent_working_dir(current_working_dir.begin(), current_working_dir.end());

		return wcurrent_working_dir + L'\\';
	}
#endif