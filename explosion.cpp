#include "explosion.h"
#include <QDebug>

std::vector<QOpenGLTexture*> Explosion::explosionTextures;
float Explosion::explosionWidth = 0.1f; 
float Explosion::explosionHeight = 0.1f;

Explosion::Explosion(float x, float y) : posX(x), posY(y), currentFrame(0), finished(false) {}

void Explosion::loadTextures(QOpenGLFunctions* glFunctions) {
    for (int i = 0; i < totalFrames; ++i) {
        QString texturePath = QString(":/game/explosion_frame_%1.png").arg(i);
        QImage img(texturePath);
        if (img.isNull()) {
            qDebug() << "Failed to load texture:" << texturePath;
            continue;
        }
        QOpenGLTexture* texture = new QOpenGLTexture(img);
        texture->setMinificationFilter(QOpenGLTexture::Nearest);
        texture->setMagnificationFilter(QOpenGLTexture::Linear);
        explosionTextures.push_back(texture);
    }
}

void Explosion::render() {
    if (currentFrame < totalFrames && !finished) {
        // Bind the texture for the current frame
        if (currentFrame < explosionTextures.size()) {
            explosionTextures[currentFrame]->bind();
        }

        // Assuming explosionWidth and explosionHeight define the size of the explosion
        float halfWidth = explosionWidth / 2.0f;
        float halfHeight = explosionHeight / 2.0f;

        // Render the explosion at its position
        glPushMatrix(); // Save the current transformation matrix
        glTranslatef(posX, posY, 0.0f); // Translate to the position of the explosion

        glBegin(GL_QUADS); // Begin drawing a quad
            glTexCoord2f(0.0f, 0.0f); glVertex2f(-halfWidth, -halfHeight); // Bottom-left
            glTexCoord2f(1.0f, 0.0f); glVertex2f(halfWidth, -halfHeight); // Bottom-right
            glTexCoord2f(1.0f, 1.0f); glVertex2f(halfWidth, halfHeight); // Top-right
            glTexCoord2f(0.0f, 1.0f); glVertex2f(-halfWidth, halfHeight); // Top-left
        glEnd(); // End drawing

        glPopMatrix(); // Restore the transformation matrix

        if (currentFrame < explosionTextures.size()) {
            explosionTextures[currentFrame]->release(); // Unbind the texture
        }

        // Update the current frame
        currentFrame++;
        if (currentFrame >= totalFrames) {
            finished = true; // Mark the explosion as finished
        }
    }
}

bool Explosion::isFinished() const {
    return finished;
}

void Explosion::bindTextureForFrame(int frame) {
    if (frame < explosionTextures.size()) {
        explosionTextures[frame]->bind();
    }
}
