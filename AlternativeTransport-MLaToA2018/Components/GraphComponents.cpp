//
//  GraphComponents.cpp
//  AlternativeTransport-MLaToA2018
//
//  Created by Никита Исаенко on 17/11/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#include "GraphComponents.hpp"

namespace at
{
    namespace GraphComponents
    {
        GraphMap::GraphMap(Graph* graph)
        {
            this->graph = graph;
            if (graph->loaded)
                Load();
            
            if (graph != nullptr)
            {
                using std::placeholders::_1;
                using std::placeholders::_2;
                using std::placeholders::_3;
                using std::placeholders::_4;
                
                algorithmIndex = 1; /// <- Включённый изначально алгоритм по номеру
                
                /// Добавлять по типу:
                ///     make_tuple(  НАЗВАНИЕ,  ФУНКЦИЯ_ПРЕДОБРАБОТКИ,  ПРЕДОБРАБОТКА_ТЯЖЁЛАЯ,  ФУНКЦИЯ_ЗАПРОСА  )
                algorithms.push_back(std::make_tuple(L"Slow Дейкстра",
                                                     std::bind( &Graph::SlowDijkstra_Preprocessing, graph, _1, _2, _3, _4 ),
                                                     false,
                                                     std::bind( &Graph::SlowDijkstra, graph, _1, _2 ),
                                                     std::bind( &Graph::SlowDijkstra_Destroy, graph) ));
                algorithms.push_back(std::make_tuple(L"Дейкстра",
                                                     std::bind( &Graph::DijkstraPreprocessing, graph, _1, _2, _3, _4 ),
                                                     false,
                                                     std::bind( &Graph::Dijkstra, graph, _1, _2 ),
                                                     std::bind( &Graph::DijkstraDestroy, graph) ));
                algorithms.push_back(std::make_tuple(L"External Дейкстра",
                                                     std::bind( &Graph::ExternalDijkstra_Preprocessing, graph, _1, _2, _3, _4 ),
                                                     false,
                                                     std::bind( &Graph::ExternalDijkstra, graph, _1, _2 ),
                                                     std::bind( &Graph::ExternalDijkstra_Unload, graph) ));
                algorithms.push_back(std::make_tuple(L"Таблица",
                                                     std::bind( &Graph::TableLookup_Preprocessing, graph, _1, _2, _3, _4 ),
                                                     true,
                                                     std::bind( &Graph::TableLookup, graph, _1, _2 ),
                                                     std::bind( &Graph::TableLookup_Unload, graph) ));
            }
        }
        void GraphMap::Init()
        {
            line[0] = sf::Vertex({0, 0}, sf::Color(255,255,255,110));
            line[1] = sf::Vertex({10, 10}, sf::Color(255,255,255,110));
            
            circle.setRadius(pointRadius * gs::scale);
            circle.setFillColor(sf::Color::White);
            circle.setOutlineColor(sf::Color::Black);
            circle.setOutlineThickness(gs::scale);
            circle.setPointCount(5);
            
            if ((fontLoaded = (fc::GetFont(L"Arial.ttf") != nullptr)))
                text.setFont(*fc::GetFont(L"Arial.ttf"));
            text.setFillColor(sf::Color::White);
            text.setOutlineColor(sf::Color::Black);
            text.setString(L"Карта");
            
            if ((fontLoaded = (fc::GetFont(L"Arial.ttf") != nullptr)))
                info.setFont(*fc::GetFont(L"Arial.ttf"));
            info.setFillColor(sf::Color::White);
            info.setOutlineColor(sf::Color::Black);
            
            if (image.loadFromFile(resourcePath() + "Data/Images/map1.jpg"))
            {
                texture.loadFromImage(image);
                texture.setSmooth(true);
                sprite.setTexture(texture);
            }
            
            panelShape.setFillColor(sf::Color(0, 0, 0, 160));
            button_Algorithm.SetFont(L"Arial.ttf");
            if (algorithmIndex < algorithms.size())
            {
                button_Algorithm.SetString(L"[" + std::get<0>(algorithms[algorithmIndex]) + L"]");
                if (std::get<2>(algorithms[algorithmIndex]))
                    needsPreprocessing = true;
                else if ((std::get<1>(algorithms[algorithmIndex])) != nullptr)
                {
                    needsPreprocessing = false;
                    std::function<void(unsigned int, Vertex*, Edge*, unsigned long)> func = std::get<1>(algorithms[algorithmIndex]);
                    func(0, nullptr, nullptr, 0);
                } else needsPreprocessing = false;
            }
            button_Algorithm.characterSize = 31;
            button_Algorithm.halign = GUIButton::halignEnum::left;
            
            button_Action.SetFont(L"Arial.ttf");
            button_Action.SetString(L"[Выполнить шаг]");
            button_Action.characterSize = 22;
            
            button_DoPreprocessing.SetFont(L"Arial.ttf");
            button_DoPreprocessing.SetString(L"[Выполнить предобработку]");
            button_DoPreprocessing.characterSize = 22;
            
            button_Graph.SetFont(L"Arial.ttf");
            button_Graph.SetString(L"Автомобильный");
            button_Graph.characterSize = 22;
            button_Graph.halign = at::GUIButton::halignEnum::left;
        }
        void GraphMap::Destroy()
        {
            for (auto v : vertexes)
                delete v;
        }
        void GraphMap::Update(const sf::Time &elapsedTime)
        {
            button_Algorithm.Update(elapsedTime);
            button_Action.Update(elapsedTime);
            button_DoPreprocessing.Update(elapsedTime);
            button_Graph.Update(elapsedTime);
            
            if (contentUpdateTime > 0)
            {
                contentUpdateTime -= elapsedTime.asSeconds();
                if (contentUpdateTime < 0)
                {
                    contentUpdateTime = 0;
                    DrawContent();
                }
            }
            
            if (sf::Mouse::isButtonPressed(sf::Mouse::Right) || sf::Mouse::isButtonPressed(sf::Mouse::Middle))
            {
                x = move_dx + sf::Mouse::getPosition(*gs::window).x/(scale*gs::scale);
                y = move_dy + sf::Mouse::getPosition(*gs::window).y/(scale*gs::scale);
                sprite.setPosition(x*gs::scale*scale, y*gs::scale*scale);
            }
        }
        void GraphMap::Draw(sf::RenderWindow* window)
        {
            window->draw(sprite);
            
            bool wasHighlighted{ false };
            for (auto v : vertexes)
            {
                if (v->visible)
                {
                    //TODO: if the vertex fits the screen
                    {
                        float scaledRadius = circle.getRadius();
                        line[0].position = sf::Vector2f{ (x + v->x)*gs::scale*scale, (y + v->y)*gs::scale*scale};
                        for (auto e : v->vertex->edges)
                            if (e->out && e->to->vertexinfo->visible)
                            {
                                line[1].position = sf::Vector2f{ (x + e->to->vertexinfo->x)*gs::scale*scale,
                                                                 (y + e->to->vertexinfo->y)*gs::scale*scale };
                                if (!e->in) line[1].color = sf::Color(255,90,90,140);
                                window->draw(line, 2, sf::Lines);
                                if (!e->in) line[1].color = sf::Color(255,255,255,140);
                            }
                        circle.setPosition((x + v->x)*gs::scale*scale - scaledRadius, (y + v->y)*gs::scale*scale - scaledRadius);
                        
                        if (v->highlighted) {
                            wasHighlighted = true;
                            circle.setFillColor(sf::Color::Magenta);
                        } else if (wasHighlighted) {
                            wasHighlighted = false;
                            circle.setFillColor(sf::Color::White);
                        }
                        
                        window->draw(circle);
                    }
                }
            }
            if (!graph->shortestPath.empty())
                for (unsigned long i = 0; i < graph->shortestPath.size() - 1; ++i)
                {
                    Vertex* from = graph->shortestPath[i];
                    Vertex* to = graph->shortestPath[i + 1];
                    line[0].position = sf::Vector2f{ (x + from->vertexinfo->x)*gs::scale*scale,
                                                     (y + from->vertexinfo->y)*gs::scale*scale};
                    line[1].position = sf::Vector2f{ (x + to->vertexinfo->x)*gs::scale*scale ,
                                                     (y + to->vertexinfo->y)*gs::scale*scale };
                    line[0].color = sf::Color::Green; line[1].color = sf::Color::Green;
                    window->draw(line, 2, sf::Lines);
                    line[0].color = sf::Color(255,255,255,110); line[1].color = sf::Color(255,255,255,110);
                }
            if (wasHighlighted)
                circle.setFillColor(sf::Color::White);
            
            if (fontLoaded)
            {
                if (panelVisible)
                    window->draw(panelShape);
                window->draw(text);
                
                int yy = info_yy;
                sf::String string;
                
                if (screen == screenEnum::main)
                {
                    string = L"Вершин: "; string += std::to_wstring(vertexes.size());
                    info.setPosition(info.getPosition().x, yy);
                    info.setString(string); window->draw(info);
                    yy += info.getLocalBounds().height + 2*gs::scale;
                    
                    string = L"Масштаб: "; string += std::to_wstring(scale);
                    info.setPosition(info.getPosition().x, yy);
                    info.setString(string); window->draw(info);
                    yy += (info.getLocalBounds().height + 2*gs::scale) * 2;
                    
                    button_Algorithm.SetPosition(gs::width - panelShape.getSize().x + 6*gs::scale, yy);
                    button_Algorithm.Draw(window);
                    yy += button_Algorithm.text.getLocalBounds().height + 5*gs::scale;
                    
                    string = L"Расстояние: "; if (distance == std::numeric_limits<double>::infinity())
                        string += L"inf"; else string += std::to_wstring((unsigned long)distance);
                    info.setPosition(info.getPosition().x, yy);
                    info.setString(string); window->draw(info);
                    yy += info.getLocalBounds().height + 2*gs::scale;
                    
                    string = L"Время: "; string += L"TODO";
                    info.setPosition(info.getPosition().x, yy);
                    info.setString(string); window->draw(info);
                    yy += info.getLocalBounds().height + 2*gs::scale;
                    
                    string = L"(выполнено за: "; string += std::to_wstring(clockRuntime); string += L")";
                    info.setPosition(info.getPosition().x, yy);
                    info.setString(string); window->draw(info);
                    yy += (info.getLocalBounds().height + 2*gs::scale)*1.7;
                    
                    
                    string = L"Предобработка"; info.setCharacterSize(32 * gs::scale);
                    info.setPosition(info.getPosition().x, yy);
                    info.setString(string); window->draw(info);
                    yy += info.getLocalBounds().height + 5*gs::scale;  info.setCharacterSize(20 * gs::scale);
                    
                    button_DoPreprocessing.SetPosition(gs::width - panelShape.getSize().x/2, yy);
                    button_DoPreprocessing.Draw(window);
                    yy += button_DoPreprocessing.text.getLocalBounds().height + 4*gs::scale;
                    
                    if (needsPreprocessing) {
                        string = L"!!! Требуется предобработка !!!";
                        info.setFillColor(sf::Color::Red);
                    } else {
                        string = L"Предобработка выполнена";
                        info.setFillColor(sf::Color::Green); }
                    info.setPosition(info.getPosition().x, yy);
                    info.setString(string); window->draw(info); info.setFillColor(sf::Color::White);
                    yy += info.getLocalBounds().height + 2*gs::scale;
                    
                    string = L"(выполнено за: "; string += std::to_wstring(clockPretime); string += L")";
                    info.setPosition(info.getPosition().x, yy);
                    info.setString(string); window->draw(info);
                    yy += (info.getLocalBounds().height + 2*gs::scale)*2;
                    
                    
                    string = L"Пошаговое управление"; info.setCharacterSize(25 * gs::scale);
                    info.setPosition(info.getPosition().x, yy);
                    info.setString(string); window->draw(info);
                    yy += info.getLocalBounds().height + 2*gs::scale;  info.setCharacterSize(20 * gs::scale);
                    
                    button_Action.SetPosition(gs::width - panelShape.getSize().x/2, yy);
                    button_Action.Draw(window);
                    yy += (button_Action.text.getLocalBounds().height + 2*gs::scale)*2;
                    
                    string = L"Персонализация"; info.setCharacterSize(30 * gs::scale);
                    info.setPosition(info.getPosition().x, yy);
                    info.setString(string); window->draw(info);
                    yy += info.getLocalBounds().height + 10*gs::scale; info.setCharacterSize(20 * gs::scale);
                    
                    string = L"Скорость: "; string += L"TODO";
                    info.setPosition(info.getPosition().x, yy);
                    info.setString(string); window->draw(info);
                    yy += info.getLocalBounds().height + 2*gs::scale;
                }
                else if (screen == screenEnum::choosing_algorithm)
                {
                    for (auto a : algorithms)
                    {
                        button_Algorithm.SetString(std::get<0>(a));
                        button_Algorithm.SetPosition(gs::width - panelShape.getSize().x + 10, yy);
                        button_Algorithm.Draw(window);
                        yy += button_Algorithm.text.getLocalBounds().height + 2*gs::scale;
                    }
                    
                    button_Algorithm.SetString(L"Назад");
                    button_Algorithm.SetPosition(gs::width - panelShape.getSize().x/2 - button_Algorithm.text.getLocalBounds().width/2, gs::height - gs::height/7);
                    button_Algorithm.Draw(window);
                }
                
                
                int xx = button_Graph_xx;
                button_Graph.SetString(L"Автомобильный");
                button_Graph.SetPosition(xx, button_Graph.text.getPosition().y);
                button_Graph.Draw(window);
                xx += button_Graph.text.getLocalBounds().width + 15*gs::scale;
                
                button_Graph.SetString(L"Пеший");
                button_Graph.SetPosition(xx, button_Graph.text.getPosition().y);
                button_Graph.Draw(window);
                xx += button_Graph.text.getLocalBounds().width + 15*gs::scale;
                
                button_Graph.SetString(L"Общественный");
                button_Graph.SetPosition(xx, button_Graph.text.getPosition().y);
                button_Graph.Draw(window);
                xx += button_Graph.text.getLocalBounds().width + 15*gs::scale;
                
                button_Graph.SetString(L"ЛЭТИ(временно)");
                button_Graph.SetPosition(xx, button_Graph.text.getPosition().y);
                button_Graph.Draw(window);
            }
        }
        void GraphMap::Resize(unsigned int width, unsigned int height)
        {
            circle.setRadius(pointRadius * gs::scale*scale);
            circle.setOutlineThickness(gs::scale*scale);
            
            text.setCharacterSize(45 * gs::scale);
            text.setOutlineThickness(gs::scale);
            
            sprite.setPosition(x*gs::scale*scale, y*gs::scale*scale);
            sprite.setScale(gs::scale*scale*imageScale, gs::scale*scale*imageScale);
            
            float posx = gs::width - text.getLocalBounds().width - gs::width/10 - 2*20*gs::scale;
            text.setPosition((float)(gs::width - (text.getLocalBounds().width + gs::width/10 + 2*20*gs::scale)/2 - text.getLocalBounds().width/2), gs::height/14);
            
            info.setCharacterSize(20 * gs::scale);
            info.setOutlineThickness(gs::scale);
            info_yy = gs::height/10 + text.getLocalBounds().height + 2*gs::scale;
            info.setPosition(posx + 10*gs::scale, info_yy);
            
            panelShape.setSize({(float)(gs::width - posx), (float)(gs::height)});
            panelShape.setPosition(posx, 0);
            button_Algorithm.Resize(width, height);
            button_Action.Resize(width, height);
            button_Graph.Resize(width, height);
            button_DoPreprocessing.Resize(width, height);
            
            if (button_Algorithm.text.getLocalBounds().width + 20*gs::scale > panelShape.getSize().x)
                button_Algorithm.text.setScale((float)panelShape.getSize().x / (button_Algorithm.text.getLocalBounds().width + 20*gs::scale), 1);
            else
                button_Algorithm.text.setScale(1, 1);
            
            button_Graph.SetString(L"Автомобильный"); int xx = button_Graph.text.getLocalBounds().width + 15*gs::scale;
            button_Graph.SetString(L"Пеший"); xx += button_Graph.text.getLocalBounds().width + 15*gs::scale;
            button_Graph.SetString(L"Общественный"); xx += button_Graph.text.getLocalBounds().width + 15*gs::scale;
            button_Graph.SetString(L"ЛЭТИ(временно)"); xx += button_Graph.text.getLocalBounds().width;
            button_Graph_xx = gs::width/2 - xx/2;
            
            if (contentLoaded)
            {
                content.create((abs(rightBorderX - leftBorderX) > image.getSize().x ?
                                abs(rightBorderX - leftBorderX) : (float)image.getSize().x) * gs::scale * scale,
                               (abs(bottomBorderY - topBorderY) > image.getSize().y ?
                                abs(bottomBorderY - topBorderY) : (float)image.getSize().y) * gs::scale * scale);
                DrawContent();
            }
            
            overlayContent.create(width, height);
            DrawOverlay();
        }
        void GraphMap::PollEvent(sf::Event& event)
        {
            if (event.type == sf::Event::MouseButtonPressed &&
                (event.mouseButton.button == sf::Mouse::Right || event.mouseButton.button == sf::Mouse::Middle))
            {
                move_dx = x - event.mouseButton.x/(scale*gs::scale);
                move_dy = y - event.mouseButton.y/(scale*gs::scale);
            }
            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift))
                {
                    Vertex* vertex = new Vertex();
                    VertexInfo* vinfo = new VertexInfo(vertex);
                    vertex->vertexinfo = vinfo;
                    
                    vinfo->x = -x + event.mouseButton.x/(gs::scale * scale);
                    vinfo->y = -y + event.mouseButton.y/(gs::scale * scale);
                    vinfo->visible = true;
                    
                    graph->Push(vertex);
                    vertexes.push_back(vinfo);
                    
                    if (std::get<2>(algorithms[algorithmIndex]))
                        { needsPreprocessing = true; clockPretime = 0; }
                    else if ((std::get<1>(algorithms[algorithmIndex])) != nullptr)
                    {
                        std::function<void(unsigned int, Vertex*, Edge*, unsigned long)> func = std::get<1>(algorithms[algorithmIndex]);
                        clock_t beg = clock();
                        func(1, vertex, nullptr, graph->vertexes.size() - 1);
                        clock_t end = clock();
                        clockPretime = end - beg;
                    }
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    bool found{ false };
                    float mx = -x + event.mouseButton.x/(gs::scale * scale);
                    float my = -y + event.mouseButton.y/(gs::scale * scale);
                    float actualRadius = ((scale >= 2.5) ? 2.5/scale : 1.f) * pointRadius;
                    for (unsigned long i = vertexes.size() - 1; i >= 0 && !found; --i)
                    {
                        if (mx > vertexes[i]->x - actualRadius && mx < vertexes[i]->x + actualRadius &&
                            my > vertexes[i]->y - actualRadius && my < vertexes[i]->y + actualRadius)
                        {
                            if (source == nullptr) {
                                source = vertexes[i];
                                source->highlighted = true;
                            } else if (source == vertexes[i]) {
                                source->highlighted = false;
                                source = nullptr;
                            } else {
                                vertexes[i]->vertex->Sync(source->vertex,
                                                          sqrt(pow(vertexes[i]->x - source->x, 2) + pow(vertexes[i]->y - source->y, 2)));
                                if (std::get<2>(algorithms[algorithmIndex]))
                                    { needsPreprocessing = true; clockPretime = 0; }
                                else if ((std::get<1>(algorithms[algorithmIndex])) != nullptr)
                                {
                                    std::function<void(unsigned int, Vertex*, Edge*, unsigned long)> func = std::get<1>(algorithms[algorithmIndex]);
                                    clock_t beg = clock();
                                    func(2, source->vertex, vertexes[i]->vertex->Connection(source->vertex), i);
                                    clock_t end = clock();
                                    clockPretime = end - beg;
                                }
                                source->highlighted = false;
                                source = nullptr;
                            }
                            found = true;
                            event = sf::Event();
                        }
                        if (i == 0) break;
                    }
                    if (!found)
                    {
                        if (source != nullptr)
                            source->highlighted = false;
                        source = nullptr;
                    }
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
                {
                    float mx = -x + event.mouseButton.x/(gs::scale * scale);
                    float my = -y + event.mouseButton.y/(gs::scale * scale);
                    float actualRadius = ((scale >= 2.5) ? 2.5/scale : 1.f) * pointRadius;
                    for (unsigned long i = vertexes.size() - 1; i >= 0; --i)
                    {
                        if (mx > vertexes[i]->x - actualRadius && mx < vertexes[i]->x + actualRadius &&
                            my > vertexes[i]->y - actualRadius && my < vertexes[i]->y + actualRadius)
                        {
                            Vertex* temp = vertexes[i]->vertex;
                            
                            if (source == vertexes[i])
                                source = nullptr;
                            graph->Remove(vertexes[i]->vertex);
                            delete vertexes[i];
                            vertexes.erase(vertexes.begin() + i);
                            
                            if (std::get<2>(algorithms[algorithmIndex]))
                                { needsPreprocessing = true; clockPretime = 0; }
                            else if ((std::get<1>(algorithms[algorithmIndex])) != nullptr)
                            {
                                std::function<void(unsigned int, Vertex*, Edge*, unsigned long)> func = std::get<1>(algorithms[algorithmIndex]);
                                clock_t beg = clock();
                                func(3, temp, nullptr, i);
                                clock_t end = clock();
                                clockPretime = end - beg;
                            }
                            
                            event = sf::Event();
                            break;
                        }
                        if (i == 0) break;
                    }
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                {
                    if (!graph->shortestPath.empty())
                    {
                        for (auto v : graph->shortestPath)
                            v->vertexinfo->highlighted = false;
                        graph->shortestPath.clear();
                    }
                    
                    bool found{ false };
                    if (!needsPreprocessing)
                    {
                        float mx = -x + event.mouseButton.x/(gs::scale * scale);
                        float my = -y + event.mouseButton.y/(gs::scale * scale);
                        float actualRadius = ((scale >= 2.5) ? 2.5/scale : 1.f) * pointRadius;
                        for (unsigned long i = vertexes.size() - 1; i >= 0 && !found; --i)
                        {
                            if (mx > vertexes[i]->x - actualRadius && mx < vertexes[i]->x + actualRadius &&
                                my > vertexes[i]->y - actualRadius && my < vertexes[i]->y + actualRadius)
                            {
                                distance = std::numeric_limits<double>::infinity();
                                if (source == vertexes[i]) {
                                    source->highlighted = false;
                                    source = nullptr;
                                } else if (source == nullptr) {
                                    source = vertexes[i]; sourcei = i;
                                    source->highlighted = true;
                                } else {
                                    graph->shortestPath.clear();
                                    
                                    clock_t beg = clock();
                                    if ((std::get<3>(algorithms[algorithmIndex])) != nullptr) {
                                        std::function<double(unsigned long, unsigned long)> func = std::get<3>(algorithms[algorithmIndex]);
                                        distance = func(sourcei, i); }
                                    clock_t end = clock();
                                    clockRuntime = end - beg;
                                    
                                    source->highlighted = false;
                                    source = nullptr;
                                    if (!graph->shortestPath.empty())
                                        for (auto v : graph->shortestPath)
                                            v->vertexinfo->highlighted = true;
                                }
                                found = true;
                                event = sf::Event();
                            }
                            if (i == 0) break;
                        }
                    }
                    if (!found)
                    {
                        if (source != nullptr)
                            source->highlighted = false;
                        source = nullptr;
                    }
                }
                else
                {
                    bool algorithmButtonPressed{ false };
                    
                    if (screen == screenEnum::main)
                    {
                        button_Algorithm.PollEvent(event);
                        if (button_Algorithm.IsPressed())
                        {
                            algorithmButtonPressed = true;
                            screen = screenEnum::choosing_algorithm;
                            button_Algorithm.SetCharacterSize(30);
                            button_Algorithm.text.setScale(1, 1);
                            button_Algorithm.halign = GUIButton::halignEnum::left;
                            text.setString(L"Алгор");
                        }
                    }
                    else if (screen == screenEnum::choosing_algorithm)
                    {
                        button_Algorithm.PollEvent(event); bool isWasBack{ true };
                        if (button_Algorithm.IsPressed())
                            algorithmButtonPressed = true;
                        else
                        {
                            int yy = info_yy; isWasBack = false;
                            for (unsigned long i = 0; i < algorithms.size(); ++i)
                            {
                                button_Algorithm.SetString(std::get<0>(algorithms[i]));
                                button_Algorithm.SetPosition(gs::width - panelShape.getSize().x + 10, yy);
                                button_Algorithm.PollEvent(event);
                                if (button_Algorithm.IsPressed())
                                {
                                    algorithmButtonPressed = true;
                                    if (algorithmIndex < algorithms.size() && (std::get<4>(algorithms[algorithmIndex])) != nullptr)
                                    {
                                        std::function<void()> func = std::get<4>(algorithms[algorithmIndex]);
                                        func();
                                    }
                                    algorithmIndex = i;
                                    break;
                                }
                                yy += button_Algorithm.text.getLocalBounds().height + 2*gs::scale;
                            }
                        }
                        
                        if (algorithmButtonPressed)
                        {
                            screen = screenEnum::main;
                            
                            if (algorithmIndex < algorithms.size())
                            {
                                button_Algorithm.SetString(L"[" + std::get<0>(algorithms[algorithmIndex]) + L"]");
                                if (!isWasBack)
                                {
                                    if (!graph->shortestPath.empty()) {
                                        for (auto v : graph->shortestPath)
                                            v->vertexinfo->highlighted = false;
                                        graph->shortestPath.clear();
                                    } graph->shortestPath.clear();
                                    clockPretime = 0; clockRuntime = 0; distance = std::numeric_limits<double>::infinity(); time = distance;
                                    if (std::get<2>(algorithms[algorithmIndex]))
                                        needsPreprocessing = true;
                                    else if ((std::get<1>(algorithms[algorithmIndex])) != nullptr)
                                    {
                                        needsPreprocessing = false;
                                        std::function<void(unsigned int, Vertex*, Edge*, unsigned long)> func = std::get<1>(algorithms[algorithmIndex]);
                                        clock_t beg = clock(); func(0, nullptr, nullptr, 0); clock_t end = clock();
                                        clockPretime = end - beg;
                                    } else needsPreprocessing = false;
                                }
                            }
                            
                            button_Algorithm.SetCharacterSize(31);
                            if (button_Algorithm.text.getLocalBounds().width + 20*gs::scale > panelShape.getSize().x)
                                button_Algorithm.text.setScale((float)panelShape.getSize().x / (button_Algorithm.text.getLocalBounds().width + 20*gs::scale), 1);
                            else
                                button_Algorithm.text.setScale(1, 1);
                            button_Algorithm.halign = GUIButton::halignEnum::left;
                            text.setString(L"Карта");
                        }
                    }
                    if (!algorithmButtonPressed)
                    {
                        button_DoPreprocessing.PollEvent(event);
                        if (button_DoPreprocessing.IsPressed())
                        {
                            if (needsPreprocessing)
                            {
                                if ((std::get<1>(algorithms[algorithmIndex])) != nullptr)
                                {
                                    std::function<void(unsigned int, Vertex*, Edge*, unsigned long)> func = std::get<1>(algorithms[algorithmIndex]);
                                    clock_t beg = clock();
                                    func(0, nullptr, nullptr, 0);
                                    clock_t end = clock();
                                    clockPretime = end - beg;
                                }
                                needsPreprocessing = false;
                            }
                        }
                        else
                        {
                            button_Action.PollEvent(event);
                            if (button_Action.IsPressed())
                                cout << "Pressed" << endl;
                            else
                            {
                                int xx = button_Graph_xx;
                                button_Graph.SetString(L"Автомобильный");
                                button_Graph.SetPosition(xx, button_Graph.text.getPosition().y);
                                button_Graph.PollEvent(event);
                                if (button_Graph.IsPressed())
                                    { Clear(); Load(utf16(resourcePath()) + L"Data/Graph/auto.txt"); }
                                else
                                {
                                    xx += button_Graph.text.getLocalBounds().width + 15*gs::scale;
                                    
                                    button_Graph.SetString(L"Пеший");
                                    button_Graph.SetPosition(xx, button_Graph.text.getPosition().y);
                                    button_Graph.PollEvent(event);
                                    if (button_Graph.IsPressed())
                                        { Clear(); Load(utf16(resourcePath()) + L"Data/Graph/foot.txt"); }
                                    else
                                    {
                                        xx += button_Graph.text.getLocalBounds().width + 15*gs::scale;
                                        
                                        button_Graph.SetString(L"Общественный");
                                        button_Graph.SetPosition(xx, button_Graph.text.getPosition().y);
                                        button_Graph.PollEvent(event);
                                        if (button_Graph.IsPressed())
                                            { Clear(); Load(utf16(resourcePath()) + L"Data/Graph/social.txt"); }
                                        else
                                        {
                                            xx += button_Graph.text.getLocalBounds().width + 15*gs::scale;
                                            
                                            button_Graph.SetString(L"ЛЭТИ(временно)");
                                            button_Graph.SetPosition(xx, button_Graph.text.getPosition().y);
                                            button_Graph.PollEvent(event);
                                            if (button_Graph.IsPressed())
                                                { Clear(); Load(utf16(resourcePath()) + L"Data/Graph/eltech.txt"); }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Num0)
            {
                /// Служебное событие для глобальных корректировок
                /*for (auto v : vertexes)
                {
                    v->x += 10;
                    v->y += 10;
                }*/
            }
            else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::R)
            {
                x = (-rightBorderX + leftBorderX) / (2);
                y = (-bottomBorderY + topBorderY) / (2);
                sprite.setPosition(x*gs::scale*scale, y*gs::scale*scale);
            }
            else if (event.type == sf::Event::MouseWheelScrolled)
            {
                if (event.mouseWheelScroll.delta != 0)
                {
                    if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                    {
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                            y += 6/scale * event.mouseWheelScroll.delta;
                        else
                        {
                            float scalePrev = scale;
                            
                            scale += (scale < 0.6) ? 0.06*scale * event.mouseWheelScroll.delta : 0.05 * event.mouseWheelScroll.delta;
                            if (scale < 0.05) scale = 0.05;
                            if (scale >= 2.5) {
                                circle.setRadius(2.5/scale * pointRadius * gs::scale*scale);
                                circle.setOutlineThickness(2.5/scale * gs::scale*scale);
                            } else {
                                circle.setRadius(pointRadius * gs::scale*scale);
                                circle.setOutlineThickness(gs::scale*scale);
                            }
                            
                            x -= (gs::width/(gs::scale*scale) - gs::width/(gs::scale*scale) * (scalePrev/scale))/2;
                            y -= (gs::height/(gs::scale*scale) - gs::height/(gs::scale*scale) * (scalePrev/scale))/2;
                            sprite.setScale(gs::scale*scale*imageScale, gs::scale*scale*imageScale);
                            contentDeltaScale -= scalePrev - scale;
                            contentUpdateTime = 0.2f;
                        }
                    }
                    else if (event.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                        x += 6/scale * event.mouseWheelScroll.delta;
                    
                    sprite.setPosition(x*gs::scale*scale, y*gs::scale*scale);
                }
            }
        }
        void GraphMap::Load()
        {
            if (graph != nullptr && graph->loaded)
            {
                std::wifstream wif;
#ifdef _WIN32
                wif.open(graph->filePath);
#else
                wif.open(utf8(graph->filePath));
#endif
                wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
                
                if (wif.is_open())
                {
                    std::wstring line;
                    
                    std::getline(wif, line);
                    unsigned long size = std::atol(utf8(line).c_str());
                    if (size != 0)
                    {
                        vertexes.resize(size);
                        for (int i = 0; i < size; ++i)
                        {
                            vertexes[i] = new VertexInfo(graph->vertexes[i]);
                            graph->vertexes[i]->vertexinfo = vertexes[i];
                        }
                        while (!wif.eof())
                        {
                            std::getline(wif, line);
                            if (line.length() != 0)
                            {
                                if (line[0] == L'v' || line[0] == L'm') // info related to MAP, like x-y-z pos etc.
                                {
                                    unsigned int pos = 1;
                                    std::wstring num = ParseUntil(line, ' ', pos);
                                    pos += num.length() + 1;
                                    
                                    unsigned long index = 0;
                                    double x = 0;
                                    double y = 0;
                                    float z = 0;
                                    std::wstring label = L"";
                                    
                                    if (num.length() != 0)
                                    {
                                        index = std::atol(utf8(num).c_str());
                                        if (index < size)
                                        {
                                            std::wstring xpos = ParseUntil(line, ' ', pos);
                                            if (xpos.length() != 0)
                                            {
                                                pos += xpos.length() + 1;
                                                x = std::atof(utf8(xpos).c_str());
                                                if (x < leftBorderX)
                                                    leftBorderX = x;
                                                if (x > rightBorderX)
                                                    rightBorderX = x;
                                                
                                                std::wstring ypos = ParseUntil(line, ' ', pos);
                                                if (ypos.length() != 0)
                                                {
                                                    pos += ypos.length() + 1;
                                                    y = std::atof(utf8(ypos).c_str());
                                                    if (y < topBorderY)
                                                        topBorderY = y;
                                                    if (y > bottomBorderY)
                                                        bottomBorderY = y;
                                                    
                                                    std::wstring zpos = ParseUntil(line, ' ', pos);
                                                    if (zpos.length() != 0)
                                                    {
                                                        if (zpos[0] == L'"')
                                                        {
                                                            // TODO: Label
                                                        }
                                                        else
                                                        {
                                                            pos += zpos.length() + 1;
                                                            z = std::atof(utf8(zpos).c_str());
                                                        }
                                                    }
                                                }
                                                
                                                vertexes[index]->x = x;
                                                vertexes[index]->y = y;
                                                //vertexes[index]->z = z;
                                                //vertexes[index]->label = label;
                                            }
                                            vertexes[index]->visible = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    if (!graph->shortestPath.empty()) {
                        for (auto v : graph->shortestPath)
                            v->vertexinfo->highlighted = false;
                        graph->shortestPath.clear();
                    } graph->shortestPath.clear();
                    
                    clockPretime = 0; clockRuntime = 0; distance = std::numeric_limits<double>::infinity(); time = distance;
                    if (algorithmIndex < algorithms.size())
                    {
                        if (std::get<2>(algorithms[algorithmIndex]))
                            needsPreprocessing = true;
                        else if ((std::get<1>(algorithms[algorithmIndex])) != nullptr)
                        {
                            needsPreprocessing = false;
                            std::function<void(unsigned int, Vertex*, Edge*, unsigned long)> func = std::get<1>(algorithms[algorithmIndex]);
                            clock_t beg = clock(); func(0, nullptr, nullptr, 0); clock_t end = clock();
                            clockPretime = end - beg;
                        } else needsPreprocessing = false;
                    }
                    
                    contentLoaded = false; // <- true
                    if (contentLoaded)
                    {
                        content.create((abs(rightBorderX - leftBorderX) > image.getSize().x ?
                                        abs(rightBorderX - leftBorderX) : (float)image.getSize().x) * gs::scale * scale,
                                       (abs(bottomBorderY - topBorderY) > image.getSize().y ?
                                        abs(bottomBorderY - topBorderY) : (float)image.getSize().y) * gs::scale * scale);
                        DrawContent();
                    }
                }
            }
        }
        void GraphMap::Load(const std::wstring& filename)
        {
            if (!graph->loaded)
                graph->Load(filename);
            
            if (graph->loaded)
                Load();
        }
        void GraphMap::Save(const std::wstring& filename)
        {
            if (graph != nullptr)
            {
                graph->Save(filename);
                std::wofstream wof;
#ifdef _WIN32
                wof.open(filename, std::ios_base::app);
#else
                wof.open(utf8(filename), std::ios_base::app);
#endif
                wof.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
                
                if (wof.is_open())
                {
                    wof << endl;
                    for (unsigned long i = 0; i < vertexes.size(); ++i)
                        if (vertexes[i]->visible)
                            wof << L'm' << i << L' ' << vertexes[i]->x << L' ' << vertexes[i]->y << endl;
                }
                wof.close();
            }
        }
        void GraphMap::Clear()
        {
            if (graph != nullptr && graph->loaded)
                graph->Clear();
            for (int i = 0; i < vertexes.size(); ++i) { delete vertexes[i]; vertexes[i] = nullptr; }
            vertexes.clear();
        }
        
        
        
        
        
        
        void GraphMap::Draw1(sf::RenderWindow* window)
        {
            sf::Sprite sprite(content.getTexture());
            sprite.setPosition(x*contentDeltaScale, y*contentDeltaScale);
            sprite.setScale(contentDeltaScale, contentDeltaScale);
            window->draw(sprite);
            window->draw(sf::Sprite(overlayContent.getTexture()));
            
            if (fontLoaded)
            {
                int yy = info_yy;
                sf::String string;
                
                string = L"Вершин: "; string += std::to_wstring(vertexes.size());
                info.setPosition(info.getPosition().x, yy);
                info.setString(string); window->draw(info);
                yy += info.getLocalBounds().height + 2*gs::scale;
                
                string = L"Масштаб: "; string += std::to_wstring(scale);
                info.setPosition(info.getPosition().x, yy);
                info.setString(string); window->draw(info);
                yy += (info.getLocalBounds().height + 2*gs::scale) * 2;
                
                button_Algorithm.SetPosition(gs::width - panelShape.getSize().x/2, yy);
                button_Algorithm.Draw(window);
                yy += button_Algorithm.text.getLocalBounds().height + 5*gs::scale;
                
                string = L"Расстояние: "; string += std::to_wstring(distance);
                info.setPosition(info.getPosition().x, yy);
                info.setString(string); window->draw(info);
                yy += info.getLocalBounds().height + 2*gs::scale;
                
                string = L"(выполнено за: "; string += std::to_wstring(clockRuntime); string += L")";
                info.setPosition(info.getPosition().x, yy);
                info.setString(string); window->draw(info);
                yy += (info.getLocalBounds().height + 2*gs::scale)*2;
                
                string = L"Пошаговое управление";
                info.setPosition(info.getPosition().x, yy);
                info.setString(string); window->draw(info);
                yy += info.getLocalBounds().height + 2*gs::scale;
                
                button_Action.SetString(L"Назад");
                button_Action.SetPosition(gs::width - panelShape.getSize().x/2, yy);
                button_Action.Draw(window);
                yy += button_Action.text.getLocalBounds().height + 2*gs::scale;
                
                button_Action.SetString(L"Вперёд");
                button_Action.SetPosition(gs::width - panelShape.getSize().x/2, yy);
                button_Action.Draw(window);
                yy += button_Action.text.getLocalBounds().height + 2*gs::scale;
            }
        }
        void GraphMap::DrawContent()
        {
            
        }
        void GraphMap::DrawContent1()
        {
            contentDeltaScale = 1.f;
            content.clear(sf::Color::Transparent);
            
            sf::RenderTexture* window = &content;
            window->draw(sprite);
            
            bool wasHighlighted{ false };
            for (auto v : vertexes)
            {
                if (v->visible)
                {
                    //TODO: if the vertex fits the screen
                    {
                        float scaledRadius = circle.getRadius(); //* gs::scale*scale;
                        line[0].position = sf::Vector2f{ (x + v->x)*gs::scale*scale + scaledRadius, (y + v->y)*gs::scale*scale + scaledRadius};
                        for (auto e : v->vertex->edges)
                            if (e->out && e->to->vertexinfo->visible)
                            {
                                line[1].position = sf::Vector2f{ (x + e->to->vertexinfo->x)*gs::scale*scale + scaledRadius,
                                    (y + e->to->vertexinfo->y)*gs::scale*scale + scaledRadius };
                                window->draw(line, 2, sf::Lines);
                            }
                        circle.setPosition((x + v->x)*gs::scale*scale, (y + v->y)*gs::scale*scale);
                        
                        if (v->highlighted) {
                            wasHighlighted = true;
                            circle.setFillColor(sf::Color::Magenta);
                        } else if (wasHighlighted) {
                            wasHighlighted = false;
                            circle.setFillColor(sf::Color::White);
                        }
                        
                        window->draw(circle);
                    }
                }
            }
            if (!graph->shortestPath.empty())
                for (unsigned long i = 0; i < graph->shortestPath.size() - 1; ++i)
                {
                    Vertex* from = graph->shortestPath[i];
                    Vertex* to = graph->shortestPath[i + 1];
                    float scaledRadius = circle.getRadius();
                    line[0].position = sf::Vector2f{ (x + from->vertexinfo->x)*gs::scale*scale + scaledRadius,
                        (y + from->vertexinfo->y)*gs::scale*scale + scaledRadius};
                    line[1].position = sf::Vector2f{ (x + to->vertexinfo->x)*gs::scale*scale + scaledRadius,
                        (y + to->vertexinfo->y)*gs::scale*scale + scaledRadius };
                    line[0].color = sf::Color::Green; line[1].color = sf::Color::Green;
                    window->draw(line, 2, sf::Lines);
                    line[0].color = sf::Color::Red; line[1].color = sf::Color::Red;
                }
            if (wasHighlighted)
                circle.setFillColor(sf::Color::White);
            
            content.display();
        }
        void GraphMap::DrawOverlay()
        {
            
        }
        void GraphMap::DrawOverlay1()
        {
            overlayContent.clear(sf::Color::Transparent);
            
            sf::RenderTexture* window = &overlayContent;
            if (fontLoaded)
            {
                if (panelVisible)
                    window->draw(panelShape);
                window->draw(text);
                
                int yy = info_yy;
                sf::String string;
                
                string = L"Вершин: "; string += std::to_wstring(vertexes.size());
                info.setString(string);
                yy += info.getLocalBounds().height + 2*gs::scale;
                
                string = L"Масштаб: "; string += std::to_wstring(scale);
                info.setString(string);
                yy += (info.getLocalBounds().height + 2*gs::scale) * 2;
                
                button_Algorithm.SetPosition(gs::width - panelShape.getSize().x/2, yy);
                yy += button_Algorithm.text.getLocalBounds().height + 5*gs::scale;
                
                string = L"Расстояние: "; string += std::to_wstring(distance);
                info.setString(string);
                yy += info.getLocalBounds().height + 2*gs::scale;
                
                string = L"(выполнено за: "; string += std::to_wstring(clockRuntime); string += L")";
                info.setString(string);
                yy += (info.getLocalBounds().height + 2*gs::scale)*2;
                
                string = L"Пошаговое управление";
                info.setPosition(info.getPosition().x, yy);
                info.setString(string); window->draw(info);
                yy += info.getLocalBounds().height + 2*gs::scale;
                
                button_Action.SetString(L"Назад");
                yy += button_Action.text.getLocalBounds().height + 2*gs::scale;
                
                button_Action.SetString(L"Вперёд");
                yy += button_Action.text.getLocalBounds().height + 2*gs::scale;
            }
            
            overlayContent.display();
        }
    }
}
