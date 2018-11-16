//
//  Graph.hpp
//  AlternativeTransport-MLaToA2018
//
//  Created by Никита Исаенко on 15/11/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#ifndef Graph_hpp
#define Graph_hpp

#include <iostream>
#include <vector>
#include <fstream>

#include "../Essentials/Base.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::vector;

namespace at
{
    struct Vertex;
    struct Edge
    {
        Vertex* to{ nullptr };
        double weight{ 0 };
        bool out{ false };
        bool in{ false };
        
        Edge(Vertex* to, double weight, bool out = true, bool in = true) : to(to), weight(weight), out(out), in(in) { }
    };

    struct Vertex
    {
        vector<Edge*> edges; // max amount of edges = 18.446.744.073.709.551.615 (~2 * 10^19)
        double dijekstraWeight{ std::numeric_limits<double>::infinity() };
        bool dijekstraOut{ false };
        
        ~Vertex();
        void Link(Vertex* to, double weight, bool out = true, bool in = true);
        void Sync(Vertex* to, double weight, bool out = true, bool in = true);
        Edge* Connection(Vertex* to);
        
        inline std::vector<Edge*>::iterator begin() noexcept { return edges.begin(); }
        inline std::vector<Edge*>::const_iterator cbegin() const noexcept { return edges.cbegin(); }
        inline std::vector<Edge*>::iterator end() noexcept { return edges.end(); }
        inline std::vector<Edge*>::const_iterator cend() const noexcept { return edges.cend(); }
    };

    struct Graph
    {
        vector<Vertex*> vertexes; // max amount of vertexes = 18.446.744.073.709.551.615 (~2 * 10^19)
        
        ~Graph();
        void Push(Vertex* v);
        Vertex* operator[](int i);
        unsigned long size();
        void PrintHierarchy();
        void Load(const std::wstring& filename);
        void Save(const std::wstring& filename);
        void Dijekstra(Vertex* s, Vertex* t);
        
        inline std::vector<Vertex*>::iterator begin() noexcept { return vertexes.begin(); }
        inline std::vector<Vertex*>::const_iterator cbegin() const noexcept { return vertexes.cbegin(); }
        inline std::vector<Vertex*>::iterator end() noexcept { return vertexes.end(); }
        inline std::vector<Vertex*>::const_iterator cend() const noexcept { return vertexes.cend(); }
    };





    // Можно сначала создавать вершины, делать между ними связи, а потом вставлять их в граф
    /*Vertex* A = new Vertex();
     Vertex* B = new Vertex();
     A->Link(B, 5, true, false);
     B->Link(A, 5, false, true);
     graph.Push(A);
     graph.Push(B);
     
     // А можно сначала вставить вершины в граф, а потом работать с ними (учитывая индексы, конечно)
     graph.Push(new Vertex());
     graph.Push(new Vertex());
     graph[2]->Link(graph[3], 12, true, false);
     graph[3]->Link(graph[2], 12, false, true);
     
     // Задаём ещё связи любым методом
     A->Link(graph[3], 52);
     graph[3]->Link(A, 52);*/

    // С использование функции Sync вместо Link можно сделать сразу двустороннюю связь
    /* A->Sync(B, 5, true, false); */
}

#endif /* Graph_hpp */
