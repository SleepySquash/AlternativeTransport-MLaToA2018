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
#include <fstream>
#include <algorithm>

#include <vector>
#include <deque>
#include <unordered_map>
#include <queue>

#include "../Essentials/Base.hpp"

using std::cin;
using std::cout;
using std::endl;

using std::vector;
using std::deque;
using std::unordered_map;
using std::priority_queue;

using std::pair;
using std::make_pair;

namespace at
{
    namespace GraphComponents
    {
        struct VertexInfo;
    }
    struct Vertex;
    
    struct DataHolder { };
    struct DijkstraHolder : DataHolder
    {
        double weight{ 0 };
        bool out{ false };
        Vertex* previous{ nullptr };
    };
    
    struct Edge
    {
        Vertex* to{ nullptr };
        double weight{ 0 };
        bool out{ false };
        bool in{ false };
        
        DataHolder* data{ nullptr };
        bool savingCompleted{ false };
        
        Edge(Vertex* to, double weight, bool out = true, bool in = true) :
             to(to), weight(weight), out(out), in(in) { }
        
        friend class Vertex;
        friend class Graph;
    };
    
    struct Vertex
    {
        // max amount of edges = 18.446.744.073.709.551.615 (~2 * 10^19)
        vector<Edge*> edges;
        GraphComponents::VertexInfo* vertexinfo{ nullptr };
        DataHolder* data{ nullptr };
        
        ~Vertex();
        void Link(Vertex* to, double weight, bool out = true, bool in = true);
        void Sync(Vertex* to, double weight, bool out = true, bool in = true);
        Edge* Connection(Vertex* to);
        
        inline std::vector<Edge*>::iterator begin() { return edges.begin(); }
        inline std::vector<Edge*>::const_iterator cbegin() const { return edges.cbegin(); }
        inline std::vector<Edge*>::iterator end() { return edges.end(); }
        inline std::vector<Edge*>::const_iterator cend() const { return edges.cend(); }
        
        friend class Edge;
        friend class Graph;
    };

    struct DijkstraOptimizedData
    {
        double weight;
        bool out;
        Vertex* previous;
        
        DijkstraOptimizedData() : weight(std::numeric_limits<double>::infinity()), out(false), previous(nullptr) { }
    };
    struct Graph
    {
        // max amount of vertices = 18.446.744.073.709.551.615 (~2 * 10^19)
        vector<Vertex*> vertices;
        vector<Vertex*> shortestPath;
        
        std::wstring filePath = L"";
        bool loaded{ false };
        
        ~Graph();
        void Push(Vertex* v);
        Vertex* operator[](int i);
        unsigned long size();
        void PrintHierarchy();
        void Remove(Vertex* vertex);
        void Remove(unsigned long index);
        void Load(const std::wstring& filename);
        void Save(const std::wstring& filename);
        void Clear();
        
        
        void SlowDijkstra_Preprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index);
        double SlowDijkstra(unsigned long si, unsigned long ti);
        void SlowDijkstra_Destroy();
        
        
        void DijkstraPreprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index);
        double Dijkstra(unsigned long si, unsigned long ti);
        void DijkstraDestroy();
        
        
        deque<DijkstraOptimizedData> dijkstraOData;
        unordered_map<Vertex*, DijkstraOptimizedData*> dijkstraOMap;
        void ExternalDijkstra_Preprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index);
        double ExternalDijkstra(unsigned long si, unsigned long ti);
        void ExternalDijkstra_Unload();
        
        
        void TableLookup_Preprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index);
        double TableLookup(unsigned long si, unsigned long ti);
        void TableLookup_Unload();
        
        
        inline std::vector<Vertex*>::iterator begin() { return vertices.begin(); }
        inline std::vector<Vertex*>::const_iterator cbegin() const { return vertices.cbegin(); }
        inline std::vector<Vertex*>::iterator end() { return vertices.end(); }
        inline std::vector<Vertex*>::const_iterator cend() const { return vertices.cend(); }
        
        friend class Edge;
        friend class Vertex;
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
