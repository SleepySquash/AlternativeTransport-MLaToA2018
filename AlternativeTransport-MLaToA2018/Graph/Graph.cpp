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
    Edge::~Edge() { delete data; }
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
    Edge* Vertex::Connection(Vertex* to) {
        if (to != nullptr) {if (edges.size() != 0) { for (auto e : edges) if (e != nullptr && e->to == to) return e; }} return nullptr; }

    
    ArcFlagsHolder::~ArcFlagsHolder() { delete flags; }


    Graph::~Graph() { for (int i = 0; i < vertices.size(); ++i) { delete vertices[i]; vertices[i] = nullptr; } }
    void Graph::Push(Vertex* v) { vertices.push_back(v); }
    Vertex* Graph::operator[](int i) { return vertices[i]; }
    unsigned long Graph::size() { return vertices.size(); }
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
        if (vertices.size() != 0)
            for (int i = 0; i < vertices.size(); ++i)
                if (vertices[i] == vertex)
                {
                    Remove(i);
                    break;
                }
    }
    void Graph::Remove(unsigned long index)
    {
        if (index >= 0 && index < vertices.size())
        {
            for (auto e : vertices[index]->edges)
                for (unsigned long j = 0; j < e->to->edges.size(); ++j)
                    if (e->to->edges[j]->to == vertices[index])
                    {
                        e->to->edges.erase(e->to->edges.begin() + j);
                        //break;
                    }
            delete vertices[index];
            vertices.erase(vertices.begin() + index);
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
                vertices.resize(size);
                for (int i = 0; i < size; ++i)
                    vertices[i] = new Vertex();
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
                                        
                                        vertices[from]->Sync(vertices[to], weight, out, in);
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
            for (auto v : vertices)
                for (auto e : v->edges)
                    e->savingCompleted = false;
            
            wof << vertices.size() << endl;
            for (unsigned long i = 0; i < vertices.size(); ++i)
            {
                if (vertices[i]->edges.size() != 0)
                    for (unsigned long j = 0; j < vertices.size(); ++j)
                    {
                        Edge* edge = vertices[i]->Connection(vertices[j]);
                        if (edge != nullptr && !edge->savingCompleted && edge->weight > 0)
                        {
                            wof << i << L'-' << j << L' ' << edge->weight;
                            if (!edge->out || !edge->in)
                                wof << " " << edge->out << " " << edge->in;
                            wof << endl;
                            
                            Edge* another = vertices[j]->Connection(vertices[i]);
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
        for (int i = 0; i < vertices.size(); ++i) { delete vertices[i]; vertices[i] = nullptr; }
        vertices.clear();
        loaded = false;
        filePath = L"";
    }
    struct PairByDistanceComparator { int operator() (const pair<double, Vertex*>& p1, const pair<double, Vertex*>& p2) { return p1.first > p2.first; } };
    
    
    
    void Graph::DijkstraPreprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index)
    {
        switch (mode) {
            case 0:
                for (auto v : vertices)
                {
                    if (v->data != nullptr)
                        delete v->data;
                    v->data = new DijkstraHolder();
                }
                break;
            case 1:
                vertices.back()->data = new DijkstraHolder();
                break;
            default: break;
        }
    }
    double Graph::Dijkstra(unsigned long si, unsigned long ti)
    {
        Vertex* s = vertices[si];
        Vertex* t = vertices[ti];
        
        for (auto v : vertices) {
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
            if (current == t) break;
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
                        /*if (e->to != t)*/ stack.push(make_pair(to_data->weight, e->to));
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
        for (auto v : vertices) {
            if (v->data != nullptr) delete v->data;
            v->data = nullptr;
        }
    }
    
    double Graph::OriginalDijkstra(unsigned long si, unsigned long ti)
    {
        Vertex* s = vertices[si];
        Vertex* t = vertices[ti];
        
        for (auto v : vertices) {
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
                        stack.push(make_pair(to_data->weight, e->to));
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
    
    
    
    void Graph::TDijkstra_Preprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index)
    {
        switch (mode) {
            case 0:
                for (auto v : vertices)
                {
                    if (v->data != nullptr)
                        delete v->data;
                    v->data = new TDijkstraHolder();
                }
                break;
            case 1:
                vertices.back()->data = new TDijkstraHolder();
                break;
            default: break;
        }
    }
    double Graph::TDijkstra(unsigned long si, unsigned long ti)
    {
        Vertex* s = vertices[si];
        Vertex* t = vertices[ti];
        
        /*for (auto v : vertices) {
            TDijkstraHolder* data = reinterpret_cast<TDijkstraHolder*>(v->data);
            data->out = false;
            data->weight = std::numeric_limits<double>::infinity(); }*/
        TDijkstraHolder* s_data = reinterpret_cast<TDijkstraHolder*>(s->data);
        TDijkstraHolder* t_data = reinterpret_cast<TDijkstraHolder*>(t->data);
        s_data->previous = nullptr;
        t_data->previous = nullptr;
        
        priority_queue<pair<double, Vertex*>, vector<pair<double, Vertex*>>, PairByDistanceComparator> stack;
        s_data->weight = 0;
        stack.push(make_pair(0, s));
        
        while (!stack.empty())
        {
            Vertex* current = stack.top().second;
            if (current == t) break;
            TDijkstraHolder* data = reinterpret_cast<TDijkstraHolder*>(current->data);
            stack.pop();
            
            for (auto e : *current)
            {
                // TODO: Fix. Infinity loop possible. What to do with data->out? <- problem is here.
                TDijkstraHolder* to_data = reinterpret_cast<TDijkstraHolder*>(e->to->data);
                if (to_data->time != timeLabel && e->out)
                {
                    to_data->weight = data->weight + e->weight;
                    to_data->previous = current;
                    stack.push(make_pair(to_data->weight, e->to));
                }
                else if (e->out && !to_data->out)
                {
                    if (to_data->weight > data->weight + e->weight)
                    {
                        to_data->weight = data->weight + e->weight;
                        to_data->previous = current;
                        /*if (e->to != t)*/ stack.push(make_pair(to_data->weight, e->to));
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
                current = reinterpret_cast<TDijkstraHolder*>(current->data)->previous;
            }
        }
        
        ++timeLabel;
        if (timeLabel == std::numeric_limits<unsigned char>::max())
            timeLabel = 0;
        
        return t_data->weight;
    }
    
    
    
    
    double Graph::ParallelMomentDijkstra(unsigned long si, unsigned long ti)
    {
        Vertex* s = vertices[si];
        Vertex* t = vertices[ti];
        
        parallelEnding = false;
        parallel_sEnd = nullptr; parallel_tEnd = nullptr;
        for (auto v : vertices) {
            ParallelDijkstraHolder* data = reinterpret_cast<ParallelDijkstraHolder*>(v->data);
            data->out = 0;
            data->weight = data->weightR = std::numeric_limits<double>::infinity(); }
        ParallelDijkstraHolder* s_data = reinterpret_cast<ParallelDijkstraHolder*>(s->data);
        ParallelDijkstraHolder* t_data = reinterpret_cast<ParallelDijkstraHolder*>(t->data);
        s_data->previous = nullptr;
        t_data->previousR = nullptr;
        
        s_data->weight = 0;
        t_data->weightR = 0;
        
        std::thread reverseDijkstra(&Graph::ReverseMomentDijkstra, this, si, ti);
        
        
        priority_queue<pair<double, Vertex*>, vector<pair<double, Vertex*>>, PairByDistanceComparator> stack;
        stack.push(make_pair(0, s));
        
        while (!stack.empty())
        {
            if (parallelEnding)
                break;
            
            Vertex* current = stack.top().second;
            if (current == t) {
                parallel_sEnd = reinterpret_cast<ParallelDijkstraHolder*>(current->data)->previous;
                parallel_tEnd = t; parallelEnding = true; break;}
            ParallelDijkstraHolder* data = reinterpret_cast<ParallelDijkstraHolder*>(current->data);
            data->out = 1;
            stack.pop();
            
            for (auto e : *current)
            {
                ParallelDijkstraHolder* to_data = reinterpret_cast<ParallelDijkstraHolder*>(e->to->data);
                if (to_data->out == 2)
                {
                    parallel_sEnd = current;
                    parallel_tEnd = e->to;
                    parallelEnding = true;
                    break;
                }
                else if (!to_data->out && e->out)
                {
                    if (to_data->weight > data->weight + e->weight)
                    {
                        to_data->weight = data->weight + e->weight;
                        to_data->previous = current;
                        /*if (e->to != t)*/ stack.push(make_pair(to_data->weight, e->to));
                    }
                }
            }
        }
        reverseDijkstra.join();
        
        
        ParallelDijkstraHolder* sEnd_data = reinterpret_cast<ParallelDijkstraHolder*>(parallel_sEnd->data);
        ParallelDijkstraHolder* tEnd_data = reinterpret_cast<ParallelDijkstraHolder*>(parallel_tEnd->data);
        if (sEnd_data->weight != std::numeric_limits<double>::infinity() && tEnd_data->weightR != std::numeric_limits<double>::infinity())
        {
            Vertex* current = parallel_tEnd;
            while (current != nullptr)
            {
                shortestPath.push_back(current);
                current = reinterpret_cast<ParallelDijkstraHolder*>(current->data)->previousR;
            }
            current = parallel_sEnd;
            while (current != nullptr)
            {
                shortestPath.insert(shortestPath.begin(), current);
                current = reinterpret_cast<ParallelDijkstraHolder*>(current->data)->previous;
            }
        }
        
        Edge* connection = parallel_sEnd->Connection(parallel_tEnd);
        return sEnd_data->weight + tEnd_data->weightR + connection->weight;
    }
    void Graph::ReverseMomentDijkstra(unsigned long si, unsigned long ti)
    {
        Vertex* s = vertices[si];
        Vertex* t = vertices[ti];
        
        priority_queue<pair<double, Vertex*>, vector<pair<double, Vertex*>>, PairByDistanceComparator> stack;
        stack.push(make_pair(0, t));
        
        while (!stack.empty())
        {
            if (parallelEnding)
                break;
            
            Vertex* current = stack.top().second;
            if (current == s) {
                parallel_sEnd = reinterpret_cast<ParallelDijkstraHolder*>(current->data)->previousR;
                parallel_tEnd = t; parallelEnding = true; break;}
            ParallelDijkstraHolder* data = reinterpret_cast<ParallelDijkstraHolder*>(current->data);
            data->out = 2;
            stack.pop();
            
            for (auto e : *current)
            {
                ParallelDijkstraHolder* to_data = reinterpret_cast<ParallelDijkstraHolder*>(e->to->data);
                if (to_data->out == 1)
                {
                    parallel_sEnd = e->to;
                    parallel_tEnd = current;
                    parallelEnding = true;
                    break;
                }
                else if (!to_data->out && e->in)
                {
                    if (to_data->weightR > data->weightR + e->weight)
                    {
                        to_data->weightR = data->weightR + e->weight;
                        to_data->previousR = current;
                        /*if (e->to != s)*/ stack.push(make_pair(to_data->weightR, e->to));
                    }
                }
            }
        }
    }
    
    
    
    void Graph::ParallelDijkstra_Preprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index)
    {
        switch (mode) {
            case 0:
                for (auto v : vertices)
                {
                    if (v->data != nullptr)
                        delete v->data;
                    v->data = new ParallelDijkstraHolder();
                }
                break;
            case 1:
                vertices.back()->data = new ParallelDijkstraHolder();
                break;
            default: break;
        }
    }
    double Graph::ParallelDijkstra(unsigned long si, unsigned long ti)
    {
        Vertex* s = vertices[si];
        Vertex* t = vertices[ti];
        
        mightEndAmount = 0; sEndAmount = -1; tEndAmount = -1;
        encounters.clear();
        sEnd = nullptr; tEnd = nullptr;
        for (auto v : vertices) {
            ParallelDijkstraHolder* data = reinterpret_cast<ParallelDijkstraHolder*>(v->data);
            data->out = 0;
            data->weight = data->weightR = std::numeric_limits<double>::infinity(); }
        ParallelDijkstraHolder* s_data = reinterpret_cast<ParallelDijkstraHolder*>(s->data);
        ParallelDijkstraHolder* t_data = reinterpret_cast<ParallelDijkstraHolder*>(t->data);
        s_data->previous = nullptr;
        t_data->previousR = nullptr;
        
        s_data->weight = 0;
        t_data->weightR = 0;
        
        std::thread reverseDijkstra(&Graph::ReverseDijkstra, this, si, ti);
        
        
        priority_queue<pair<double, Vertex*>, vector<pair<double, Vertex*>>, PairByDistanceComparator> stack;
        stack.push(make_pair(0, s));
        
        while (!stack.empty())
        {
            if (mightEndAmount > 0)
            {
                if (sEndAmount == -1)
                    sEndAmount = mightEndAmount;
                
                if (sEndAmount > 0)
                    --sEndAmount;
                if (sEndAmount <= 0)
                    break;
            }
            
            Vertex* current = stack.top().second;
            if (current == t) {sEnd = t; mightEndAmount = 1; tEndAmount = 0; break;}
            ParallelDijkstraHolder* data = reinterpret_cast<ParallelDijkstraHolder*>(current->data);
            data->out = 1;
            stack.pop();
            
            for (auto e : *current)
            {
                ParallelDijkstraHolder* to_data = reinterpret_cast<ParallelDijkstraHolder*>(e->to->data);
                if (to_data->out == 2)
                {
                    encounters.push_back(make_pair(current, e->to));
                    
                    if (mightEndAmount == 0)
                        mightEndAmount = parallelStepsDone;
                }
                else if (e->out && !to_data->out)
                {
                    if (to_data->weight > data->weight + e->weight)
                    {
                        to_data->weight = data->weight + e->weight;
                        to_data->previous = current;
                        /*if (e->to != t)*/ stack.push(make_pair(to_data->weight, e->to));
                    }
                }
            }
        }
        reverseDijkstra.join(); cout << endl;
        
        
        double minWeight = std::numeric_limits<double>::infinity();
        if (sEnd == nullptr && tEnd == nullptr)
        {
            for (auto p : encounters)
                if (p.first != nullptr && p.second != nullptr)
                {
                    ParallelDijkstraHolder* sEnd_data = reinterpret_cast<ParallelDijkstraHolder*>(p.first->data);
                    ParallelDijkstraHolder* tEnd_data = reinterpret_cast<ParallelDijkstraHolder*>(p.second->data);
                    Edge* connection = p.first->Connection(p.second);
                    if (sEnd_data->weight + tEnd_data->weightR + connection->weight < minWeight)
                    {
                        minWeight = sEnd_data->weight + tEnd_data->weightR + connection->weight;
                        sEnd = p.first;
                        tEnd = p.second;
                    }
                }
            ParallelDijkstraHolder* sEnd_data = reinterpret_cast<ParallelDijkstraHolder*>(sEnd->data);
            ParallelDijkstraHolder* tEnd_data = reinterpret_cast<ParallelDijkstraHolder*>(tEnd->data);
            if (sEnd_data->weight != std::numeric_limits<double>::infinity() && tEnd_data->weightR != std::numeric_limits<double>::infinity())
            {
                Vertex* current = tEnd;
                while (current != nullptr)
                {
                    shortestPath.push_back(current);
                    current = reinterpret_cast<ParallelDijkstraHolder*>(current->data)->previousR;
                }
                current = sEnd;
                while (current != nullptr)
                {
                    shortestPath.insert(shortestPath.begin(), current);
                    current = reinterpret_cast<ParallelDijkstraHolder*>(current->data)->previous;
                }
            }
            
            return minWeight;
        }
        else
        {
            if (sEnd != nullptr)
            {
                Vertex* current = sEnd;
                while (current != nullptr)
                {
                    shortestPath.insert(shortestPath.begin(), current);
                    current = reinterpret_cast<ParallelDijkstraHolder*>(current->data)->previous;
                }
                return reinterpret_cast<ParallelDijkstraHolder*>(sEnd->data)->weight;
            }
            else
            {
                Vertex* current = tEnd;
                while (current != nullptr)
                {
                    shortestPath.push_back(current);
                    current = reinterpret_cast<ParallelDijkstraHolder*>(current->data)->previousR;
                }
                return reinterpret_cast<ParallelDijkstraHolder*>(tEnd->data)->weightR;
            }
        }
    }
    void Graph::ReverseDijkstra(unsigned long si, unsigned long ti)
    {
        Vertex* s = vertices[si];
        Vertex* t = vertices[ti];
        
        priority_queue<pair<double, Vertex*>, vector<pair<double, Vertex*>>, PairByDistanceComparator> stack;
        stack.push(make_pair(0, t));
        
        while (!stack.empty())
        {
            if (mightEndAmount > 0)
            {
                if (tEndAmount == -1)
                    tEndAmount = mightEndAmount;
                
                if (tEndAmount > 0)
                    --tEndAmount;
                if (tEndAmount <= 0)
                    break;
            }
            
            Vertex* current = stack.top().second;
            if (current == s) {tEnd = s; mightEndAmount = 1; sEndAmount = 0; break;}
            ParallelDijkstraHolder* data = reinterpret_cast<ParallelDijkstraHolder*>(current->data);
            data->out = 2;
            stack.pop();
            
            for (auto e : *current)
            {
                if (e != nullptr)
                {
                    ParallelDijkstraHolder* to_data = reinterpret_cast<ParallelDijkstraHolder*>(e->to->data);
                    if (to_data->out == 1)
                    {
                        encounters.push_back(make_pair(e->to, current));
                        if (mightEndAmount == 0)
                            mightEndAmount = parallelStepsDone;
                    }
                    else if (e->in && !to_data->out)
                    {
                        if (to_data->weightR > data->weightR + e->weight)
                        {
                            to_data->weightR = data->weightR + e->weight;
                            to_data->previousR = current;
                            /*if (e->to != s)*/ stack.push(make_pair(to_data->weight, e->to));
                        }
                    }
                }
            }
        }
    }
    void Graph::ParallelDijkstra_Destroy()
    {
        for (auto v : vertices) {
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
                dijkstraOData.resize(vertices.size());
                dijkstraOMap.reserve(vertices.size());
                for (unsigned long i = 0; i < vertices.size(); ++i)
                {
                    dijkstraOData[i] = DijkstraOptimizedData();
                    dijkstraOMap[vertices[i]] = &(dijkstraOData[i]);
                }
                break;
                
            case 1:
                dijkstraOData.push_back(DijkstraOptimizedData());
                dijkstraOMap.insert({vertices[index], &(dijkstraOData[index])});
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
        for (unsigned long i = 0; i < vertices.size(); ++i) {
            dijkstraOData[i].weight = std::numeric_limits<double>::infinity();
            dijkstraOData[i].out = false; }
        dijkstraOData[s].previous = nullptr;
        dijkstraOData[t].previous = nullptr;
        
        priority_queue<pair<double, Vertex*>, vector<pair<double, Vertex*>>, PairByDistanceComparator> stack;
        dijkstraOData[s].weight = 0;
        stack.push(make_pair(0, vertices[s]));
        
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
                        if (e->to != vertices[t]) stack.push(make_pair((*dijkstraOMap[e->to]).weight, e->to));
                    }
                }
            (*dijkstraOMap[current]).out = true;
        }
        if (dijkstraOData[t].weight != 0 && dijkstraOData[t].weight != std::numeric_limits<double>::infinity())
        {
            Vertex* current = vertices[t];
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
    
    
    
    void Graph::ArcFlags_Dijkstra(Vertex* s, Vertex* t)
    {
        for (auto v : vertices) {
            ArcDijkstraHolder* data = reinterpret_cast<ArcDijkstraHolder*>(v->data);
            data->out = false;
            data->weight = std::numeric_limits<double>::infinity(); }
        ArcDijkstraHolder* s_data = reinterpret_cast<ArcDijkstraHolder*>(s->data);
        ArcDijkstraHolder* t_data = reinterpret_cast<ArcDijkstraHolder*>(t->data);
        s_data->previous = nullptr;
        t_data->previous = nullptr;
        
        priority_queue<pair<double, Vertex*>, vector<pair<double, Vertex*>>, PairByDistanceComparator> stack;
        s_data->weight = 0;
        stack.push(make_pair(0, s));
        
        while (!stack.empty())
        {
            Vertex* current = stack.top().second;
            if (current == t) break;
            ArcDijkstraHolder* data = reinterpret_cast<ArcDijkstraHolder*>(current->data);
            stack.pop();
            
            for (auto e : *current)
            {
                ArcDijkstraHolder* to_data = reinterpret_cast<ArcDijkstraHolder*>(e->to->data);
                if (e->out && !to_data->out)
                {
                    if (to_data->weight > data->weight + e->weight)
                    {
                        to_data->weight = data->weight + e->weight;
                        to_data->previous = current;
                        /*if (e->to != t)*/ stack.push(make_pair(to_data->weight, e->to));
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
                current = reinterpret_cast<ArcDijkstraHolder*>(current->data)->previous;
            }
        }
    }
    void Graph::ArcFlags_Preprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index)
    {
        int axesZones{ arcFlagsZonesAxes };
        
        if (mode == 0)
        {
            for (auto v : vertices)
            {
                if (v->data != nullptr)
                    delete v->data;
                v->data = new ArcDijkstraHolder();
            }
            
            // Разбиваем на области
            ArcFlags_ZoneDivision(axesZones);
            
            for (auto v : vertices)
            {
                for (auto e : v->edges)
                {
                    if (e->data != nullptr)
                        delete e->data;
                    e->data = new ArcFlagsHolder();
                    ArcFlagsHolder* data = reinterpret_cast<ArcFlagsHolder*>(e->data);
                    data->flags = new bool[ZonesNum]();
                }
            }
            
            // Определяем единицы в массивах
            for (auto v : vertices)
                for (auto e : v->edges)
                {
                    ArcDijkstraHolder* ve_data = reinterpret_cast<ArcDijkstraHolder*>(v->data);
                    ArcDijkstraHolder* to_data = reinterpret_cast<ArcDijkstraHolder*>(e->to->data);
                    if (ve_data->zone == to_data->zone)
                    {
                        ArcFlagsHolder* data = reinterpret_cast<ArcFlagsHolder*>(e->data);
                        data->flags[ve_data->zone] = true;
                    }
                }
            for (auto s : vertices)
                for (auto t :vertices)
                    if (s != t)
                    {
                        // Запускаем алгоритм Дейкстры и добавляем true рёбрам из кратчайшего пути в ячейке v2->zone.
                        ArcDijkstraHolder* v1_data = reinterpret_cast<ArcDijkstraHolder*>(s->data);
                        ArcDijkstraHolder* v2_data = reinterpret_cast<ArcDijkstraHolder*>(t->data);
                        if (v1_data->zone != v2_data->zone)
                        {
                            shortestPath.clear();
                            ArcFlags_Dijkstra(s, t);
                            if (shortestPath.size() != 0)
                                for (int i = 0; i < shortestPath.size() - 1; ++i)
                                {
                                    Edge* e = shortestPath[i]->Connection(shortestPath[i + 1]);
                                    ArcFlagsHolder* data = reinterpret_cast<ArcFlagsHolder*>(e->data);
                                    data->flags[v2_data->zone] = true;
                                }
                        }
                    }
            shortestPath.clear();
        }
    }
    double Graph::ArcFlags(unsigned long si, unsigned long ti)
    {
        Vertex* s = vertices[si];
        Vertex* t = vertices[ti];
        
        for (auto v : vertices) {
            ArcDijkstraHolder* data = reinterpret_cast<ArcDijkstraHolder*>(v->data);
            data->out = false;
            data->weight = std::numeric_limits<double>::infinity(); }
        ArcDijkstraHolder* s_data = reinterpret_cast<ArcDijkstraHolder*>(s->data);
        ArcDijkstraHolder* t_data = reinterpret_cast<ArcDijkstraHolder*>(t->data);
        s_data->previous = nullptr;
        t_data->previous = nullptr;
        unsigned long tZone{ t_data->zone };
        
        priority_queue<pair<double, Vertex*>, vector<pair<double, Vertex*>>, PairByDistanceComparator> stack;
        s_data->weight = 0;
        stack.push(make_pair(0, s));
        
        while (!stack.empty())
        {
            Vertex* current = stack.top().second;
            if (current == t) break;
            ArcDijkstraHolder* data = reinterpret_cast<ArcDijkstraHolder*>(current->data);
            stack.pop();
            
            for (auto e : *current)
            {
                ArcFlagsHolder* arc_data = reinterpret_cast<ArcFlagsHolder*>(e->data);
                // ЕСЛИ ребро e имеет единицу в массиве, то только тогда его рассматриваем
                if (arc_data->flags[tZone])
                {
                    ArcDijkstraHolder* to_data = reinterpret_cast<ArcDijkstraHolder*>(e->to->data);
                    if (e->out && !to_data->out)
                    {
                        if (to_data->weight > data->weight + e->weight)
                        {
                            to_data->weight = data->weight + e->weight;
                            to_data->previous = current;
                            /*if (e->to != t)*/ stack.push(make_pair(to_data->weight, e->to));
                        }
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
                current = reinterpret_cast<ArcDijkstraHolder*>(current->data)->previous;
            }
        }
        
        return t_data->weight;
    }
    void Graph::ArcFlags_Destroy()
    {
        for (auto v : vertices)
        {
            for (auto e : v->edges)
            {
                if (e->data != nullptr) delete e->data;
                e->data = nullptr;
            }
            if (v->data != nullptr) delete v->data;
            v->data = nullptr;
        }
    }
    
    
    
    void Graph::ArcFlags_ParallelDijkstra1(Vertex* s, Vertex* t)
    {
        for (auto v : vertices) {
            ArcParDijkstraHolder* data = reinterpret_cast<ArcParDijkstraHolder*>(v->data);
            data->out = false;
            data->weight = std::numeric_limits<double>::infinity(); }
        ArcParDijkstraHolder* s_data = reinterpret_cast<ArcParDijkstraHolder*>(s->data);
        ArcParDijkstraHolder* t_data = reinterpret_cast<ArcParDijkstraHolder*>(t->data);
        s_data->previous = nullptr;
        t_data->previous = nullptr;
        
        priority_queue<pair<double, Vertex*>, vector<pair<double, Vertex*>>, PairByDistanceComparator> stack;
        s_data->weight = 0;
        stack.push(make_pair(0, s));
        
        while (!stack.empty())
        {
            Vertex* current = stack.top().second;
            if (current == t) break;
            ArcParDijkstraHolder* data = reinterpret_cast<ArcParDijkstraHolder*>(current->data);
            stack.pop();
            
            for (auto e : *current)
            {
                ArcParDijkstraHolder* to_data = reinterpret_cast<ArcParDijkstraHolder*>(e->to->data);
                if (e->out && !to_data->out)
                {
                    if (to_data->weight > data->weight + e->weight)
                    {
                        to_data->weight = data->weight + e->weight;
                        to_data->previous = current;
                        /*if (e->to != t)*/ stack.push(make_pair(to_data->weight, e->to));
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
                current = reinterpret_cast<ArcParDijkstraHolder*>(current->data)->previous;
            }
        }
    }
    void Graph::ArcFlags_ParallelDijkstra2(Vertex* s, Vertex* t)
    {
        for (auto v : vertices) {
            ArcParDijkstraHolder* data = reinterpret_cast<ArcParDijkstraHolder*>(v->data);
            data->out2 = false;
            data->weight2 = std::numeric_limits<double>::infinity(); }
        ArcParDijkstraHolder* s_data = reinterpret_cast<ArcParDijkstraHolder*>(s->data);
        ArcParDijkstraHolder* t_data = reinterpret_cast<ArcParDijkstraHolder*>(t->data);
        s_data->previous2 = nullptr;
        t_data->previous2 = nullptr;
        
        priority_queue<pair<double, Vertex*>, vector<pair<double, Vertex*>>, PairByDistanceComparator> stack;
        s_data->weight2 = 0;
        stack.push(make_pair(0, s));
        
        while (!stack.empty())
        {
            Vertex* current = stack.top().second;
            if (current == t) break;
            ArcParDijkstraHolder* data = reinterpret_cast<ArcParDijkstraHolder*>(current->data);
            stack.pop();
            
            for (auto e : *current)
            {
                ArcParDijkstraHolder* to_data = reinterpret_cast<ArcParDijkstraHolder*>(e->to->data);
                if (e->out && !to_data->out2)
                {
                    if (to_data->weight2 > data->weight2 + e->weight)
                    {
                        to_data->weight2 = data->weight2 + e->weight;
                        to_data->previous2 = current;
                        /*if (e->to != t)*/ stack.push(make_pair(to_data->weight2, e->to));
                    }
                }
            }
            data->out2 = true;
        }
        if (t_data->weight2 != 0 && t_data->weight2 != std::numeric_limits<double>::infinity())
        {
            Vertex* current = t;
            while (current != nullptr)
            {
                shortestPath_th.insert(shortestPath_th.begin(), current);
                current = reinterpret_cast<ArcParDijkstraHolder*>(current->data)->previous2;
            }
        }
    }
    void Graph::ArcFlags_ParallelPreprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index)
    {
        int axesZones{ arcFlagsZonesAxes };
        
        if (mode == 0)
        {
            for (auto v : vertices)
            {
                if (v->data != nullptr)
                    delete v->data;
                v->data = new ArcParDijkstraHolder();
            }
            
            // Разбиваем на области
            ArcFlags_ZoneDivision(axesZones);
            
            for (auto v : vertices)
            {
                for (auto e : v->edges)
                {
                    if (e->data != nullptr)
                        delete e->data;
                    e->data = new ArcFlagsHolder();
                    ArcFlagsHolder* data = reinterpret_cast<ArcFlagsHolder*>(e->data);
                    data->flags = new bool[ZonesNum]();
                }
            }
            
            // Определяем единицы в массивах
            for (auto v : vertices)
                for (auto e : v->edges)
                {
                    ArcDijkstraHolder* ve_data = reinterpret_cast<ArcDijkstraHolder*>(v->data);
                    ArcDijkstraHolder* to_data = reinterpret_cast<ArcDijkstraHolder*>(e->to->data);
                    if (ve_data->zone == to_data->zone)
                    {
                        ArcFlagsHolder* data = reinterpret_cast<ArcFlagsHolder*>(e->data);
                        data->flags[ve_data->zone] = true;
                    }
                }
            
            std::thread parallel(&Graph::ArcFlags_PreprocessingThread, this);
            for (unsigned long i = 0; i < vertices.size()/2; ++i)
                for (unsigned long j = 0; j < vertices.size(); ++j)
                    if (i != j)
                    {
                        // Запускаем алгоритм Дейкстры и добавляем true рёбрам из кратчайшего пути в ячейке v2->zone.
                        ArcParDijkstraHolder* v1_data = reinterpret_cast<ArcParDijkstraHolder*>(vertices[i]->data);
                        ArcParDijkstraHolder* v2_data = reinterpret_cast<ArcParDijkstraHolder*>(vertices[j]->data);
                        if (v1_data->zone != v2_data->zone)
                        {
                            shortestPath.clear();
                            ArcFlags_ParallelDijkstra1(vertices[i], vertices[j]);
                            if (shortestPath.size() != 0)
                                for (int i = 0; i < shortestPath.size() - 1; ++i)
                                {
                                    Edge* e = shortestPath[i]->Connection(shortestPath[i + 1]);
                                    ArcFlagsHolder* data = reinterpret_cast<ArcFlagsHolder*>(e->data);
                                    data->flags[v2_data->zone] = true;
                                }
                        }
                    }
            shortestPath.clear();
            parallel.join();
        }
    }
    void Graph::ArcFlags_PreprocessingThread()
    {
        for (unsigned long i = vertices.size()/2; i < vertices.size(); ++i)
            for (unsigned long j = 0; j < vertices.size(); ++j)
                if (i != j)
                {
                    // Запускаем алгоритм Дейкстры и добавляем true рёбрам из кратчайшего пути в ячейке v2->zone.
                    ArcParDijkstraHolder* v1_data = reinterpret_cast<ArcParDijkstraHolder*>(vertices[i]->data);
                    ArcParDijkstraHolder* v2_data = reinterpret_cast<ArcParDijkstraHolder*>(vertices[j]->data);
                    if (v1_data->zone != v2_data->zone)
                    {
                        shortestPath_th.clear();
                        ArcFlags_ParallelDijkstra2(vertices[i], vertices[j]);
                        if (shortestPath_th.size() != 0)
                            for (int i = 0; i < shortestPath_th.size() - 1; ++i)
                            {
                                Edge* e = shortestPath_th[i]->Connection(shortestPath_th[i + 1]);
                                ArcFlagsHolder* data = reinterpret_cast<ArcFlagsHolder*>(e->data);
                                data->flags[v2_data->zone] = true;
                            }
                    }
                }
        shortestPath_th.clear();
    }
    double Graph::ParallelArcFlags(unsigned long si, unsigned long ti)
    {
        Vertex* s = vertices[si];
        Vertex* t = vertices[ti];
        
        for (auto v : vertices) {
            ArcParDijkstraHolder* data = reinterpret_cast<ArcParDijkstraHolder*>(v->data);
            data->out = false;
            data->weight = std::numeric_limits<double>::infinity(); }
        ArcParDijkstraHolder* s_data = reinterpret_cast<ArcParDijkstraHolder*>(s->data);
        ArcParDijkstraHolder* t_data = reinterpret_cast<ArcParDijkstraHolder*>(t->data);
        s_data->previous = nullptr;
        t_data->previous = nullptr;
        unsigned long tZone{ t_data->zone };
        
        priority_queue<pair<double, Vertex*>, vector<pair<double, Vertex*>>, PairByDistanceComparator> stack;
        s_data->weight = 0;
        stack.push(make_pair(0, s));
        
        while (!stack.empty())
        {
            Vertex* current = stack.top().second;
            if (current == t) break;
            ArcParDijkstraHolder* data = reinterpret_cast<ArcParDijkstraHolder*>(current->data);
            stack.pop();
            
            for (auto e : *current)
            {
                ArcFlagsHolder* arc_data = reinterpret_cast<ArcFlagsHolder*>(e->data);
                // ЕСЛИ ребро e имеет единицу в массиве, то только тогда его рассматриваем
                if (arc_data->flags[tZone])
                {
                    ArcParDijkstraHolder* to_data = reinterpret_cast<ArcParDijkstraHolder*>(e->to->data);
                    if (e->out && !to_data->out)
                    {
                        if (to_data->weight > data->weight + e->weight)
                        {
                            to_data->weight = data->weight + e->weight;
                            to_data->previous = current;
                            /*if (e->to != t)*/ stack.push(make_pair(to_data->weight, e->to));
                        }
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
                current = reinterpret_cast<ArcParDijkstraHolder*>(current->data)->previous;
            }
        }
        
        return t_data->weight;
    }
    
    
    
    void Graph::CH_Dijkstra(Vertex* s, Vertex* t)
    {
        for (auto v : vertices) {
            ArcDijkstraHolder* data = reinterpret_cast<ArcDijkstraHolder*>(v->data);
            data->out = false;
            data->weight = std::numeric_limits<double>::infinity(); }
        ArcDijkstraHolder* s_data = reinterpret_cast<ArcDijkstraHolder*>(s->data);
        ArcDijkstraHolder* t_data = reinterpret_cast<ArcDijkstraHolder*>(t->data);
        s_data->previous = nullptr;
        t_data->previous = nullptr;
        
        priority_queue<pair<double, Vertex*>, vector<pair<double, Vertex*>>, PairByDistanceComparator> stack;
        s_data->weight = 0;
        stack.push(make_pair(0, s));
        
        while (!stack.empty())
        {
            Vertex* current = stack.top().second;
            if (current == t) break;
            ArcDijkstraHolder* data = reinterpret_cast<ArcDijkstraHolder*>(current->data);
            stack.pop();
            
            for (auto e : *current)
            {
                ArcDijkstraHolder* to_data = reinterpret_cast<ArcDijkstraHolder*>(e->to->data);
                if (e->out && !to_data->out)
                {
                    if (to_data->weight > data->weight + e->weight)
                    {
                        to_data->weight = data->weight + e->weight;
                        to_data->previous = current;
                        /*if (e->to != t)*/ stack.push(make_pair(to_data->weight, e->to));
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
                current = reinterpret_cast<ArcDijkstraHolder*>(current->data)->previous;
            }
        }
    }
    void Graph::CH_Preprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index)
    {
        int axesZones{ 3 };
        
        if (mode == 0)
        {
            for (auto v : vertices)
            {
                if (v->data != nullptr)
                    delete v->data;
                v->data = new ArcDijkstraHolder();
            }
            
            // Разбиваем на области
            ArcFlags_ZoneDivision(axesZones);
            
            for (auto v : vertices)
            {
                for (auto e : v->edges)
                {
                    if (e->data != nullptr)
                        delete e->data;
                    e->data = new ArcFlagsHolder();
                    ArcFlagsHolder* data = reinterpret_cast<ArcFlagsHolder*>(e->data);
                    data->flags = new bool[ZonesNum]();
                }
            }
            
            // Определяем единицы в массивах
            for (auto v : vertices)
                for (auto e : v->edges)
                {
                    ArcDijkstraHolder* ve_data = reinterpret_cast<ArcDijkstraHolder*>(v->data);
                    ArcDijkstraHolder* to_data = reinterpret_cast<ArcDijkstraHolder*>(e->to->data);
                    if (ve_data->zone == to_data->zone)
                    {
                        ArcFlagsHolder* data = reinterpret_cast<ArcFlagsHolder*>(e->data);
                        data->flags[ve_data->zone] = true;
                    }
                }
            for (auto s : vertices)
                for (auto t :vertices)
                    if (s != t)
                    {
                        // Запускаем алгоритм Дейкстры и добавляем true рёбрам из кратчайшего пути в ячейке v2->zone.
                        ArcDijkstraHolder* v1_data = reinterpret_cast<ArcDijkstraHolder*>(s->data);
                        ArcDijkstraHolder* v2_data = reinterpret_cast<ArcDijkstraHolder*>(t->data);
                        if (v1_data->zone != v2_data->zone)
                        {
                            shortestPath.clear();
                            ArcFlags_Dijkstra(s, t);
                            if (shortestPath.size() != 0)
                                for (int i = 0; i < shortestPath.size() - 1; ++i)
                                {
                                    Edge* e = shortestPath[i]->Connection(shortestPath[i + 1]);
                                    ArcFlagsHolder* data = reinterpret_cast<ArcFlagsHolder*>(e->data);
                                    data->flags[v2_data->zone] = true;
                                }
                        }
                    }
            shortestPath.clear();
        }
    }
    double Graph::CH(unsigned long si, unsigned long ti)
    {
        Vertex* s = vertices[si];
        Vertex* t = vertices[ti];
        
        for (auto v : vertices) {
            ArcDijkstraHolder* data = reinterpret_cast<ArcDijkstraHolder*>(v->data);
            data->out = false;
            data->weight = std::numeric_limits<double>::infinity(); }
        ArcDijkstraHolder* s_data = reinterpret_cast<ArcDijkstraHolder*>(s->data);
        ArcDijkstraHolder* t_data = reinterpret_cast<ArcDijkstraHolder*>(t->data);
        s_data->previous = nullptr;
        t_data->previous = nullptr;
        unsigned long tZone{ t_data->zone };
        
        priority_queue<pair<double, Vertex*>, vector<pair<double, Vertex*>>, PairByDistanceComparator> stack;
        s_data->weight = 0;
        stack.push(make_pair(0, s));
        
        while (!stack.empty())
        {
            Vertex* current = stack.top().second;
            ArcDijkstraHolder* data = reinterpret_cast<ArcDijkstraHolder*>(current->data);
            stack.pop();
            
            for (auto e : *current)
            {
                ArcFlagsHolder* arc_data = reinterpret_cast<ArcFlagsHolder*>(e->data);
                // ЕСЛИ ребро e имеет единицу в массиве, то только тогда его рассматриваем
                if (arc_data->flags[tZone])
                {
                    ArcDijkstraHolder* to_data = reinterpret_cast<ArcDijkstraHolder*>(e->to->data);
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
            }
            data->out = true;
        }
        if (t_data->weight != 0 && t_data->weight != std::numeric_limits<double>::infinity())
        {
            Vertex* current = t;
            while (current != nullptr)
            {
                shortestPath.insert(shortestPath.begin(), current);
                current = reinterpret_cast<ArcDijkstraHolder*>(current->data)->previous;
            }
        }
        
        return t_data->weight;
    }
    void Graph::CH_Destroy()
    {
        for (auto v : vertices)
        {
            if (v->data != nullptr) delete v->data;
            v->data = nullptr;
        }
    }
    
    
    
    void Graph::Overlay_Dijkstra(Vertex* s, Vertex* t)
    {
        for (auto v : vertices) {
            ArcDijkstraHolder* data = reinterpret_cast<ArcDijkstraHolder*>(v->data);
            data->out = false;
            data->weight = std::numeric_limits<double>::infinity(); }
        ArcDijkstraHolder* s_data = reinterpret_cast<ArcDijkstraHolder*>(s->data);
        ArcDijkstraHolder* t_data = reinterpret_cast<ArcDijkstraHolder*>(t->data);
        s_data->previous = nullptr;
        t_data->previous = nullptr;
        
        priority_queue<pair<double, Vertex*>, vector<pair<double, Vertex*>>, PairByDistanceComparator> stack;
        s_data->weight = 0;
        stack.push(make_pair(0, s));
        
        while (!stack.empty())
        {
            Vertex* current = stack.top().second;
            if (current == t) break;
            ArcDijkstraHolder* data = reinterpret_cast<ArcDijkstraHolder*>(current->data);
            stack.pop();
            
            for (auto e : *current)
            {
                ArcDijkstraHolder* to_data = reinterpret_cast<ArcDijkstraHolder*>(e->to->data);
                if (e->out && !to_data->out)
                {
                    if (to_data->weight > data->weight + e->weight)
                    {
                        to_data->weight = data->weight + e->weight;
                        to_data->previous = current;
                        /*if (e->to != t)*/ stack.push(make_pair(to_data->weight, e->to));
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
                current = reinterpret_cast<ArcDijkstraHolder*>(current->data)->previous;
            }
        }
    }
    void Graph::Overlay_Preprocessing(unsigned int mode, Vertex* vertex, Edge* edge, unsigned long index)
    {
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Нужно разделить граф сначала на области
        /// ЦИКЛ ПО ВСЕМ ОБЛАСТЯМ:
        ///   Определить массив граничных вершин в области (сразу добавить в ОВЕРЛЕЙ_ГРАФ все граничные вершины)
        ///   От каждой граничной вершины внутри одной области рассчитать кратчайшие пути до каждой другой граничной вершины
        ///   Сразу в ОВЕРЛЕЙ_ГРАФ добавлять рёбра, веса которых равны кратчайшему пути от граничной вершины до другой граничной вершины
        /// КЦ
        ///
        /// Каждая вершины ОВЕРЛЕЙ_ГРАФа должна сохранять информацию о номере зоны, из которой она была создана.
        ///  Плюс нужно хранить ссылку на эту же граничную вершину из исходного графа, чтобы потом продолжить локальный поиск.
        ///
        /// При запросе нужно сначала рассчитать кратчайшие пути до всех границ внутри области, игнорируя вершины из других областей.
        ///  Одновременно (параллельно) делаем это же из конечной вершины.
        ///  Далее переходим в ОВЕРЛЕЙ_ГРАФ, где продолжаем поиск Дейкстры, пока не найдём
        ///////////////////////////////////////////////////////////////////////////////////////////
        
        int axesZones{ overlayZonesAxes };
        
        if (mode == 0)
        {
            for (auto v : vertices)
            {
                if (v->data != nullptr)
                    delete v->data;
                v->data = new OverlayHolder();
            }
            
            // Разбиваем на области
            Overlay_ZoneDivision(axesZones);
            
            
            // Определяем единицы в массивах
            for (auto s : vertices)
                for (auto t :vertices)
                    if (s != t)
                    {
                        // Запускаем алгоритм Дейкстры и добавляем true рёбрам из кратчайшего пути в ячейке v2->zone.
                        ArcDijkstraHolder* v1_data = reinterpret_cast<ArcDijkstraHolder*>(s->data);
                        ArcDijkstraHolder* v2_data = reinterpret_cast<ArcDijkstraHolder*>(t->data);
                        if (v1_data->zone != v2_data->zone)
                        {
                            shortestPath.clear();
                            ArcFlags_Dijkstra(s, t);
                            if (shortestPath.size() != 0)
                                for (int i = 0; i < shortestPath.size() - 1; ++i)
                                {
                                    Edge* e = shortestPath[i]->Connection(shortestPath[i + 1]);
                                    ArcFlagsHolder* data = reinterpret_cast<ArcFlagsHolder*>(e->data);
                                    data->flags[v2_data->zone] = true;
                                }
                        }
                    }
            shortestPath.clear();
        }
    }
    double Graph::OverlayGraph(unsigned long si, unsigned long ti)
    {
        Vertex* s = vertices[si];
        Vertex* t = vertices[ti];
        
        for (auto v : vertices) {
            ArcDijkstraHolder* data = reinterpret_cast<ArcDijkstraHolder*>(v->data);
            data->out = false;
            data->weight = std::numeric_limits<double>::infinity(); }
        ArcDijkstraHolder* s_data = reinterpret_cast<ArcDijkstraHolder*>(s->data);
        ArcDijkstraHolder* t_data = reinterpret_cast<ArcDijkstraHolder*>(t->data);
        s_data->previous = nullptr;
        t_data->previous = nullptr;
        unsigned long tZone{ t_data->zone };
        
        priority_queue<pair<double, Vertex*>, vector<pair<double, Vertex*>>, PairByDistanceComparator> stack;
        s_data->weight = 0;
        stack.push(make_pair(0, s));
        
        while (!stack.empty())
        {
            Vertex* current = stack.top().second;
            ArcDijkstraHolder* data = reinterpret_cast<ArcDijkstraHolder*>(current->data);
            stack.pop();
            
            for (auto e : *current)
            {
                ArcFlagsHolder* arc_data = reinterpret_cast<ArcFlagsHolder*>(e->data);
                // ЕСЛИ ребро e имеет единицу в массиве, то только тогда его рассматриваем
                if (arc_data->flags[tZone])
                {
                    ArcDijkstraHolder* to_data = reinterpret_cast<ArcDijkstraHolder*>(e->to->data);
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
            }
            data->out = true;
        }
        if (t_data->weight != 0 && t_data->weight != std::numeric_limits<double>::infinity())
        {
            Vertex* current = t;
            while (current != nullptr)
            {
                shortestPath.insert(shortestPath.begin(), current);
                current = reinterpret_cast<ArcDijkstraHolder*>(current->data)->previous;
            }
        }
        
        return t_data->weight;
    }
    void Graph::Overlay_Destroy()
    {
        for (auto v : vertices)
        {
            for (auto e : v->edges)
            {
                if (e->data != nullptr) delete e->data;
                e->data = nullptr;
            }
            if (v->data != nullptr) delete v->data;
            v->data = nullptr;
        }
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
