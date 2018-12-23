#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <vector>
#include <cmath>
#include <map>

using namespace std;

struct Position
{
public:
    float x;
    float y;
    int index;
    int directIndex[4]; // 0 for up, 1 for down, 2 for left, 3 for right

    Position() : x(0.0), y(0.0) { Init(); };
    Position(float x, float y) : x(x), y(y) { Init(); };

    bool operator < (const Position &rhs) const
    {
        if (y == rhs.y)
            return x < rhs.x;
        return y < rhs.y;
    }

    bool operator == (Position rhs)
    {
        return x == rhs.x && y == rhs.y;
    }

private :
    void Init() {
        for (int i = 0; i < 4; i++) {
            directIndex[i] = -1;
        }
    }
};

struct Edge
{
public :
    int uIndex;
    int vIndex;
    int index;
    vector<Position> lines;

    Edge() : uIndex(-1), vIndex(-1) {};
    Edge(int uIndex, int vIndex) : uIndex(uIndex), vIndex(vIndex) {};

    bool operator < (Edge rhs)
    {
        if (uIndex == rhs.uIndex)
            return vIndex < rhs.vIndex;
        return uIndex < rhs.uIndex;
    }

    bool operator == (Edge rhs)
    {
        return uIndex == rhs.uIndex && vIndex == rhs.vIndex;
    }

};

struct TexturePolygon
{
public :
    vector<int> edges; // edge index
};

struct TextureRectangle
{
public:
    float minX, minY;
    float maxX, maxY;

    TextureRectangle(float minX, float minY, float maxX, float maxY) :
        minX(minX), minY(minY), maxX(maxX), maxY(maxY) {};
    void Print() {
        cout << minX << " " << maxX << " " << minY << " " << maxY << endl;
    };
};

class Texture
{
public :
    vector<TextureRectangle> rectangles;
    vector<Position> points;
    vector<Edge> edges;
    vector<TexturePolygon> polygons;
    map<Position, int> pointMap;
    map<pair<int, int>, int> edgeMap;

    Texture() : width(512), height(512) {};
    Texture(float width, float height) : width(width), height(height) {};

    void BuildBasicJointPattern(float avgWidth, float avgHeight);
    vector<TextureRectangle> GetRectangles() { return rectangles; };


private :
    float width;
    float height;

    

    bool CheckBasicJointPatternIsLegal(vector<TextureRectangle> &rectangles);
    bool IsInRange(float x, float y, float minX, float maxX, float minY, float maxY);

    void BuildGraph();
    void AddPoint(Position p, int &Count);
    void AddEdge(Edge e, int &Count);

    void PointsDisplacement();
};




#endif // !TEXTURE_H
