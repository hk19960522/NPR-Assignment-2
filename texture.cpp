#include "texture.h"
#include <cstdio>
#include <cstring>
#include <random>

bool FloatEqual(float a, float b) { return fabs(a - b) <= 0.0005; }

void Texture::BuildBasicJointPattern(float avgWidth, float avgHeight)
{
    int rectangleCount = 0;
    float widthCount = 0.0;

    vector<Rectangle> nowRects;
    vector<Rectangle> preRects;

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
                Rectangle &rect = rectangles[i];
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
                        Rectangle &rect = rectangles[i];
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
                            Rectangle &rect = rectangles[i];
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
                    Rectangle &rect = rectangles[i];
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
                    Rectangle newRect(
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
            

            Rectangle newRect(minX, minY, maxX, maxY);
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
}

// private method

bool Texture::CheckBasicJointPatternIsLegal(vector<struct Rectangle> &rectangles)
{
    float area = 0.0;
    int rectNum = rectangles.size();
    for (int i = 0; i < rectNum; i++) {
        Rectangle& rectA = rectangles[i];
        for (int j = 0; j < rectNum; j++) {
            if (i == j) continue;

            Rectangle& rectB = rectangles[j];
            
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