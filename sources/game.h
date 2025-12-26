#ifndef _GAME_H
#define _GAME_H

#include "raylib.h"
#include "asset_manager.h"
#include "frame_animation.h"
#include "ecs/world.h"
#include "tilemap.h"

#include <vector>

struct LightColor 
{
    float r;
    float g;
    float b;
};

struct LightSource
{
    float x = 0;
    float y = 0;
    float radius = 0;
    float height = 0;
    float ambient_attenuation = 0;
    LightColor color = {0};
};

struct Game
{
    Shader shader;
    bool running;
    int screen_width;
    int screen_height;

    bool debug_draw;

    AssetManager asset_m;

    ECS::World world;
    Tilemap tilemap;
    int tile_width;
    int tile_height;

    float dt; // delta time in ms
    float gravity;

    int player;

    LightSource light;

    Vector2 light_pos;
    float light_radius;
    LightColor light_color;
    float ambient_attenuation;
    std::string current_normal_map;

    void init();
    void destroy();

    void loop();
    void update();
    void draw();
    void draw_tilemap();
    void draw_tilemap_debug_overlay();


    void create_tile(float x, float y);
    void create_player(float x, float y);

    void load_tilemap(const std::string& filepath);
    void load_tileset(const std::string& filepath);
};

#endif
