//
//  Base.hpp
//  AlternativeTransport-MLaToA2018
//
//  Created by Никита Исаенко on 15/11/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#ifndef Base_hpp
#define Base_hpp

#include <iostream>
#include <codecvt>

//FileExists
#include <sys/stat.h>

std::wstring ParseUntil(std::wstring line, const char& until, unsigned int from);
std::string utf8(const std::wstring& wstr);
std::wstring utf16(const std::string& wstr);

std::wstring GetCurrentWorkingDir();
bool FileExists(const std::wstring& path);


#endif /* Base_hpp */
