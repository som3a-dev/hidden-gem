#ifndef _GAME_H
#define _GAME_H

#include "asset_manager.h"
#include "frame_animation.h"
#include "ecs/world.h"
#include "tilemap.h"
#include "light_source.h"
#include "game_draw_buffer.h"
#include "camera.h"

#include "ui/textbox.h"
#include "ui/popup_box.h"
#include "ui/question_panel.h"

#include <raylib.h>
#include <vector>

struct Game
{
public:
    bool running;

    void init();
    void destroy();
    void loop();

    // Some counterparts of raylib functions, matching in args and case

    // Queue text to be drawn in the UI/Screen pass rather than the draw buffer pass
    // Positions here are the logical positions as would be drawn in the draw buffer,
    // They get scaled to the screen.
    void QueueText(const char* text, int x, int y, int fontsz, Color color);
    void QueueTextEx(Font _font, const char* text, Vector2 pos, float fontsz, float spacing, Color color);

    int tile_width;
    int tile_height;
    int screen_width;
    int screen_height;
    float dt; // delta time in ms

    Shader shader;
    GameDrawBuffer draw_buf;
    Font font;

    bool debug_draw;
    bool show_paper;

    AssetManager asset_m;

    ECS::World world;
    Tilemap tilemap;

    float gravity;

    int player;

    FollowCamera camera;

    LightSource torch_light;
    LightSource window_light;
    float ambient_attenuation;
    std::string current_normal_map;

    UI::Textbox box;
    UI::QuestionPanel question_panel;
    UI::PopupBox popup;

    int missions_done = 0;

    bool use_shader = true;

    Texture win = {0};
    float window_x = 0;
    float window_y = 0;
    float window_scale = 0.5f;

    Texture background = {0};

    // Callbacks
    static void on_question_answered(Game* game);

    void update();
    void update_shaders();
    void draw();
    void draw_ui();
    void draw_debug();
    void render_buffer();
   
    // Draw all the TextDrawCalls for the frame, and clear the list
    void draw_td_calls();

    void draw_tilemap();

    void draw_player_debug_overlay();
    void draw_tilemap_debug_overlay();

    void create_player(float x, float y);
    void create_torch(float x, float y);
    void create_table(float x, float y);

    void create_window(float x, float y);

    void load_tilemap(const std::string& filepath);
    void load_tileset(const std::string& filepath);

    void load_castle();
    void load_outside();

    private:
    struct TextDrawCall
    {
        Font font;
        std::string text;
        Vector2 pos;
        float fontsz;
        float spacing;
        Color color;
    };

    std::vector<TextDrawCall> td_calls;
};

#endif


// NEXT(): Game Menu, Game intro, Camera, Sound, Mission progress, Web/Android build

// next(): small popup box for hints, confirmation (with yes or no) and other stuff, timer for questions