#include "texture.h"
#include <cstdio>
#include <cstring>
#include <random>
#include <algorithm>
#include <set>
#include <utility>


bool FloatEqual(float a, float b) { return fabs(a - b) <= 0.0005; }

void Texture::BuildBasicJointPattern(float avgWidth, float avgHeight)
{
    int rectangleCount = 0;
    float widthCount = 0.0;

    vector<TextureRectangle> nowRects;
    vector<TextureRectangle> preRects;

    while (true) {
        bool haveNewRectangle = false;
        float widthCount = 0.0;
        while (widthCount < width) {

            // create rectangle with random size
            float wid = avgWidth * ((float)(rand() % 20) / 10.0);
            float hei = avgWidth * ((float)(rand() % 20) / 10.0);

            float minX, maxX, minY, maxY;
            int index = -1;
            minX = maxX = minY = maxY = 0.0;

            minX = widthCount;
            maxX = min(minX + wid, width);

            if (FloatEqual(minX, maxX) || FloatEqual(hei, 0)) continue;
            // Find the fittest y coordinate ( height )
            for (int i = 0; i < rectangles.size(); i++) {
                TextureRectangle &rect = rectangles[i];
                if (rect.maxX > minX && rect.minX < maxX) {
                    minY = max(minY, rect.maxY);
                    if (FloatEqual(minY, height)) {
                        maxX = rect.minX;
                        minY = 0.0;
                        i = -1;
                        continue;
                    }
                }
            }

            if (FloatEqual(minX, maxX)) {
                // Have to move to next place
                while (true) {
                    index = -1;
                    minY = 0;
                    for (int i = 0; i < rectangles.size(); i++) {
                        TextureRectangle &rect = rectangles[i];
                        if (rect.maxX > minX && rect.minX <= minX) {
                            minY = max(minY, rect.maxY);
                            if (FloatEqual(minY, height)) {
                                index = i;
                                break;
                            }
                        }
                    }
                    if (index != -1) {
                        minX = rectangles[index].maxX;
                        maxX = min(minX + wid, width);
                    }
                    else {
                        minY = 0;
                        for (int i = 0; i < rectangles.size(); i++) {
                            TextureRectangle &rect = rectangles[i];
                            if (rect.maxX > minX && rect.minX < maxX) {
                                minY = max(minY, rect.maxY);
                                if (FloatEqual(minY, height)) {
                                    maxX = rect.minX;
                                    minY = 0.0;
                                    i = -1;
                                    continue;
                                }
                            }
                        }
                        break;
                    }
                }
            }

            maxY = min(minY + hei, height);
            

            if (minY >= height || FloatEqual(minY, height) || FloatEqual(minX, maxX)) {
                // Can't put this rectangle
                //cout << minY << endl;
                //cout << minX << " " << maxX << endl;
                //cout << "HIDHIDHFIDHF\n";
                break;
            }

            // Find vacant space
            float nowX = minX;
            while (nowX < maxX) {
                index = -1;
                for (int i = 0; i < rectangles.size(); i++) {
                    TextureRectangle &rect = rectangles[i];
                    if (rect.maxX > nowX && rect.minX <= nowX) {
                        if (!FloatEqual(rect.maxY, minY)) {
                            if (index == -1 || rect.maxY > rectangles[index].maxY) {
                                index = i;
                            }
                        }
                        else {
                            index = -2;
                            nowX = rect.maxX;
                            break;
                        }
                    }
                }

                if (index == -2) continue;

                if (index != -1) {
                    TextureRectangle newRect(
                        nowX, rectangles[index].maxY,
                        min(rectangles[index].maxX, maxX), minY);
                    rectangles.push_back(newRect);
                    nowX = min(rectangles[index].maxX, maxX);

                    cout << "Vacant Space : ";
                    newRect.Print();
                }
                else {
                    break;
                }
            }
            

            TextureRectangle newRect(minX, minY, maxX, maxY);
            rectangles.push_back(newRect);
            haveNewRectangle = true;
            cout << minX << " " << maxX << " & " << minY << " " << maxY << endl;

            widthCount = maxX;
        }

        if (!haveNewRectangle) {
            break;
        }
    }
    
    if (!CheckBasicJointPatternIsLegal(rectangles)) {
        cout << "Wrong joint pattern.\n";
    }
    else {
        cout << "Correct.\n";
    }

    BuildGraph();
}

// private method

bool Texture::CheckBasicJointPatternIsLegal(vector<TextureRectangle> &rectangles)
{
    float area = 0.0;
    int rectNum = rectangles.size();
    for (int i = 0; i < rectNum; i++) {
        TextureRectangle& rectA = rectangles[i];
        for (int j = 0; j < rectNum; j++) {
            if (i == j) continue;

            TextureRectangle& rectB = rectangles[j];
            
            if (IsInRange(rectA.minX, rectA.minY, rectB.minX, rectB.maxX, rectB.minY, rectB.maxY) ||
                IsInRange(rectA.minX, rectA.maxY, rectB.minX, rectB.maxX, rectB.minY, rectB.maxY) ||
                IsInRange(rectA.maxX, rectA.minY, rectB.minX, rectB.maxX, rectB.minY, rectB.maxY) ||
                IsInRange(rectA.maxX, rectA.maxY, rectB.minX, rectB.maxX, rectB.minY, rectB.maxY)) {

                cout << "WRONG: \n";
                rectA.Print();
                rectB.Print();
                return false;
            }
        }

        float wid = rectangles[i].maxX - rectangles[i].minX;
        float hei = rectangles[i].maxY - rectangles[i].minY;

        area += wid * hei;
    }

    cout << "Area :\n";
    cout << area << " " << width * height << endl;
    return true;
}

