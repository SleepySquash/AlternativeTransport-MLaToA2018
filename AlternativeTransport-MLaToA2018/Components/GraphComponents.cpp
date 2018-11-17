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
            for (auto v : vertexes)
            {
                if (v->visible)
                {
                    float scaledRadius = circle.getRadius(); //* gs::scale*scale;
                    line[0].position = sf::Vector2f{ (x + v->x)*gs::scale*scale + scaledRadius, (y + v->y)*gs::scale*scale + scaledRadius};
                    for (auto e : v->vertex->edges)
                    {
                        if (e->out && e->to->vertexinfo->visible)
                        {
                            line[1].position = sf::Vector2f{ (x + e->to->vertexinfo->x)*gs::scale*scale + scaledRadius,
                                                             (y + e->to->vertexinfo->y)*gs::scale*scale + scaledRadius };
                            window->draw(line, 2, sf::Lines);
                        }
                    }
                    circle.setPosition((x + v->x)*gs::scale*scale, (y + v->y)*gs::scale*scale);
                    window->draw(circle);
                }
            }
        }
        void GraphMap::Resize(unsigned int width, unsigned int height)
        {
            circle.setRadius(pointRadius * gs::scale*scale);
            circle.setOutlineThickness(gs::scale*scale);
        }
        void GraphMap::PollEvent(sf::Event& event)
        {
            if (event.type == sf::Event::MouseWheelScrolled)
            {
                if (event.mouseWheelScroll.delta != 0)
                {
                    if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                    {
                        if (sf::Keyboard::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                            y += 3/scale * event.mouseWheelScroll.delta;
                        else
                        {
                            //float scalePrev = scale;
                            
                            scale += 0.05 * event.mouseWheelScroll.delta;
                            circle.setRadius(pointRadius * gs::scale*scale);
                            circle.setOutlineThickness(gs::scale*scale);
                            
                            //x += (gs::width/2 * (scalePrev - scale));
                            //y += (gs::height/2 * (scalePrev - scale));
                            //x -= ((float)gs::width - (float)gs::width * (scalePrev/scale))/2;
                            //y -= ((float)gs::height - (float)gs::height * (scalePrev/scale))/2;
                        }
                    }
                    else if (event.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel)
                        x += 3/scale * event.mouseWheelScroll.delta;
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
                                                
                                                std::wstring ypos = ParseUntil(line, ' ', pos);
                                                if (ypos.length() != 0)
                                                {
                                                    pos += ypos.length() + 1;
                                                    y = std::atof(utf8(ypos).c_str());
                                                    
                                                    std::wstring zpos = ParseUntil(line, ' ', pos);
                                                    if (zpos.length() != 0)
                                                    {
                                                        if (zpos[0] == L'"')
                                                        {
                                                            
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
        
    }
}
