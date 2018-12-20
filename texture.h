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

    Position() : x(0.0), y(0.0) {};
    Position(float x, float y) : x(x), y(y) {};

    bool operator < (const Position & rhs) const
    {
        if (y == rhs.y)
            return x < rhs.x;
        return y < rhs.y;
    }

    bool operator = (const Position & rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }
};

struct Edge
{
public :
    int uIndex;
    int vIndex;
    vector<struct Position> lines;

    Edge() : uIndex(-1), vIndex(-1) {};
    Edge(int uIndex, int vIndex) : uIndex(uIndex), vIndex(vIndex) {};

};

struct Polygon
{
public :
    vector<struct Edge> edges;
};

struct Rectangle
{
public:
    float minX, minY;
    float maxX, maxY;

    Rectangle(float minX, float minY, float maxX, float maxY) :
        minX(minX), minY(minY), maxX(maxX), maxY(maxY) {};
    void Print() {
        cout << minX << " " << maxX << " " << minY << " " << maxY << endl;
    };
};

class Texture
{
public :
    Texture() : width(512), height(512) {};
    Texture(float width, float height) : width(width), height(height) {};

    void BuildBasicJointPattern(float avgWidth, float avgHeight);
    vector<struct Rectangle> GetRectangles() { return rectangles; };


private :
    float width;
    float height;

    vector<struct Rectangle> rectangles;
    vector<struct Position> points;
    map<struct Position, int> pointMap;

    bool CheckBasicJointPatternIsLegal(vector<struct Rectangle> &rectangles);
    bool IsInRange(float x, float y, float minX, float maxX, float minY, float maxY);

    void BuildGraph();
    void AddPoint(Position p, int &Count);


};




#endif // !TEXTURE_H
