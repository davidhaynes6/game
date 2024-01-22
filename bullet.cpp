#include "bullet.h"

Bullet::Bullet(const QPointF& startPosition, float speed, GameSettings::Direction dir, QOpenGLTexture* texture)
    : position(startPosition), speed(speed), direction(dir), texture(texture) {
    // Initialize other necessary members here
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
    float bulletWidth = GameSettings::BULLET_SIZE;
    float bulletHeight = bulletWidth; // Uniform width and height

    glPushMatrix();
    glTranslatef(position.x(), position.y(), 0.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-bulletWidth / 2, -bulletHeight / 2);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(bulletWidth / 2, -bulletHeight / 2);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(bulletWidth / 2, bulletHeight / 2);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-bulletWidth / 2, bulletHeight / 2);
    glEnd();

    glPopMatrix();
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
    // Assuming you have a way to define the boundaries of your game screen
    // For example, you might have screen width and height defined somewhere
    const float screenLeft = -2.0f;  // Left boundary
    const float screenRight = 2.0f;  // Right boundary
    const float screenTop = 2.0f;    // Top boundary
    const float screenBottom = -2.0f; // Bottom boundary

    // Check if the bullet's position is outside these boundaries
    return position.x() < screenLeft || position.x() > screenRight ||
        position.y() < screenBottom || position.y() > screenTop;
}