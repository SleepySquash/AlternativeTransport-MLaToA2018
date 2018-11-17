//
//  EntitySystem.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 26/08/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#ifndef EntitySystem_hpp
#define EntitySystem_hpp

#include <algorithm>
#include <iostream>
#include <list>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "StaticMethods.hpp"

using std::list;

namespace at
{
    class Component;
    class Entity;
    class EntitySystem;
    
    class Component
    {
    private:
        Entity* entity{ nullptr };
        
    public:
        virtual ~Component();
        virtual void Init();
        virtual void Update(const sf::Time&);
        virtual void Draw(sf::RenderWindow*);
        virtual void Resize(unsigned int width, unsigned int height);
        virtual void PollEvent(sf::Event& event);
        virtual void Destroy();
        void SetEntity(Entity* entity);
        Entity* GetEntity();
    };
    
    class Entity
    {
    private:
        EntitySystem* system = nullptr;
        list<Component*> components;
        
    public:
        Entity();
        Entity(EntitySystem* system);
        void Update(const sf::Time& elapsedTime);
        void Draw(sf::RenderWindow* window);
        void Resize(unsigned int width, unsigned int height);
        void PollEvent(sf::Event& event);
        void PopComponent(Component* component);
        void Destroy();
        void SetEntitySystem(EntitySystem* system);
        template<typename T, typename ...Args> T* AddComponent(Args... args)
        {
            T* component = new T(args...);
            
            component->SetEntity(this);
            component->Init();
            component->Resize(gs::width, gs::height);
            
            components.push_back(std::move(component));
            T* t = dynamic_cast<T*>(components.back());
            return t;
        }
    };
    
    class EntitySystem
    {
    private:
        list<Entity*> entities;
        
    public:
        EntitySystem();
        void Update(const sf::Time& elapsedTime);
        void Draw(sf::RenderWindow* window);
        void Resize(unsigned int width, unsigned int height);
        void PollEvent(sf::Event& event);
        Entity* AddEntity();
        void PopEntity(Entity* entity);
        void Destroy();
    };
}

#endif /* EntitySystem_hpp */

