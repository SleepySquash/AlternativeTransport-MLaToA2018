//
//  main.cpp
//  AlternativeTransport-MLaToA2018
//
//  Created by Никита Исаенко on 07/11/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#include <iostream>
#include <vector>

#include <fstream>
#include <codecvt>

using std::cin;
using std::cout;
using std::endl;
using std::vector;


std::wstring ParseUntil(std::wstring line, const char until, unsigned int from)
{
    unsigned int pos{ from };
    std::wstring text = L"";
    bool Found{ false };
    
    while (!Found && pos < line.length())
    {
        if (!(Found = (line[pos] == until)))
        {
            if (line[pos] != 13)
                text += line[pos];
            pos++;
        }
    }
    
    return text;
}



std::string utf8(const std::wstring& wstr)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::string u8str = converter.to_bytes(wstr);
    return u8str;
}


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
    vector<Edge*> edges;
    
    ~Vertex() { for (int i = 0; i < edges.size(); ++i) { delete edges[i]; edges[i] = nullptr; } }
    void Link(Vertex* to, double weight, bool out = true, bool in = true) { edges.push_back(new Edge(to, weight, out, in)); }
};

struct Graph
{
    vector<Vertex*> vertexes;
    
    ~Graph() { for (int i = 0; i < vertexes.size(); ++i) { delete vertexes[i]; vertexes[i] = nullptr; } }
    void Push(Vertex* v) { vertexes.push_back(v); }
    Vertex* operator[](int i) { return vertexes[i]; }
    unsigned long size() { return vertexes.size(); }
    void PrintHierarchy()
    {
        for (auto v : *this)
        {
            cout << "- Вершина " << v << endl;
            for (auto e : v->edges)
                cout << "---- " << e->to << " " << e->weight << " " << e->out << " " << e->in << endl;
            cout << endl;
        }
    }
    void Load(const std::wstring& filename)
    {
        std::wifstream wif;
#ifdef _WIN32
        wif.open(filename);
#else
        wif.open(utf8(filename));
#endif
        wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
        
        if (wif.is_open())
        {
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
                        std::wstring v1 = ParseUntil(line, '-', pos);
                        pos += v1.length() + 1;
                        
                        long from = 0;
                        long to = 0;
                        double weight = 0;
                        bool out = true;
                        bool in = true;
                        
                        if (v1.length() != 0)
                        {
                            from = std::atol(utf8(v1).c_str());
                            std::wstring v2 = ParseUntil(line, ' ', pos);
                            pos += v2.length() + 1;
                            to = std::atol(utf8(v2).c_str());
                            
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
                            
                            vertexes[from]->Link(vertexes[to], weight, out, in);
                        }
                        
                    }
                }
            }
        }
    }
    void Save(const std::wstring& filename)
    {
        // TODO
    }
    
    inline std::vector<Vertex*>::iterator begin() noexcept { return vertexes.begin(); }
    inline std::vector<Vertex*>::const_iterator cbegin() const noexcept { return vertexes.cbegin(); }
    inline std::vector<Vertex*>::iterator end() noexcept { return vertexes.end(); }
    inline std::vector<Vertex*>::const_iterator cend() const noexcept { return vertexes.cend(); }
};




int main()
{
    Graph graph; // Граф
    graph.Load(L"in.txt");
    
    
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
    
    
    graph.PrintHierarchy();
}

/*
struct Vertex
{
    struct Edge
    {
        Vertex* to{ nullptr };
        double weight{ 0 };
        bool isTo{ false };
        bool isFrom{ false };
        
        Edge(Vertex* to, double weight, bool isTo = true, bool isFrom = true) : to(to), weight(weight), isTo(isTo), isFrom(isFrom) { }
    };
    
    vector<Edge> edges;
};


int main()
{
    vector<Vertex> graph; // <- GRAPH!!!
    
    graph.emplace_back();
    graph.emplace_back();
    graph.emplace_back();
    graph.emplace_back();
    graph.emplace_back();
    graph.emplace_back();
    graph.emplace_back();
    graph.emplace_back();
    graph.emplace_back();
    
    graph[0].edges.emplace_back(&graph[1], 5, true, true);
    graph[1].edges.emplace_back(&graph[0], 5, true, true);
    
    graph[0].edges.emplace_back(&graph[6], 12, false, true);
    graph[6].edges.emplace_back(&graph[0], 12, true, false);
    
    for (auto& v : graph)
    {
        cout << "- Вершина " << &v << endl;
        for (auto& e : v.edges)
            cout << "---- " << e.to << " " << e.weight << " " << e.isTo << " " << e.isFrom << endl;
        cout << endl;
    }
}*/
