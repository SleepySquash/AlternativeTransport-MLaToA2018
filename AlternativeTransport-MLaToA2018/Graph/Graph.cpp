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
    Vertex::~Vertex() { for (int i = 0; i < edges.size(); ++i) { delete edges[i]; edges[i] = nullptr; } }
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
    void Graph::Load(const std::wstring& filename)
    {
        std::wifstream wif;
    #ifdef _WIN32
        wif.open(filename);
    #else
        wif.open(utf8(filename));
    #endif
        wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
        
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
                        if (line[0] != L'v') // it'll be info related to MAP, like x-y-z pos etc.
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
        // TODO
    }
    void Graph::Dijekstra(Vertex* s, Vertex* t)
    {
        vector<Vertex*> stack;
        s->dijekstraWeight = 0;
        stack.push_back(s);
        
        while (!stack.empty())
        {
            // Ищем вершину с минимальным весом, её обрабатываем первой (эвристика Дейкстры)
            double min = std::numeric_limits<double>::infinity();
            unsigned long beg = 0;
            for (int i = 0; i < stack.size(); ++i)
                if (min > stack[i]->dijekstraWeight)
                {
                    beg = i;
                    min = stack[i]->dijekstraWeight;
                }
            Vertex* current = stack[beg];
            
            // Каждой соединённой с нынешней вершиной вершине ищем "новые" минимальные расстояния
            for (auto e : *current)
                if (e->out && !e->to->dijekstraOut)
                {
                    if (e->to->dijekstraWeight > current->dijekstraWeight + e->weight)
                        e->to->dijekstraWeight = current->dijekstraWeight + e->weight;
                    if (e->to != t) stack.push_back(e->to);
                }
            current->dijekstraOut = true;
            stack.erase(stack.begin() + beg);
            
            // TODO: прекращение работы Дейкстры, если уже нашли мин. расстояние до (t)arget
            //       if (current == s) done = true; ?
        }
        cout << "s-t расстояние: " << t->dijekstraWeight << endl;
    }
}
