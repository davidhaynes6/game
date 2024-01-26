#pragma once
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include "settings.h"

class Spacecraft {
public:
    Spacecraft(QOpenGLTexture* texture);
    ~Spacecraft();

    QRectF getBoundingBox() const;

    void draw();
    void move(float deltaX, float deltaY);
    void setDirection(GameSettings::Direction direction);
    GameSettings::Direction getDirection() const;
    QPointF getPosition() const;
    void updatePosition(float deltaX, float deltaY);
    void setCameraPosition(QPointF& cameraPos);
    float getWidth();
    float getHeight();

private:
    QPointF position;
    GameSettings::Direction direction;
    float aspectRatio;
    QOpenGLTexture* texture;
    float width;
    float height;
    float upperBoundary;
    float lowerBoundary;
};
