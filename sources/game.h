#ifndef _GAME_H
#define _GAME_H

#include "raylib.h"
#include "asset_manager.h"
#include "texture_animation.h"
#include "ecs/world.h"

#include <vector>

struct Game
{
    bool running;
    int screen_width;
    int screen_height;

    AssetManager asset_m;
    ECS::World world;

    int player;

    TextureAnimation animation;
    Vector2 sprite_pos;
    float sprite_speed;

    void init();
    void destroy();

    void loop();
    void update();
    void draw();

    void create_player();
};

#endif