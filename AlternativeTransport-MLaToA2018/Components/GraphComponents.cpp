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
        }
        void GraphMap::Init()
        {
            circle.setRadius(pointRadius * gs::scale);
            circle.setFillColor(sf::Color::White);
            circle.setOutlineColor(sf::Color::Black);
            circle.setOutlineThickness(gs::scale);
            
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
        }
        void GraphMap::Destroy()
        {
            for (auto v : vertexes)
                delete v;
        }
        void GraphMap::Update(const sf::Time& elapsedTime)
        {
            
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
            if (wasHighlighted)
                circle.setFillColor(sf::Color::White);
            if (fontLoaded)
            {
                if (panelVisible)
                    window->draw(panelShape);
                window->draw(text);
                
                int yy = info_yy;
                sf::String string;
                
                string = L"Вершин: "; string += std::to_wstring(vertexes.size());
                info.setPosition(info.getPosition().x, yy);
                info.setString(string); window->draw(info);
                yy += info.getLocalBounds().height + 2*gs::scale;
                
                string = L"Масштаб: "; string += std::to_wstring(scale);
                info.setPosition(info.getPosition().x, yy);
                info.setString(string); window->draw(info);
                yy += info.getLocalBounds().height + 2*gs::scale;
                
                string = L"Дейкстра: "; string += std::to_wstring(dijekstraWeight);
                info.setPosition(info.getPosition().x, yy);
                info.setString(string); window->draw(info);
                yy += info.getLocalBounds().height + 2*gs::scale;
            }
        }
        void GraphMap::Resize(unsigned int width, unsigned int height)
        {
            circle.setRadius(pointRadius * gs::scale*scale);
            circle.setOutlineThickness(gs::scale*scale);
            
            text.setCharacterSize(30 * gs::scale);
            text.setOutlineThickness(gs::scale);
            
            info.setCharacterSize(20 * gs::scale);
            info.setOutlineThickness(gs::scale);
            info_yy = gs::height/10 + text.getLocalBounds().height + 10*gs::scale;
            //info_xx = gs::width - text.getLocalBounds().width - gs::width/10 - 10*gs::scale;
            info.setPosition(gs::width - text.getLocalBounds().width - gs::width/10 - 20*gs::scale, info_yy);
            
            sprite.setPosition(x*gs::scale*scale, y*gs::scale*scale);
            sprite.setScale(gs::scale*scale*imageScale, gs::scale*scale*imageScale);
            
            float posx = gs::width - text.getLocalBounds().width - gs::width/10 - 2*20*gs::scale;
            text.setPosition((float)(gs::width - (text.getLocalBounds().width + gs::width/10 + 2*20*gs::scale)/2 - text.getLocalBounds().width/2), gs::height/14);
            
            panelShape.setSize({(float)(gs::width - posx), (float)(gs::height)});
            panelShape.setPosition(posx, 0);
        }
        void GraphMap::PollEvent(sf::Event& event)
        {
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                if (sf::Keyboard::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    Vertex* vertex = new Vertex();
                    VertexInfo* vinfo = new VertexInfo(vertex);
                    vertex->vertexinfo = vinfo;
                    vinfo->x = -x + event.mouseButton.x/(gs::scale * scale) - pointRadius;
                    vinfo->y = -y + event.mouseButton.y/(gs::scale * scale) - pointRadius;
                    vinfo->visible = true;
                    
                    //vertex->Sync(graph->vertexes[0], 10);
                    
                    graph->vertexes.push_back(vertex);
                    vertexes.push_back(vinfo);
                }
                else if (sf::Keyboard::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    bool found{ false };
                    float mx = -x + event.mouseButton.x/(gs::scale * scale) - pointRadius;
                    float my = -y + event.mouseButton.y/(gs::scale * scale) - pointRadius;
                    for (unsigned long i = vertexes.size() - 1; i >= 0 && !found; --i)
                    {
                        if (mx > vertexes[i]->x - pointRadius && mx < vertexes[i]->x + pointRadius &&
                            my > vertexes[i]->y - pointRadius && my < vertexes[i]->y + pointRadius)
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
                else if (sf::Keyboard::Keyboard::isKeyPressed(sf::Keyboard::S))
                {
                    bool found{ false };
                    float mx = -x + event.mouseButton.x/(gs::scale * scale) - pointRadius;
                    float my = -y + event.mouseButton.y/(gs::scale * scale) - pointRadius;
                    for (unsigned long i = vertexes.size() - 1; i >= 0 && !found; --i)
                    {
                        if (mx > vertexes[i]->x - pointRadius && mx < vertexes[i]->x + pointRadius &&
                            my > vertexes[i]->y - pointRadius && my < vertexes[i]->y + pointRadius)
                        {
                            dijekstraWeight = std::numeric_limits<double>::infinity();
                            if (source == vertexes[i]) {
                                source->highlighted = false;
                                source = nullptr;
                            } else if (source == nullptr) {
                                source = vertexes[i];
                                source->highlighted = true;
                            } else {
                                dijekstraWeight = graph->Dijekstra(source->vertex, vertexes[i]->vertex);
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
                else if (sf::Keyboard::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
                {
                    float mx = -x + event.mouseButton.x/(gs::scale * scale) - pointRadius;
                    float my = -y + event.mouseButton.y/(gs::scale * scale) - pointRadius;
                    for (unsigned long i = vertexes.size() - 1; i >= 0; --i)
                    {
                        if (mx > vertexes[i]->x - pointRadius && mx < vertexes[i]->x + pointRadius &&
                            my > vertexes[i]->y - pointRadius && my < vertexes[i]->y + pointRadius)
                        {
                            graph->Remove(vertexes[i]->vertex);
                            delete vertexes[i];
                            vertexes.erase(vertexes.begin() + i);
                            
                            event = sf::Event();
                            break;
                        }
                        if (i == 0) break;
                    }
                }
            }
            else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::R)
            {
                x = (rightBorderX - leftBorderX) / (scale * 2);
                y = (bottomBorderY - topBorderY) / (scale * 2);
            }
            else if (event.type == sf::Event::MouseWheelScrolled)
            {
                if (event.mouseWheelScroll.delta != 0)
                {
                    if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                    {
                        if (sf::Keyboard::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                            y += 6/scale * event.mouseWheelScroll.delta;
                        else
                        {
                            float scalePrev = scale;
                            
                            scale += (scale < 0.6) ? 0.06*scale * event.mouseWheelScroll.delta : 0.05 * event.mouseWheelScroll.delta;
                            if (scale < 0.05) scale = 0.05;
                            circle.setRadius(pointRadius * gs::scale*scale);
                            circle.setOutlineThickness(gs::scale*scale);
                            
                            x -= (gs::width/(gs::scale*scale) - gs::width/(gs::scale*scale) * (scalePrev/scale))/2;
                            y -= (gs::height/(gs::scale*scale) - gs::height/(gs::scale*scale) * (scalePrev/scale))/2;
                            sprite.setScale(gs::scale*scale*imageScale, gs::scale*scale*imageScale);
                        }
                    }
                    else if (event.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel)
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
        
    }
}
