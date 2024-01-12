#pragma once
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <vector>

class Explosion {
public:
    Explosion(float x, float y);
    void render();
    bool isFinished() const;

    static void loadTextures(QOpenGLFunctions* glFunctions); // Function to load textures
    static std::vector<QOpenGLTexture*> explosionTextures; // Vector to store textures
    static float explosionWidth;
    static float explosionHeight;

private:
    float posX, posY;
    int currentFrame;
    static const int totalFrames = 10; 
    bool finished = false;

    void bindTextureForFrame(int frame);
};
