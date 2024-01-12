#pragma once
#include <cmath>

class CollisionDetector {
public:
    struct BoundingBox {
        float x, y; // Center position
        float width, height;
    };

    static bool checkCollision(const BoundingBox& box1, const BoundingBox& box2);
};