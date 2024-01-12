#pragma once
#include "settings.h"
#include <qpoint.h>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>

class Bullet {
public:
    Bullet(const QPointF& startPosition, float speed, GameSettings::Direction dir, QOpenGLTexture* texture);
    void update();
    void draw() const;
    QPointF getPosition() const;
    QRectF getBoundingBox() const;
    bool isOffScreen() const;

private:
    QPointF position;
    float speed;
    GameSettings::Direction direction;
    QOpenGLTexture* texture; 
};
