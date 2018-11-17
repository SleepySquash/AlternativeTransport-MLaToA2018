//
//  GraphComponents.hpp
//  AlternativeTransport-MLaToA2018
//
//  Created by Никита Исаенко on 17/11/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#ifndef GraphComponents_hpp
#define GraphComponents_hpp

#include <iostream>
#include <vector>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "../Engine/EntitySystem.hpp"
#include "../Engine/StaticMethods.hpp"
#include "../Graph/Graph.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::vector;

namespace at
{
    namespace GraphComponents
    {
        struct VertexInfo
        {
            float x{ 0 }, y{ 0 };
            Vertex* vertex{ nullptr };
            bool visible{ false };
            VertexInfo(Vertex* vertex) : vertex(vertex) { }
            VertexInfo(double x, double y, Vertex* vertex) : x(x), y(y), vertex(vertex) { }
        };
        class GraphMap : public Component
        {
        private:
            vector<VertexInfo*> vertexes;
            Graph* graph{ nullptr };
            
            sf::CircleShape circle;
            sf::Vertex line[2] =
            {
                sf::Vertex({0, 0}, sf::Color::Red),
                sf::Vertex({10, 10}, sf::Color::Red)
            };
            
            bool controlPressed{ false };
            
        public:
            float pointRadius{ 10 }, scale { 1.f };
            float x{ 0 }, y{ 0 };
            
            GraphMap(Graph* graph);
            void Init() override;
            void Destroy() override;
            void Update(const sf::Time& elapsedTime) override;
            void Draw(sf::RenderWindow* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            void PollEvent(sf::Event& event) override;
            void Load();
        };
    }
}

#endif /* GraphComponents_hpp */
