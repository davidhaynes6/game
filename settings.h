#pragma once
class GameSettings {
public:
    static constexpr float WORLD_WIDTH = 2.5f;
    static constexpr float WORLD_HEIGHT = 2.5f;
    static constexpr float ACCELERATION = 0.01f;
    static constexpr float SPACESHIP_SIZE = 0.1f;
    static constexpr float BULLET_SIZE = 0.025f;
    static constexpr float GAME_LEFT_BOUNDARY = -1.0f;
    static constexpr float GAME_RIGHT_BOUNDARY = 1.0f;
    static constexpr float GAME_TOP_BOUNDARY = 1.0f;
    static constexpr float GAME_BOTTOM_BOUNDARY = -1.0f;
    static constexpr float BULLET_SPEED = 0.05f;
    static constexpr float SPACESHIP_SPEED = 0.02f;
    static constexpr float BULLET_WIDTH = 10.0f;
    static constexpr float BULLET_HEIGHT = 5.0f;
    static constexpr float MOMENTUM_DECREASE = 0.995f;
    static constexpr float SCROLL_FACTOR_X = 0.005f;
    static constexpr float SCROLL_FACTOR_Y = 0.005f;
    static constexpr float BACKGROUND_SCALE_X = 2.0f; 
    static constexpr float BACKGROUND_SCALE_Y = 2.0f; 
    static constexpr float SCREENBOUNDARY = 2.0f; // Boundary for wrapping
    static constexpr int   LIFEICONSIZE = 20; // Adjust the size of the life icon
    static constexpr int   X_OFFSET = 10; // Adjust the horizontal offset
    static constexpr int   Y_OFFSET = 10; // Adjust the vertical offset

    enum Direction { Left, Right, Up, Down };

    struct Rect {
        float x, y; // Position top-left corner
        float width, height; // Dimensions

        Rect() : x(0), y(0), width(0), height(0) {}
        Rect(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}
    };
};
