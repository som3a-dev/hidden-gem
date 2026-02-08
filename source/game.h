#ifndef _GAME_H
#define _GAME_H

#include "raylib.h"
#include "asset_manager.h"
#include "frame_animation.h"
#include "ecs/world.h"
#include "tilemap.h"
#include "light_source.h"
#include "textbox.h"
#include "popup_image.h"
#include "game_draw_buffer.h"

#include <vector>


struct Game
{
    bool running;
    int screen_width;
    int screen_height;

    Shader shader;
    GameDrawBuffer draw_buf;
    Font font;

    bool debug_draw;
    bool show_paper;

    float mission_paper_scale;

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

    Textbox box;
    PopupImage mission_popup;

    void init();
    void destroy();

    void loop();

private:
    void update();
    void draw();

    void draw_tilemap();

    void draw_player_debug_overlay();
    void draw_tilemap_debug_overlay();

    void create_player(float x, float y);
    void create_torch(float x, float y);
    void create_table(float x, float y);

    void load_tilemap(const std::string& filepath);
    void load_tileset(const std::string& filepath);
};

#endif
