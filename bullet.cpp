#include "bullet.h"

Bullet::Bullet(const QPointF& startPosition, float speed, GameSettings::Direction dir, QOpenGLTexture* texture)
    : position(startPosition), speed(speed), direction(dir), texture(texture) {
}

void Bullet::update() {
    // Move the bullet based on its speed and direction
    if (direction == GameSettings::Direction::Left) {
        position.rx() -= speed;
    }
    else { // Assume right direction
        position.rx() += speed;
    }
}

void Bullet::draw() const {
    if (!texture) return;

    texture->bind();
    float halfBulletWidth = GameSettings::BULLET_SIZE / 2;
    float halfBulletHeight = halfBulletWidth; // Uniform width and height

    glPushMatrix(); // save off current matrix
    glTranslatef(position.x(), position.y(), 0.0f);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-halfBulletWidth, -halfBulletHeight);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(halfBulletWidth, -halfBulletHeight);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(halfBulletWidth, halfBulletHeight);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-halfBulletWidth, halfBulletHeight);
    glEnd();

    glPopMatrix(); // restore previous matrix
    texture->release();
}

QPointF Bullet::getPosition() const {
    return position;
}

// Collision box for the bullet
QRectF Bullet::getBoundingBox() const {
    qDebug() << "Collision box for the bullet: x =" << position.x() << ", y =" << position.y();
    return QRectF(position.x() - GameSettings::BULLET_SIZE / 2, position.y() - GameSettings::BULLET_SIZE / 2,
        GameSettings::BULLET_SIZE, GameSettings::BULLET_SIZE);
}

bool Bullet::isOffScreen() const {
    // Check if the bullet's position is outside these boundaries
    return position.x() < -GameSettings::SCREENBOUNDARY || position.x() > GameSettings::SCREENBOUNDARY ||
           position.y() < -GameSettings::SCREENBOUNDARY || position.y() > GameSettings::SCREENBOUNDARY;
}