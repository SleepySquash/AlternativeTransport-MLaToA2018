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
#include "../Engine/GUIInterface.hpp"
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
            int info_yy{ 0 }, button_Graph_xx{ 0 };
            bool fontLoaded{ false };
            
            sf::RectangleShape panelShape;
            bool panelVisible{ true };
            GUIButton button_Algorithm;
            GUIButton button_Action;
            GUIButton button_Graph;
            
            // TODO: Make it actually work
            bool contentLoaded{ false };
            float contentUpdateTime{ 0.f };
            float contentDeltaScale{ 1.f };
            sf::RenderTexture content;
            sf::RenderTexture overlayContent;
            
            sf::Image image;
            sf::Texture texture;
            sf::Sprite sprite;
            float imageScale = 1.f;
            
            sf::Text weightInfo;
            sf::CircleShape circle;
            sf::Vertex line[2];/* =  /// Cuz Visual Studio 12 swears when sees that
            {
                sf::Vertex({0, 0}, sf::Color(255,255,255,140)),
                sf::Vertex({10, 10}, sf::Color(255,255,255,140))
            };*/
            
            int move_dx{ 0 }, move_dy{ 0 };
            
            double dijkstraWeight{ std::numeric_limits<double>::infinity() };
            clock_t dijkstraTime{ 0 };
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
            void DrawContent();
            void DrawOverlay();
            void Resize(unsigned int width, unsigned int height) override;
            void PollEvent(sf::Event& event) override;
            void Load();
            void Load(const std::wstring& filename);
            void Save(const std::wstring& filename);
            void Clear();
            
            void Draw1(sf::RenderWindow* window);
            void DrawContent1();
            void DrawOverlay1();
        };
    }
}

#endif /* GraphComponents_hpp */
