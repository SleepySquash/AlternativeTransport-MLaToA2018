//
//  EntitySystem.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 26/08/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#include "EntitySystem.hpp"

namespace at
{
    Component::~Component() { }
    void Component::Init() { }
    void Component::Update(const sf::Time&) { }
    void Component::Draw(sf::RenderWindow*) { }
    void Component::Resize(unsigned int, unsigned int) { }
    void Component::PollEvent(sf::Event&) { }
    void Component::Destroy() { }
    void Component::SetEntity(Entity* entity)
    {
        this->entity = entity;
    }
    Entity* Component::GetEntity()
    {
        return entity;
    }
    
    
    Entity::Entity() { }
    Entity::Entity(EntitySystem* system)
    {
        this->system = system;
    }
    void Entity::Update(const sf::Time& elapsedTime)
    {
        if (components.size())
            for (auto c : components)
                c->Update(elapsedTime);
    }
    void Entity::Draw(sf::RenderWindow* window)
    {
        if (components.size())
            for (auto c : components)
                c->Draw(window);
    }
    void Entity::Resize(unsigned int width, unsigned int height)
    {
        if (components.size())
            for (auto c : components)
                c->Resize(width, height);
    }
    void Entity::PollEvent(sf::Event& event)
    {
        if (components.size())
            for (auto c : components)
                c->PollEvent(event);
    }
    void Entity::PopComponent(Component* component)
    {
        std::list<Component*>::iterator it = std::find(components.begin(), components.end(), component);
        if (it != components.end())
        {
            (*it)->Destroy();
            delete (*it);
            components.erase(it);
        }
    }
    void Entity::Destroy()
    {
        if (components.size())
        {
            for (auto it = components.begin(); it != components.end(); ++it)
            {
                (*it)->Destroy();
                delete (*it);
                //components.erase(it);
            }
        }
    }
    void Entity::SetEntitySystem(EntitySystem* system)
    {
        this->system = system;
    }
    
    
    
    EntitySystem::EntitySystem() { }
    void EntitySystem::Update(const sf::Time& elapsedTime)
    {
        if (entities.size())
            for (auto e : entities)
                e->Update(elapsedTime);
    }
    void EntitySystem::Draw(sf::RenderWindow* window)
    {
        if (entities.size())
            for (auto e : entities)
                e->Draw(window);
    }
    void EntitySystem::Resize(unsigned int width, unsigned int height)
    {
        if (entities.size())
            for (auto e : entities)
                e->Resize(width, height);
    }
    void EntitySystem::PollEvent(sf::Event& event)
    {
        if (entities.size())
            for (auto e : entities)
                e->PollEvent(event);
    }
    Entity* EntitySystem::AddEntity()
    {
        Entity* entity = new Entity(this);
        entities.push_back(std::move(entity));
        return entities.back();
    }
    void EntitySystem::PopEntity(Entity* entity)
    {
        std::list<Entity*>::iterator it = std::find(entities.begin(), entities.end(), entity);
        if (it != entities.end())
        {
            (*it)->Destroy();
            delete (*it);
            entities.erase(it);
        }
    }
    void EntitySystem::Destroy()
    {
        if (entities.size())
        {
            for (auto it = entities.begin(); it != entities.end(); ++it)
            {
                (*it)->Destroy();
                delete (*it);
                //entities.erase(it);
            }
        }
    }
}
