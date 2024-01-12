#include "collision.h"
#include <QDebug>

bool CollisionDetector::checkCollision(const CollisionDetector::BoundingBox& box1, const CollisionDetector::BoundingBox& box2) {
    return (box1.x < box2.x + box2.width &&
        box1.x + box1.width > box2.x &&
        box1.y < box2.y + box2.height &&
        box1.y + box1.height > box2.y);
}