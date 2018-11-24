//
//  main.cpp
//  AlternativeTransport-MLaToA2018
//
//  Created by Никита Исаенко on 07/11/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#ifdef __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
        #include <SFML/Main.hpp>
    #endif
#endif

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "Essentials/ResourcePath.hpp"
#include "Essentials/Base.hpp"
#include "Engine/StaticMethods.hpp"
#include "Engine/EntitySystem.hpp"

#include "Graph/Graph.hpp"
#include "Components/EssentialComponents.hpp"
#include "Components/GraphComponents.hpp"

using namespace at;

int main()
{
#ifdef _WIN32 
	std::system("chcp 1251");
#endif

    sf::ContextSettings settings;
    settings.antialiasingLevel = 0;
#ifdef SFML_SYSTEM_IOS
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "AlternativeTransport-MLaToA2018", sf::Style::Default);
#else
    #ifdef SFML_SYSTEM_ANDROID
        sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "AlternativeTransport-MLaToA2018", sf::Style::Default);
    #else
    sf::RenderWindow window(sf::VideoMode(sf::VideoMode::getDesktopMode().width >= 1280 ? 1280 : sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height >= 880 ? 800 : sf::VideoMode::getDesktopMode().height - 80), "AlternativeTransport-MLaToA2018", sf::Style::Default, settings);
    #endif
#endif
    gs::window = &window;
    gs::width = window.getSize().x;
    gs::height = window.getSize().y;
    
    window.setFramerateLimit(gs::framerateLimit);
    window.setVerticalSyncEnabled(gs::isVerticalSyncEnabled);
    
    sf::Image icon;
    if (!icon.loadFromFile(resourcePath() + "Data/Images/icon.png"))
        return EXIT_FAILURE;
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    
    
    
    Graph graph;
    graph.Load(utf16(resourcePath()) + L"Data/in.txt");
    
    
    
    EntitySystem system;
    
    ///----------------------------------------------------------
    /// \brief Entity to hold graph and map that depends on it
    ///----------------------------------------------------------
    Entity* Elizabeth = system.AddEntity();
    GraphComponents::GraphMap* graphmap;
    {
        graphmap = Elizabeth->AddComponent<GraphComponents::GraphMap>(&graph);
    }
    
    ///----------------------------------------------------------
    /// \brief Entity to hold essential components
    ///
    /// Entity holds components like always-on debug UI layer, system's components and other essential stuff.
    /// It also may hold components like NovelComponent that runs the novel, cuz it has to be essential component.
    ///
    ///----------------------------------------------------------
    Entity* Shimakaze = system.AddEntity();
    {
        Shimakaze->AddComponent<EssentialComponents::DebugComponent>("Update 0 build 1");
    }
    
    
    sf::Clock clock;
    window.setActive();
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::MouseButtonPressed:
                    system.PollEvent(event);
                    break;
                    
                case sf::Event::KeyPressed:
                    switch (event.key.code)
                    {
                        case sf::Keyboard::Escape:
                            //window.close();
                            break;
                        default:
                            break;
                    }
                    break;
                    
                case sf::Event::KeyReleased:
                    switch (event.key.code)
                    {
                        case sf::Keyboard::Num0:
                        case sf::Keyboard::R:
                            system.PollEvent(event);
                            break;
                        case sf::Keyboard::C:
                            graphmap->Clear();
                            break;
                        case sf::Keyboard::L:
                            graphmap->Clear();
                            graphmap->Load(utf16(resourcePath()) + L"Data/out.txt");
                            break;
                        case sf::Keyboard::K:
                            graphmap->Save(graph.filePath);
                            break;
                        default:
                            break;
                    }
                    break;
            
                case sf::Event::Resized:
                    window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
                    gs::width = event.size.width; gs::height = event.size.height;
                    gs::scale = ((float)event.size.width/1280 > (float)event.size.height/800) ? (float)event.size.width/1280 : (float)event.size.height/800;
                    system.Resize(event.size.width, event.size.height);
                    break;
                    
                case sf::Event::MouseWheelScrolled:
                    system.PollEvent(event);
                    break;
                    
                default:
                    break;
            }
        }
        
        system.Update(clock.restart());
        
        window.clear();
        system.Draw(&window);
        window.display();
    }

    system.Destroy();
    return EXIT_SUCCESS;
}
