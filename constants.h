#pragma once

// Constants for world size
const float WORLD_WIDTH = 2.0f;
const float WORLD_HEIGHT = 2.0f;
const float MOMENTUM_DECREASE_FACTOR = 0.95f; 
const float ACCELERATION = 0.01f; 
const float FRICTION = 0.98f; 

const float SPACESHIP_SIZE = 0.1f;
const float BULLET_SIZE = 0.025f;
const float GAME_LEFT_BOUNDARY = -1.0f;
const float GAME_RIGHT_BOUNDARY = 1.0f;
const float GAME_TOP_BOUNDARY = 1.0f;
const float GAME_BOTTOM_BOUNDARY = -1.0f;

const float GAME_WORLD_MIN_X = -1.0f; // Left boundary
const float GAME_WORLD_MAX_X = 1.0f;  // Right boundary
const float GAME_WORLD_MIN_Y = -1.0f; // Bottom boundary
const float GAME_WORLD_MAX_Y = 1.0f;  // Top boundary

const float screenBoundary = 1.0f; // Boundary for wrapping or removing enemy spaceships
const float BULLET_SPEED = 0.05f;
const float SPACESHIP_SPEED = 0.02f;

const float BULLET_WIDTH = 10.0f;
const float BULLET_HEIGHT = 5.0f;
const float ENEMY_WIDTH = 30.0f;
const float ENEMY_HEIGHT = 20.0f;


const float MOMENTUM_DECREASE = 0.995f;
enum Direction { Left, Right, Up, Down };


struct Rect {
    float x, y; // Position top-left corner
    float width, height; // Dimensions

    Rect() : x(0), y(0), width(0), height(0) {}
    Rect(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}
};
