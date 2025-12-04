#ifndef _GAME_H
#define _GAME_H

#include "raylib.h"

struct Game
{
    bool running;
    int screen_width;
    int screen_height;

    Texture2D texture;
    Vector2 texture_pos;
    float texture_speed;

    void init();
    void destroy();

    void loop();
    void update();
    void draw();
};

#endif