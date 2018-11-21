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
            bool highlighted{ false };
            VertexInfo(Vertex* vertex) : vertex(vertex) { }
            VertexInfo(double x, double y, Vertex* vertex) : x(x), y(y), vertex(vertex) { }
        };
        class GraphMap : public Component
        {
        private:
            VertexInfo* source{ nullptr };
            
            sf::Text text;
            sf::Text info;
            int info_yy{ 0 }, info_xx{ 0 };
            bool fontLoaded{ false };
            
            sf::RectangleShape panelShape;
            bool panelVisible{ true };
            
            sf::Image image;
            sf::Texture texture;
            sf::Sprite sprite;
            float imageScale = 1.f;
            
            sf::CircleShape circle;
            sf::Vertex line[2] =
            {
                sf::Vertex({0, 0}, sf::Color::Red),
                sf::Vertex({10, 10}, sf::Color::Red)
            };
            
            bool controlPressed{ false };
            
            double dijekstraWeight{ std::numeric_limits<double>::infinity() };
            float leftBorderX{ 0 }, rightBorderX{ 0 }, topBorderY{ 0 }, bottomBorderY{ 0 };
            
        public:
            Graph* graph{ nullptr };
            vector<VertexInfo*> vertexes;
            
            float pointRadius{ 5 }, scale { 1.f };
            float x{ 0 }, y{ 0 };
            
            GraphMap(Graph* graph);
            void Init() override;
            void Destroy() override;
            void Update(const sf::Time& elapsedTime) override;
            void Draw(sf::RenderWindow* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            void PollEvent(sf::Event& event) override;
            void Load();
            void Load(const std::wstring& filename);
            void Save(const std::wstring& filename);
            void Clear();
        };
    }
}

#endif /* GraphComponents_hpp */
