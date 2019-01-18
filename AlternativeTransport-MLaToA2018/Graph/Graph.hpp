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
    void ArcFlags_ZoneDivision(int axesZones);
    void Overlay_ZoneDivision(int axesZones);
    void GraphReload();
    void ShowUsCH();
    namespace GraphComponents
    {
        struct VertexInfo;
    }
    struct Vertex;
    struct Edge;
    
    
    
    struct DataHolder { };
    
    
    
    struct OverlayVertex;
    struct OverlayGraph;
    struct OverlayEdge
    {
        OverlayVertex* to{ nullptr };
        double weight{ 0 };
        bool out{ false };
        bool in{ false };
        
        DataHolder* data{ nullptr };
        
        ~OverlayEdge() { delete data; }
        OverlayEdge(OverlayVertex* to, double weight, bool out = true, bool in = true) :
            to(to), weight(weight), out(out), in(in) { }
        
        friend class OverlayVertex;
        friend class OverlayGraph;
    };
    struct OverlayVertex
    {
        // max amount of edges = 18.446.744.073.709.551.615 (~2 * 10^19)
        vector<OverlayEdge*> edges;
        
        Vertex* vertex{ nullptr };
        unsigned long zone{ 0 };
        
        OverlayVertex(Vertex* v) : vertex(v) { }
        ~OverlayVertex();
        OverlayEdge* Link(OverlayVertex* to, double weight, bool out = true, bool in = true);
        void Sync(OverlayVertex* to, double weight, bool out = true, bool in = true);
        OverlayEdge* Connection(OverlayVertex* to);
        
        inline std::vector<OverlayEdge*>::iterator begin() { return edges.begin(); }
        inline std::vector<OverlayEdge*>::const_iterator cbegin() const { return edges.cbegin(); }
        inline std::vector<OverlayEdge*>::iterator end() { return edges.end(); }
        inline std::vector<OverlayEdge*>::const_iterator cend() const { return edges.cend(); }
        
        friend class OverlayEdge;
        friend class OverlayGraph;
    };
    struct OverlayGraph
    {
        // max amount of vertices = 18.446.744.073.709.551.615 (~2 * 10^19)
        vector<OverlayVertex*> vertices;
        vector<OverlayVertex*> shortestPath;
        
        ~OverlayGraph();
        void Push(OverlayVertex* v);
        OverlayVertex* operator[](int i);
        unsigned long size();
        void Clear();
        
        //void DijkstraPreprocessing();
        //double Dijkstra(unsigned long si, unsigned long ti);
        
        inline std::vector<OverlayVertex*>::iterator begin() { return vertices.begin(); }
        inline std::vector<OverlayVertex*>::const_iterator cbegin() const { return vertices.cbegin(); }
        inline std::vector<OverlayVertex*>::iterator end() { return vertices.end(); }
        inline std::vector<OverlayVertex*>::const_iterator cend() const { return vertices.cend(); }
        
        friend class OverlayEdge;
        friend class OverlayVertex;
    };
    
    
    
    struct DijkstraOptimizedData
    {
        double weight;
        bool out;
        Vertex* previous;
        
        DijkstraOptimizedData() : weight(std::numeric_limits<double>::infinity()), out(false), previous(nullptr) { }
    };
    
    
    
    struct DijkstraHolder : DataHolder
    {
        double weight{ 0 };
        bool out{ false };
        Vertex* previous{ nullptr };
    };
    struct TDijkstraHolder : DataHolder
    {
        unsigned char time{ 0 };
        double weight{ std::numeric_limits<double>::infinity() };
        bool out{ false };
        Vertex* previous{ nullptr };
    };
    struct ParallelDijkstraHolder : DataHolder
    {
        char out{ 0 };
        
        double weight{ 0 };
        Vertex* previous{ nullptr };
        
        double weightR{ 0 };
        Vertex* previousR{ nullptr };
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
    
    struct ArcParDijkstraHolder : DataHolder
    {
        unsigned long zone{ 0 };
        
        double weight{ 0 };
        bool out{ false };
        Vertex* previous{ nullptr };
        
        double weight2{ 0 };
        bool out2{ false };
        Vertex* previous2{ nullptr };
    };
    
    struct OverlayHolder : DataHolder
    {
        unsigned long zone{ 0 };
        double weight{ 0 };
        bool out{ false };
        Vertex* previous{ nullptr };
        OverlayVertex* overlay{ nullptr };
    };
    struct OverlayPathHolder : DataHolder
    {
        vector<Vertex*> path;
    };
    
    struct CHHolder : DataHolder
    {
        unsigned long importance{ 0 };
        unsigned int level{ 0 };
        bool hasShortcut{ false };
        
        double weight{ 0 };
        char out{ 0 };
        Vertex* previous{ nullptr };
        
        double weightR{ 0 };
        Vertex* previousR{ nullptr };
    };
    struct ShortcutHolder : DataHolder
    {
        Vertex* vertex{ nullptr };
        ShortcutHolder() { }
        ShortcutHolder(Vertex* vertex) : vertex(vertex) { }
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
        Edge* LinkReturn(Vertex* to, double weight, bool out = true, bool in = true);
        void Sync(Vertex* to, double weight, bool out = true, bool in = true);
        std::pair<Edge*, Edge*> SyncReturn(Vertex* to, double weight, bool out = true, bool in = true);
        Edge* Connection(Vertex* to);
        
        inline std::vector<Edge*>::iterator begin() { return edges.begin(); }
        inline std::vector<Edge*>::const_iterator cbegin() const { return edges.cbegin(); }
        inline std::vector<Edge*>::iterator end() { return edges.end(); }
        inline std::vector<Edge*>::const_iterator cend() const { return edges.cend(); }
        
        friend class Edge;
        friend class Graph;
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
        
        
        void DijkstraPreprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index);
        double Dijkstra(Vertex* s, Vertex* t);
        void DijkstraDestroy();
        
        double OriginalDijkstra(Vertex* s, Vertex* t);
        
        
        unsigned char timeLabel{ 0 };
        void TDijkstra_Preprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index);
        double TDijkstra(Vertex* s, Vertex* t);
        
        
        bool parallelEnding{ false };
        Vertex* parallel_sEnd{ nullptr }, *parallel_tEnd{ nullptr };
        double ParallelMomentDijkstra(Vertex* s, Vertex* t);
        void ReverseMomentDijkstra(Vertex* s, Vertex* t);
        
        char parallelStepsDone{ 30 }; /// <- КОЛИЧЕСТВО ШАГОВ ПОСЛЕ НАХОЖДЕНИЯ СОПРИКОСНОВЕНИЯ
        char mightEndAmount{ 0 }, sEndAmount{ 0 }, tEndAmount{ 0 };
        Vertex* sEnd{ nullptr }, *tEnd{ nullptr };
        vector<pair<Vertex*, Vertex*>> encounters;
        void ParallelDijkstra_Preprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index);
        double ParallelDijkstra(Vertex* s, Vertex* t);
        void ReverseDijkstra(Vertex* s, Vertex* t);
        void ParallelDijkstra_Destroy();
        
        
        int arcFlagsZonesAxes{ 6 }; /// <- КОЛИЧЕСТВО ЗОН ПО КАЖДОЙ ИЗ ОСИ (общее количество = квадрат этого числа)
        unsigned long ZonesNum{ 0 };
        void ArcFlags_Dijkstra(Vertex* s, Vertex* t);
        void ArcFlags_Dijkstra(Vertex* s);
        void ArcFlags_Preprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index);
        double ArcFlags(Vertex* s, Vertex* t);
        void ArcFlags_Destroy();
        
        vector<Vertex*> shortestPath_th;
        void ArcFlags_ParallelDijkstra1(Vertex* s, Vertex* t);
        void ArcFlags_ParallelDijkstra2(Vertex* s, Vertex* t);
        void ArcFlags_ParallelPreprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index);
        void ArcFlags_PreprocessingThread();
        double ParallelArcFlags(Vertex* s, Vertex* t);
        
        
        void TableLookup_Preprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index);
        double TableLookup(Vertex* s, Vertex* t);
        void TableLookup_Unload();
        
        
        void CH_Dijkstra(Vertex* s);
        void CH_Preprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index);
        double CH(Vertex* s, Vertex* t);
        void CH_ShortcutUnleash(Vertex* s, Vertex* t, std::vector<Vertex*>::iterator it);
        //void CH_ShortcutUnleash(Vertex* s, Vertex* t, unsigned long position);
        void CH_Destroy();
        
        
        OverlayGraph* overlayGraph{ nullptr };
        int overlayZonesAxes{ 3 };
        void Overlay_DijkstraWithPath(OverlayVertex* s, OverlayVertex* t);
        double Overlay_Dijkstra(Vertex* s, Vertex* t);
        void Overlay_Preprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index);
        double Overlay(Vertex* s, Vertex* t);
        void Overlay_Destroy();
        
        
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
