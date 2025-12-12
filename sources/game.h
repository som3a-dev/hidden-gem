#ifndef _GAME_H
#define _GAME_H

#include "raylib.h"
#include "asset_manager.h"
#include "texture_animation.h"
#include "ecs/world.h"
#include "tilemap.h"

#include <vector>

struct Game
{
    bool running;
    int screen_width;
    int screen_height;

    bool debug_draw = false;

    AssetManager asset_m;

    ECS::World world;
    Tilemap tilemap;
    int tile_width;
    int tile_height;

    float dt; // delta time in ms
    float gravity;

    int player;

    void init();
    void destroy();

    void loop();
    void update();
    void draw();

    void create_tile(float x, float y);
    void create_player(float x, float y);

    void load_tilemap(const std::string& filepath);
};

#endif
