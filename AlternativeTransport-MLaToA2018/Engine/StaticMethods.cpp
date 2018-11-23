//
//  StaticMethods.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 26/08/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#include "StaticMethods.hpp"

namespace at
{
    sf::Font* FontCollector::GetFont(const std::wstring& fontName)
    {
        if (fonts.find(fontName) != fonts.end())
            return fonts.at(fontName);
        else
        {
            sf::Font* font = new sf::Font();
            
            sf::String fullPath = sf::String(resourcePath()) + L"Data/Fonts/" + fontName;
            bool fontLoaded{ false };
#ifdef _WIN32
            //TODO: Fix memory leak
            std::ifstream ifStream(fullPath.toWideString(), std::ios::binary | std::ios::ate);
            if (!ifStream.is_open())
                std::cerr << "Unable to open file: " << fullPath.toAnsiString() << std::endl;
            else
            {
                auto filesize = ifStream.tellg();
                char* fileInMemory = new char[static_cast<unsigned int>(filesize)];
                ifStream.seekg(0, std::ios::beg);
                ifStream.read(fileInMemory, filesize);
                ifStream.close();
                
                fontLoaded = font->loadFromMemory(fileInMemory, filesize);
            }
#else
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            std::string u8str = converter.to_bytes(fullPath.toWideString());
            if (!(fontLoaded = font->loadFromFile(u8str)))
                std::cerr << "Unable to open file: " << fullPath.toAnsiString() << std::endl;
#endif
            
            if (fontLoaded)
            {
                fonts.emplace(fontName, font);
                return font;
            }
            else
                return nullptr;
        }
    }
    void FontCollector::FreeFonts()
    {
        for (const auto& key : fonts)
            if (key.second != nullptr)
                delete key.second;
        fonts.clear();
    }
    std::unordered_map<std::wstring, sf::Font*> FontCollector::fonts = { {L"", nullptr} };
    
    
    
    
    
    
    sf::RenderWindow* gs::window = nullptr;
    
    unsigned int gs::width = 0;
    unsigned int gs::height = 0;
    float gs::scale = 1.f;
    
#ifdef _WIN32
    int gs::windowPositionOffset = IsWindows8OrGreater() ? GetSystemMetrics(SM_CXSIZEFRAME) : 0;
#else
    int gs::windowPositionOffset = 0;
#endif
    
    bool gs::isVerticalSyncEnabled = true;
    int gs::framerateLimit = 120;
    int gs::framerateNoFocus = 30;
    
    gs::algorithmEnum gs::algorithm = algorithmEnum::Dijkstra;
    
    std::wstring GetAlgorithmName(const gs::algorithmEnum& algorithm)
    {
        switch (algorithm)
        {
            case GlobalSettings::algorithmEnum::Dijkstra:
                return L"Дейкстра";
                break;
            case GlobalSettings::algorithmEnum::DijkstraOptimized:
                return L"Дейкстра++";
                break;
            case GlobalSettings::algorithmEnum::DoubleDijkstra:
                return L"Двунаправленный Дейкстра";
                break;
            case GlobalSettings::algorithmEnum::ParallelDijkstra:
                return L"Парарллельный Дейкстра";
                break;
            case GlobalSettings::algorithmEnum::AStar:
                return L"A-звезда";
                break;
            case GlobalSettings::algorithmEnum::ArcFlags:
                return L"Арк флаги";
                break;
            case GlobalSettings::algorithmEnum::ArcFlagsParallel:
                return L"Параллельные Арк флаги";
                break;
            case GlobalSettings::algorithmEnum::ContractionHierarchy:
                return L"Контракционная иерархия";
                break;
            case GlobalSettings::algorithmEnum::HubLabel:
                return L"Метки Хабами";
                break;
            case GlobalSettings::algorithmEnum::Overlay:
                return L"Оверлей граф";
                break;
            case GlobalSettings::algorithmEnum::TableLookup:
                return L"Таблица";
                break;
        }
    }
}
