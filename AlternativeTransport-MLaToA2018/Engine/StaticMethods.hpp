//
//  StaticMethods.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 26/08/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#ifndef StaticMethods_hpp
#define StaticMethods_hpp

#include <iostream>
#include <unordered_map>

#include <codecvt>

#ifdef _WIN32
//std::ifstream in GetFont
#include <fstream>
#endif

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "../Essentials/ResourcePath.hpp"
#ifdef _WIN32
    //#include "../Essentials/versionhelpers.h"
#endif
#include "../Essentials/Base.hpp"

namespace at
{
    class FontCollector
    {
    private:
        FontCollector() { }
        
    public:
        static std::unordered_map<std::wstring, sf::Font*> fonts;
        static sf::Font* GetFont(const std::wstring& fontName);
        static void FreeFonts();
    };
    
    
    
    class GlobalSettings
    {
    public:
        static sf::RenderWindow* window;
        
        static unsigned int width;
        static unsigned int height;
        static float scale;
        
        static int windowPositionOffset;
        
        static bool isVerticalSyncEnabled;
        static int framerateLimit;
        static int framerateNoFocus;
        
        enum class algorithmEnum { Dijkstra, DijkstraOptimized, DoubleDijkstra, ParallelDijkstra, AStar, ArcFlags, ArcFlagsParallel, ContractionHierarchy, HubLabel, Overlay, TableLookup };
        static algorithmEnum algorithm;
    };
    
    std::wstring GetAlgorithmName(const GlobalSettings::algorithmEnum& algorithm);
    
    typedef GlobalSettings gs;
    typedef FontCollector fc;
}

#endif /* StaticMethods_hpp */