bool Texture::IsInRange(float x, float y, float minX, float maxX, float minY, float maxY)
{
    return x > minX && x < maxX && y > minY && y < maxY;
}

void Texture::BuildGraph()
{
    int Count = 0;
    for (int i = 0; i < rectangles.size(); i++) {
        TextureRectangle &rect = rectangles[i];

        Position
            p1(rect.minX, rect.minY),
            p2(rect.minX, rect.maxY),
            p3(rect.maxX, rect.minY),
            p4(rect.maxX, rect.maxY);

        AddPoint(p1, Count), AddPoint(p2, Count), AddPoint(p3, Count), AddPoint(p4, Count);
    }

    cout << "Total Point : " << Count << endl;

    Count = 0;
    vector<Position> pos(points);
    set<pair<int, int>> s;
    sort(pos.begin(), pos.end());

    for (int i = 0; i < rectangles.size(); i++) {
        TextureRectangle &rect = rectangles[i];
        TexturePolygon poly;

        Position
            p1(rect.minX, rect.minY),
            p2(rect.maxX, rect.minY),
            p3(rect.maxX, rect.maxY),
            p4(rect.minX, rect.maxY);

        // search to right
        int index = lower_bound(pos.begin(), pos.end(), p1) - pos.begin();
        int nextIndex = index + 1;
        while (nextIndex < pos.size()) {
            Position &p = pos[nextIndex];
            if (p.y == p1.y && p.x > p1.x) {
                // Edge
                points[pos[index].index].directIndex[3] = p.index;
                points[p.index].directIndex[2] = pos[index].index;

                Edge edge(min(p.index, pos[index].index), max(p.index, pos[index].index));
                AddEdge(edge, Count);

                //cout << "From " << points[pos[index].index].x << " " << points[pos[index].index].y << endl;
                //cout << "To " << points[p.index].x << " " << points[p.index].y << " ....\n";

                poly.edges.push_back(edgeMap[{edge.uIndex, edge.vIndex}]);
                if (p == p2) {
                    break;
                }
                index = nextIndex;
            }
            //index = nextIndex;
            nextIndex++;
        }

        // search to top
        index = nextIndex;
        nextIndex = index + 1;
        while (nextIndex < pos.size()) {
            Position &p = pos[nextIndex];
            if (p.x == p2.x && p.y > p2.y) {
                // Edge
                points[pos[index].index].directIndex[0] = p.index;
                points[p.index].directIndex[1] = pos[index].index;

                Edge edge(min(p.index, pos[index].index), max(p.index, pos[index].index));
                AddEdge(edge, Count);

                //cout << "From " << points[pos[index].index].x << " " << points[pos[index].index].y << endl;
                //cout << "To " << points[p.index].x << " " << points[p.index].y << " ....\n";

                poly.edges.push_back(edgeMap[{edge.uIndex, edge.vIndex}]);
                if (p == p3) break;
                index = nextIndex;
            }
            //index = nextIndex;
            nextIndex++;
        }

        // search to left
        index = nextIndex;
        nextIndex = index - 1;
        while (nextIndex >= 0) {
            Position &p = pos[nextIndex];
            if (p.y == p3.y && p.x < p3.x) {
                // Edge
                points[pos[index].index].directIndex[2] = p.index;
                points[p.index].directIndex[3] = pos[index].index;

                Edge edge(min(p.index, pos[index].index), max(p.index, pos[index].index));
                AddEdge(edge, Count);

                //cout << "From " << points[pos[index].index].x << " " << points[pos[index].index].y << endl;
                //cout << "To " << points[p.index].x << " " << points[p.index].y << " ....\n";

                poly.edges.push_back(edgeMap[{edge.uIndex, edge.vIndex}]);
                if (p == p4) break;
                index = nextIndex;
            }
            //index = nextIndex;
            nextIndex--;
        }

        // search to down
        index = nextIndex;
        nextIndex = index - 1;
        while (nextIndex >= 0) {
            Position &p = pos[nextIndex];
            if (p.x == p4.x && p.y < p4.y) {
                // Edge
                points[pos[index].index].directIndex[1] = p.index;
                points[p.index].directIndex[0] = pos[index].index;

                Edge edge(min(p.index, pos[index].index), max(p.index, pos[index].index));
                AddEdge(edge, Count);

                //cout << "From " << points[pos[index].index].x << " " << points[pos[index].index].y << endl;
                //cout << "To " << points[p.index].x << " " << points[p.index].y << " ....\n";

                poly.edges.push_back(edgeMap[{edge.uIndex, edge.vIndex}]);
                if (p == p1) break;
                index = nextIndex;
            }
            //index = nextIndex;
            nextIndex--;
        }

        polygons.push_back(poly); 
        for (int i = 0; i < poly.edges.size(); i++) {
            cout << poly.edges[i] << " ";
        }
        cout << endl;
    }

    cout << "Edge Count : " << Count << endl;
}

void Texture::AddPoint(Position p, int &Count)
{
    if (pointMap.find(p) == pointMap.end()) {
        pointMap.insert({ p, Count });
        p.index = Count++;
        points.push_back(p);
    }
}

void Texture::AddEdge(Edge e, int &Count)
{
    pair<int, int> p(e.uIndex, e.vIndex);
    if (edgeMap.find(p) == edgeMap.end()) {
        edgeMap.insert({ p, Count });
        e.index = Count++;
        edges.push_back(e);
    }
}