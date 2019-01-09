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
#include <functional>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "../Essentials/Base.hpp"
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
            unsigned long sourcei{ 0 };
            
            sf::Text text;
            sf::Text info;
            int info_yy{ 0 }, button_Graph_xx{ 0 };
            bool fontLoaded{ false };
            
            sf::RectangleShape panelShape;
            bool panelVisible{ true };
            GUIButton button_Algorithm;
            GUIButton button_Action;
            GUIButton button_DoPreprocessing;
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
            float imageScale = 1.f / 1.4656;
            
            sf::Text weightInfo;
            sf::CircleShape circle;
            sf::Vertex line[2];/* =  /// Cuz Visual Studio 12 swears when sees that
            {
                sf::Vertex({0, 0}, sf::Color(255,255,255,140)),
                sf::Vertex({10, 10}, sf::Color(255,255,255,140))
            };*/
            sf::Vertex thickline[4];
            sf::Vertex flags[3];
            
            sf::Texture flagTexutre;
            sf::Sprite flagSprite;
            sf::Texture finTexutre;
            sf::Sprite finSprite;
            
            enum class screenEnum { main, choosing_algorithm };
            screenEnum screen = screenEnum::main;
            unsigned long algorithmIndex{ 0 };
            vector<std::tuple<std::wstring, std::function<void(unsigned int, Vertex*, Edge*, unsigned long)>, bool, std::function<double(unsigned long, unsigned long)>, std::function<void()>>> algorithms;
            
            int move_dx{ 0 }, move_dy{ 0 };
            
            double distance{ std::numeric_limits<double>::infinity() };
            double speed{ 0 };
            double time{ 0 };
            
            clock_t clockRuntime{ 0 }, clockPretime{ 0 };
            double timeRuntime{ 0 }, timePretime{ 0 };
            bool needsPreprocessing{ false };
            
            sf::RenderTexture zones;
            sf::Sprite zonesSprite;
            int currentPosZoneX{ 0 }, currentPosZoneY{ 0 }, lastPosZoneX{ 0 }, lastPosZoneY{ 0 };
            
            sf::Color zonesImageLoadingClr{ sf::Color::Black };
            sf::Color zonesZoneShiftClr{ sf::Color::Black };
            
        public:
            Graph* graph{ nullptr };
            vector<VertexInfo*> vertexes;
            
            //std::unordered_map<std::pair<int, int>, std::pair<sf::Texture, sf::Sprite>> sectors;
            bool zonesOn{ false };
            bool zoneResized{ true };
            unsigned char zoneNumberDetailized{ 0 };
            int zoneCountX{ 0 }, zoneCountY{ 0 }, lzoneCountX{ 0 }, lzoneCountY{ 0 };
            int zone{ 64 }, sectorx1{ 0 }, sectory1{ 0 };
            float pointRadius{ 5 }, scale { 1.f };
            float x{ 0 }, y{ 0 }, xright{ 0 }, ybottom{ 0 };
            
            int arcFlagsZonesAxes{ 0 };
            float leftBorderX{ 0 }, rightBorderX{ 0 }, topBorderY{ 0 }, bottomBorderY{ 0 };
            
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
            void LoadZoneImage(const std::wstring& fullPath, const int& i, const int& j);
            void DoZones();
            void ZonesResize(unsigned int width, unsigned int height);
            void ChangeZonesPosition();
            
            void Draw1(sf::RenderWindow* window);
            void DrawContent1();
            void DrawOverlay1();
        };
    }
}

#endif /* GraphComponents_hpp */
