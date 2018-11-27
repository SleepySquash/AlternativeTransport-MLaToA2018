//
//  Graph.cpp
//  AlternativeTransport-MLaToA2018
//
//  Created by Никита Исаенко on 15/11/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#include "Graph.hpp"

namespace at
{
    Vertex::~Vertex() { for (int i = 0; i < edges.size(); ++i) { delete edges[i]; edges[i] = nullptr; } delete data; }
    void Vertex::Link(Vertex* to, double weight, bool out, bool in) { edges.push_back(new Edge(to, weight, out, in)); }
    void Vertex::Sync(Vertex* to, double weight, bool out, bool in)
    {
        Edge* edge = Connection(to);
        if (edge == nullptr)
            Link(to, weight, out, in);
        else
        {
            edge->out = (edge->out || out);
            edge->in = (edge->in || in);
        }
        
        edge = to->Connection(this);
        if (edge == nullptr)
            to->Link(this, weight, in, out);
        else
        {
            edge->out = (edge->out || in);
            edge->in = (edge->in || out);
        }
    }
    Edge* Vertex::Connection(Vertex* to) { if (edges.size() != 0) { for (auto e : edges) if (e->to == to) return e; } return nullptr; }



    Graph::~Graph() { for (int i = 0; i < vertexes.size(); ++i) { delete vertexes[i]; vertexes[i] = nullptr; } }
    void Graph::Push(Vertex* v) { vertexes.push_back(v); }
    Vertex* Graph::operator[](int i) { return vertexes[i]; }
    unsigned long Graph::size() { return vertexes.size(); }
    void Graph::PrintHierarchy()
    {
        for (auto v : *this)
        {
            cout << "- Вершина " << v << endl;
            for (auto e : v->edges)
                cout << "---- " << e->to << " " << e->weight << " " << e->out << " " << e->in << endl;
            cout << endl;
        }
    }
    void Graph::Remove(Vertex* vertex)
    {
        if (vertexes.size() != 0)
            for (int i = 0; i < vertexes.size(); ++i)
                if (vertexes[i] == vertex)
                {
                    Remove(i);
                    break;
                }
    }
    void Graph::Remove(unsigned long index)
    {
        if (index >= 0 && index < vertexes.size())
        {
            for (auto e : vertexes[index]->edges)
                for (unsigned long j = 0; j < e->to->edges.size(); ++j)
                    if (e->to->edges[j]->to == vertexes[index])
                    {
                        e->to->edges.erase(e->to->edges.begin() + j);
                        //break;
                    }
            delete vertexes[index];
            vertexes.erase(vertexes.begin() + index);
        }
    }
    void Graph::Load(const std::wstring& filename)
    {
        std::wifstream wif;
    #ifdef _WIN32
        wif.open(filename);
    #else
        wif.open(utf8(filename));
    #endif
        wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
        
        shortestPath.clear();
        if ((loaded = wif.is_open()))
        {
            filePath = filename;
            std::wstring line;
            
            std::getline(wif, line);
            unsigned long size = std::atol(utf8(line).c_str());
            if (size != 0)
            {
                vertexes.resize(size);
                for (int i = 0; i < size; ++i)
                    vertexes[i] = new Vertex();
                while (!wif.eof())
                {
                    std::getline(wif, line);
                    if (line.length() != 0)
                    {
                        unsigned int pos = 0;
                        if (line[0] != L'v' && line[0] != L'm') // it'll be info related to MAP, like x-y-z pos etc.
                        {
                            std::wstring v1 = ParseUntil(line, '-', pos);
                            pos += v1.length() + 1;
                            
                            unsigned long from = 0;
                            unsigned long to = 0;
                            double weight = 0;
                            bool out = true;
                            bool in = true;
                            
                            if (v1.length() != 0)
                            {
                                from = std::atol(utf8(v1).c_str());
                                std::wstring v2 = ParseUntil(line, ' ', pos);
                                if (v2.length() != 0 && from < size)
                                {
                                    pos += v2.length() + 1;
                                    to = std::atol(utf8(v2).c_str());
                                    
                                    if (to < size)
                                    {
                                        std::wstring v3 = ParseUntil(line, ' ', pos);
                                        pos += v3.length() + 1;
                                        weight = std::atof(utf8(v3).c_str());
                                        
                                        std::wstring v4 = ParseUntil(line, ' ', pos);
                                        pos += v4.length() + 1;
                                        if (v4.length() != 0)
                                        {
                                            out = (v4 == L"1") ? true : false;
                                            std::wstring v5 = ParseUntil(line, ' ', pos);
                                            in = (v5 == L"1") ? true : false;
                                        }
                                        
                                        vertexes[from]->Sync(vertexes[to], weight, out, in);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    void Graph::Save(const std::wstring& filename)
    {
        std::wofstream wof;
#ifdef _WIN32
        wof.open(filename);
#else
        wof.open(utf8(filename));
#endif
        wof.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
        
        if ((loaded = wof.is_open()))
        {
            for (auto v : vertexes)
                for (auto e : v->edges)
                    e->savingCompleted = false;
            
            wof << vertexes.size() << endl;
            for (unsigned long i = 0; i < vertexes.size(); ++i)
            {
                if (vertexes[i]->edges.size() != 0)
                    for (unsigned long j = 0; j < vertexes.size(); ++j)
                    {
                        Edge* edge = vertexes[i]->Connection(vertexes[j]);
                        if (edge != nullptr && !edge->savingCompleted && edge->weight > 0)
                        {
                            wof << i << L'-' << j << L' ' << edge->weight;
                            if (!edge->out || !edge->in)
                                wof << " " << edge->out << " " << edge->in;
                            wof << endl;
                            
                            Edge* another = vertexes[j]->Connection(vertexes[i]);
                            edge->savingCompleted = true;
                            another->savingCompleted = true;
                        }
                    }
            }
        }
        wof.close();
    }
    void Graph::Clear()
    {
        for (int i = 0; i < vertexes.size(); ++i) { delete vertexes[i]; vertexes[i] = nullptr; }
        vertexes.clear();
        loaded = false;
        filePath = L"";
    }
    struct PairByDistanceComparator { int operator() (const pair<double, Vertex*>& p1, const pair<double, Vertex*>& p2) { return p1.first > p2.first; } };
    
    
    
    void Graph::SlowDijkstra_Preprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index)
    {
        switch (mode)
        {
            case 0:
                for (auto v : vertexes)
                {
                    if (v->data != nullptr)
                        delete v->data;
                    v->data = new DijkstraHolder();
                }
                break;
            case 1:
                vertexes.back()->data = new DijkstraHolder();
                break;
            default: break;
        }
    }
    double Graph::SlowDijkstra(unsigned long si, unsigned long ti)
    {
        Vertex* s = vertexes[si];
        Vertex* t = vertexes[ti];
        
        for (auto v : vertexes) {
            DijkstraHolder* data = reinterpret_cast<DijkstraHolder*>(v->data);
            data->out = false;
            data->weight = std::numeric_limits<double>::infinity(); }
        DijkstraHolder* s_data = reinterpret_cast<DijkstraHolder*>(s->data);
        DijkstraHolder* t_data = reinterpret_cast<DijkstraHolder*>(t->data);
        s_data->previous = nullptr;
        t_data->previous = nullptr;
        
        vector<Vertex*> stack;
        s_data->weight = 0;
        stack.push_back(s);
        
        while (!stack.empty())
        {
            double min = std::numeric_limits<double>::infinity();
            unsigned long beg = 0;
            for (int i = 0; i < stack.size(); ++i)
            {
                DijkstraHolder* data = reinterpret_cast<DijkstraHolder*>(stack[i]);
                if (min > data->weight)
                {
                    beg = i;
                    min = data->weight;
                }
            }
            Vertex* current = stack[beg];
            DijkstraHolder* data = reinterpret_cast<DijkstraHolder*>(current->data);
            
            for (auto e : *current)
            {
                DijkstraHolder* to_data = reinterpret_cast<DijkstraHolder*>(e->to->data);
                if (e->out && !to_data->out)
                {
                    if (to_data->weight > data->weight + e->weight)
                    {
                        to_data->weight = data->weight + e->weight;
                        to_data->previous = current;
                        if (e->to != t) stack.push_back(e->to);
                    }
                }
            }
            data->out = true;
            stack.erase(stack.begin() + beg);
        }
        
        if (t_data->weight != 0 && t_data->weight != std::numeric_limits<double>::infinity())
        {
            Vertex* current = t;
            while (current != nullptr)
            {
                shortestPath.insert(shortestPath.begin(), current);
                current = reinterpret_cast<DijkstraHolder*>(current->data)->previous;
            }
        }
        
        return t_data->weight;
    }
    void Graph::SlowDijkstra_Destroy()
    {
        cout << "D Unload" << endl;
        for (auto v : vertexes)
        {
            if (v->data != nullptr)
                delete v->data;
            v->data = nullptr;
        }
    }
    
    
    
    void Graph::DijkstraPreprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index)
    {
        switch (mode) {
            case 0:
                for (auto v : vertexes)
                {
                    if (v->data != nullptr)
                        delete v->data;
                    v->data = new DijkstraHolder();
                }
                break;
            case 1:
                vertexes.back()->data = new DijkstraHolder();
                break;
            default: break;
        }
    }
    double Graph::Dijkstra(unsigned long si, unsigned long ti)
    {
        Vertex* s = vertexes[si];
        Vertex* t = vertexes[ti];
        
        for (auto v : vertexes) {
            DijkstraHolder* data = reinterpret_cast<DijkstraHolder*>(v->data);
            data->out = false;
            data->weight = std::numeric_limits<double>::infinity(); }
        DijkstraHolder* s_data = reinterpret_cast<DijkstraHolder*>(s->data);
        DijkstraHolder* t_data = reinterpret_cast<DijkstraHolder*>(t->data);
        s_data->previous = nullptr;
        t_data->previous = nullptr;
        
        priority_queue<pair<double, Vertex*>, vector<pair<double, Vertex*>>, PairByDistanceComparator> stack;
        s_data->weight = 0;
        stack.push(make_pair(0, s));
        
        while (!stack.empty())
        {
            Vertex* current = stack.top().second;
            DijkstraHolder* data = reinterpret_cast<DijkstraHolder*>(current->data);
            stack.pop();
            
            for (auto e : *current)
            {
                DijkstraHolder* to_data = reinterpret_cast<DijkstraHolder*>(e->to->data);
                if (e->out && !to_data->out)
                {
                    if (to_data->weight > data->weight + e->weight)
                    {
                        to_data->weight = data->weight + e->weight;
                        to_data->previous = current;
                        if (e->to != t) stack.push(make_pair(to_data->weight, e->to));
                    }
                }
            }
            data->out = true;
        }
        if (t_data->weight != 0 && t_data->weight != std::numeric_limits<double>::infinity())
        {
            Vertex* current = t;
            while (current != nullptr)
            {
                shortestPath.insert(shortestPath.begin(), current);
                current = reinterpret_cast<DijkstraHolder*>(current->data)->previous;
            }
        }
        
        return t_data->weight;
    }
    void Graph::DijkstraDestroy()
    {
        for (auto v : vertexes) {
            if (v->data != nullptr) delete v->data;
            v->data = nullptr;
        }
    }
    
    
    
    void Graph::ExternalDijkstra_Preprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index)
    {
        cout << "D Preproc: " << mode << "  vertex: " << vertex << "  edge: " << edge << "  index: " << index << endl;
        switch (mode)
        {
            case 0:
                dijkstraOData.clear();
                dijkstraOData.resize(vertexes.size());
                dijkstraOMap.reserve(vertexes.size());
                for (unsigned long i = 0; i < vertexes.size(); ++i)
                {
                    dijkstraOData[i] = DijkstraOptimizedData();
                    dijkstraOMap[vertexes[i]] = &(dijkstraOData[i]);
                }
                break;
                
            case 1:
                dijkstraOData.push_back(DijkstraOptimizedData());
                dijkstraOMap.insert({vertexes[index], &(dijkstraOData[index])});
                break;
                
            case 2:
                break;
                
            case 3:
                dijkstraOData.erase(dijkstraOData.begin() + index);
                dijkstraOMap.erase(vertex);
                break;
        }
    }
    double Graph::ExternalDijkstra(unsigned long s, unsigned long t)
    {
        for (unsigned long i = 0; i < vertexes.size(); ++i) {
            dijkstraOData[i].weight = std::numeric_limits<double>::infinity();
            dijkstraOData[i].out = false; }
        dijkstraOData[s].previous = nullptr;
        dijkstraOData[t].previous = nullptr;
        
        priority_queue<pair<double, Vertex*>, vector<pair<double, Vertex*>>, PairByDistanceComparator> stack;
        dijkstraOData[s].weight = 0;
        stack.push(make_pair(0, vertexes[s]));
        
        while (!stack.empty())
        {
            Vertex* current = stack.top().second;
            stack.pop();
            
            for (auto e : *current)
                if (e->out && !(*dijkstraOMap[e->to]).out)
                {
                    if ((*dijkstraOMap[e->to]).weight > (*dijkstraOMap[current]).weight + e->weight)
                    {
                        (*dijkstraOMap[e->to]).weight = (*dijkstraOMap[current]).weight + e->weight;
                        (*dijkstraOMap[e->to]).previous = current;
                        if (e->to != vertexes[t]) stack.push(make_pair((*dijkstraOMap[e->to]).weight, e->to));
                    }
                }
            (*dijkstraOMap[current]).out = true;
        }
        if (dijkstraOData[t].weight != 0 && dijkstraOData[t].weight != std::numeric_limits<double>::infinity())
        {
            Vertex* current = vertexes[t];
            while (current != nullptr)
            {
                shortestPath.insert(shortestPath.begin(), current);
                current = (*dijkstraOMap[current]).previous;
            }
        }
        
        return dijkstraOData[t].weight;
    }
    void Graph::ExternalDijkstra_Unload()
    {
        cout << "D Unload" << endl;
        dijkstraOData.clear();
        dijkstraOMap.clear();
    }
    
    
    
    void Graph::TableLookup_Preprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index)
    {
        cout << "TL Preproc: " << mode << "  vertex: " << vertex << "  edge: " << edge << "  index: " << index << endl;
    }
    double Graph::TableLookup(unsigned long s, unsigned long t)
    {
        return 321;
    }
    void Graph::TableLookup_Unload()
    {
        cout << "TL Unload" << endl;
    }
    
    
    
    
    
    
    /*double Graph::Dijkstra(unsigned long si, unsigned long ti)
    {
        Vertex* s = vertexes[si];
        Vertex* t = vertexes[ti];
        
        for (auto v : vertexes) {
            v->dijkstraOut = false;
            v->dijkstraWeight = std::numeric_limits<double>::infinity(); }
        s->dijkstraPrevious = nullptr;
        t->dijkstraPrevious = nullptr;
        
        priority_queue<pair<double, Vertex*>, vector<pair<double, Vertex*>>, PairByDistanceComparator> stack;
        s->dijkstraWeight = 0;
        stack.push(make_pair(0, s));
        
        while (!stack.empty())
        {
            Vertex* current = stack.top().second;
            stack.pop();
            
            for (auto e : *current)
                if (e->out && !e->to->dijkstraOut)
                {
                    if (e->to->dijkstraWeight > current->dijkstraWeight + e->weight)
                    {
                        e->to->dijkstraWeight = current->dijkstraWeight + e->weight;
                        e->to->dijkstraPrevious = current;
                        if (e->to != t) stack.push(make_pair(e->to->dijkstraWeight, e->to));
                    }
                }
            current->dijkstraOut = true;
        }
        
        if (t->dijkstraWeight != 0 && t->dijkstraWeight != std::numeric_limits<double>::infinity())
        {
            Vertex* current = t;
            while (current != nullptr)
            {
                shortestPath.insert(shortestPath.begin(), current);
                current = current->dijkstraPrevious;
            }
        }
        
        return t->dijkstraWeight;
    }*/
}
