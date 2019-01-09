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
#include <thread>

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
    struct Graph;
    void ArcFlags_ZoneDivision(Graph* graph, int axesZones);
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
    struct ParallelDijkstraHolder : DataHolder
    {
        double weight{ 0 };
        char out{ 0 };
        Vertex* previous{ nullptr };
    };
    struct ArcDijkstraHolder : DataHolder
    {
        unsigned long zone{ 0 };
        double weight{ 0 };
        bool out{ false };
        Vertex* previous{ nullptr };
    };
    struct ArcFlagsHolder : DataHolder
    {
        bool* flags{ nullptr };
        ~ArcFlagsHolder();
    };
    
    struct Edge
    {
        Vertex* to{ nullptr };
        double weight{ 0 };
        bool out{ false };
        bool in{ false };
        
        DataHolder* data{ nullptr };
        bool savingCompleted{ false };
        
        ~Edge();
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
        
        
        bool parallelEnding{ false };
        Vertex* parallel_sEnd{ nullptr }, *parallel_tEnd{ nullptr };
        double ParallelMomentDijkstra(unsigned long si, unsigned long ti);
        void ReverseMomentDijkstra(unsigned long si, unsigned long ti);
        
        char mightEndAmount{ 0 }, sEndAmount{ 0 }, tEndAmount{ 0 };
        Vertex* sEnd{ nullptr }, *tEnd{ nullptr };
        vector<pair<Vertex*, Vertex*>> encounters;
        void ParallelDijkstra_Preprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index);
        double ParallelDijkstra(unsigned long si, unsigned long ti);
        void ReverseDijkstra(unsigned long si, unsigned long ti);
        void ParallelDijkstra_Destroy();
        
        
        deque<DijkstraOptimizedData> dijkstraOData;
        unordered_map<Vertex*, DijkstraOptimizedData*> dijkstraOMap;
        void ExternalDijkstra_Preprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index);
        double ExternalDijkstra(unsigned long si, unsigned long ti);
        void ExternalDijkstra_Unload();
        
        
        int ZonesNum{ 0 };
        void ArcFlags_Dijkstra(Vertex* s, Vertex* t);
        void ArcFlags_Preprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index);
        double ArcFlags(unsigned long si, unsigned long ti);
        void ArcFlags_Destroy();
        
        
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
