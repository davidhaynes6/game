#include "player.h"
#include <QRectF>
#include <QPointF>

Spacecraft::Spacecraft(QOpenGLTexture* texture)
    : position(0.0f, 0.0f), direction(GameSettings::Direction::Right), aspectRatio(1.0f), texture(texture) {}

Spacecraft::~Spacecraft() {}

QRectF Spacecraft::getBoundingBox() const {
    // Assuming you have variables for the spacecraft's position, width, and height
    return QRectF(position.x() - width / 2, position.y() - height / 2, width, height);
}
void Spacecraft::draw() {
    if (!texture) return;

    texture->bind();
    float width = GameSettings::SPACESHIP_SIZE;
    float height = width / aspectRatio;

    glPushMatrix();
    glTranslatef(position.x(), position.y(), 0.0f);
    if (direction == GameSettings::Direction::Right) {
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    }

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-width / 2, -height / 2);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(width / 2, -height / 2);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(width / 2, height / 2);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-width / 2, height / 2);
    glEnd();

    glPopMatrix();
    texture->release();
}

void Spacecraft::move(float deltaX, float deltaY) {
    position.rx() += deltaX;
    position.ry() += deltaY;
}

void Spacecraft::setDirection(GameSettings::Direction newDirection) {
    direction = newDirection;
}

GameSettings::Direction Spacecraft::getDirection() const {
    return direction;
}

QPointF Spacecraft::getPosition() const {
    return position;
}

void Spacecraft::updatePosition(float deltaX, float deltaY) {
    position.rx() += deltaX;
    position.ry() += deltaY;

    // Check for screen boundaries
    if (position.y() > upperBoundary) {
        position.setY(upperBoundary);
    }
    else if (position.y() < lowerBoundary) {
        position.setY(lowerBoundary);
    }
}

void Spacecraft::setCameraPosition(QPointF& cameraPos) {
    cameraPos.setX(-position.x());
    cameraPos.setY(-position.y());
}