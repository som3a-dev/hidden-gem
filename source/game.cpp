#include "game.h"
#include "gameplay_systems.h"
#include "question.h"

#include "map_format.h"

#include "vendor/json.hpp"

#include <raymath.h>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <utility>

void Game::init()
{
    screen_width = 640 * 2;
    screen_height = 360 * 2;
    tile_width = 36;
    tile_height = 36;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, "Hidden GEM");

    screen_width = GetScreenWidth();
    screen_height = GetScreenHeight();

    draw_buf.w = 640;
    draw_buf.h = 360;
    draw_buf.tex = LoadRenderTexture(draw_buf.w, draw_buf.h);

    camera.max_left = draw_buf.w * 0.2f;
    camera.max_right = draw_buf.w * 0.8f;

    gravity = 1000.0f;

    SetTargetFPS(60);
    
    asset_m.load_texture(ASSETS_PATH"brackeys_platformer_assets/sprites/knight.png");
    asset_m.load_texture(ASSETS_PATH"brackeys_platformer_assets/sprites/block.png");
    asset_m.load_texture(ASSETS_PATH"normal_map.png");
    asset_m.load_texture(ASSETS_PATH"torch.png");
    asset_m.load_texture(ASSETS_PATH"emote22-smol.png");
    asset_m.load_texture(ASSETS_PATH"papyrus.jpg");
    asset_m.load_texture(ASSETS_PATH"table2.png");
    asset_m.load_texture(ASSETS_PATH"1.png");
    asset_m.load_texture(ASSETS_PATH"win_wood.png");
    asset_m.load_texture(ASSETS_PATH"wood.jpg");
    asset_m.load_texture(ASSETS_PATH"book.png");

    FrameAnimation anim;
    anim.set_sheet(ASSETS_PATH"torch.png", asset_m, 4, 2);
    anim.interval_ms = 100;
    anim.push_frame_interval(0, 3, 0, 3);

    asset_m.load_frame_animation("torch_idle", std::move(anim));

    load_tileset(ASSETS_PATH"tileset.json");
    tilemap.create(screen_width / tile_width * 2, screen_height / tile_height * 2);
    load_castle();

//    ToggleFullscreen();

    shader = LoadShader(ASSETS_PATH"shaders/vertex.vs", ASSETS_PATH"shaders/fragment.fs");

    const int box_w = draw_buf.w;
    const int box_h = 24;
    Rectangle box_r = {
        0, (float)(draw_buf.h - box_h),
        (float)box_w, (float)box_h
    };

    box.text = "The stone corridor stretches farther than your light should allow, its walls damp and scarred by something that once tried to escape. Every step you take feels louder than the last, and for a brief moment, you are certain the darkness ahead breathes in response to you.";
    box.percent_visible = 0;
    box.show_text = true;
    box.set_box(box_r);

    question_panel.set_background(asset_m, ASSETS_PATH"papyrus.jpg");
    question_panel.set_scale(0.8f);
    question_panel.visible = true;
    question_panel.parent = this;
    question_panel.on_question_answered = on_question_answered;

    Rectangle panel_rect;
    panel_rect.width = 400;
    panel_rect.height = 300;
    panel_rect.x = (float)(draw_buf.w / 2) - panel_rect.width / 2;
    panel_rect.y = (float)(draw_buf.h / 2) - panel_rect.height / 2;
    question_panel.set_rect(panel_rect);

    QuestionData question;
    question.question = "The next scroll is hidden inside the Pharaoh`s chariot. Can you uncover How many spokes does each wheel have, and how does this design help?";
    question.options = {"Pushing", "Balancing", "Pulling"};
    question.option_count = 3;
    question.answer = "Pulling";
    question.feedback = "Brilliant! You`ve restored the weight of wisdom. 91 grams of pure genius! The next secret beats within the heart. seek the healers of Beliefs.";

    question_panel.questions.push_back(question);
    question_panel.questions.push_back(question);

    question_panel.init_default(*this);

    popup.visible = true;
    popup.confirm_box = true;
    popup.bg_color = BLUE;
    popup.text_color = YELLOW;
    popup.center_x = draw_buf.w / 2;
    popup.center_y = draw_buf.h / 2 - 50;

    popup.w = 150;
    popup.h = 150;

    font = LoadFontEx(ASSETS_PATH"AtkinsonHyperlegible-Regular.ttf", 64, NULL, 0);
    if (!IsFontValid(font)) 
    {
        font = GetFontDefault();
    }

    current_normal_map = ASSETS_PATH"normal_map.png";
}

void Game::destroy()
{
    UnloadFont(font);
    UnloadRenderTexture(draw_buf.tex);
    UnloadShader(shader);
    CloseWindow();
}

void Game::loop()
{
    running = true;
    while (running)
    {
        update();
        draw();
    }
}

void Game::QueueText(const char *text, int x, int y, int fontsz, Color color)
{
    assert(text);

    TextDrawCall call = {0};
    call.font = font;
    call.text = text;
    call.pos = {(float)x, (float)y};
    call.fontsz = (float)fontsz;
    call.spacing = 1;
    call.color = color;

    td_calls.push_back(call);
}

void Game::QueueTextEx(Font _font, const char *text, Vector2 pos, float fontsz, float spacing, Color color)
{
    assert(text);

    TextDrawCall call = {0};
    call.font = _font;
    call.text = text;
    call.pos = pos;
    call.fontsz = fontsz;
    call.spacing = spacing;
    call.color = color;

    td_calls.push_back(call);
}

void Game::on_question_answered(Game *game)
{
    game->missions_done++;
}

void Game::update()
{
    if (WindowShouldClose())
    {
        running = false;
        return;
    }
    if (IsKeyPressed(KEY_F11))
    {
        ToggleFullscreen();
    }

    screen_width = GetScreenWidth();
    screen_height = GetScreenHeight();

//    dt = GetFrameTime();
    dt = 0.016f;

    static int fps = 60;
    if (IsKeyDown(KEY_O))
    {
        fps -= 1;
        if (fps < 20)
        {
            fps = 20;
        }

        SetTargetFPS(fps);
    }
    if (IsKeyDown(KEY_P))
    {
        fps += 1;
        if (fps > 244)
        {
            fps = 244;
        }

        SetTargetFPS(fps);
    }

    if (IsKeyPressed(KEY_L))
    {
        debug_draw = !debug_draw;
    }

    using namespace GameplaySystems;
    player_system(*this);
    collision_update_system(*this);
    transform_update_system(*this);
    interactable_update_system(*this);
    animated_drawable_system(*this);

    ECS::TransformComponent* player_trans = world.transforms.get_component(player);
    if (player_trans)
    {
        camera.target = {player_trans->x, player_trans->y};
    }
    camera.update(dt);

    box.update(); 
    question_panel.update(this);
    popup.update(this);
}

void Game::update_shaders()
{
    const Texture2D* normal_map = asset_m.get_asset<Texture2D>(current_normal_map);
    if (normal_map)
    {
        int normal_map_uniform = GetShaderLocation(shader, "normalMap");
        SetShaderValueTexture(shader, normal_map_uniform, *normal_map);
    }

    int ambient_uniform = GetShaderLocation(shader, "ambientAtt");
    SetShaderValue(shader, ambient_uniform, &ambient_attenuation, SHADER_UNIFORM_FLOAT);

    torch_light.set_uniforms(shader, 0, {camera.x, camera.y});
    window_light.set_uniforms(shader, 1, {camera.x, camera.y});
}

void Game::draw()
{
    BeginTextureMode(draw_buf.tex);
    ClearBackground({3, 3, 3, 255});

    if (IsKeyPressed(KEY_I))
    {
        use_shader = !use_shader;
    }

    if (use_shader)
    {
        BeginShaderMode(shader);
    }

    update_shaders();

    draw_tilemap();

    GameplaySystems::render_drawable_system(*this);

    if (use_shader)
    {
        EndShaderMode();
    }

    GameplaySystems::draw_ui_system(*this);

    if (debug_draw)
    {
        draw_debug();
    }

    draw_ui();

    EndTextureMode();

    render_buffer();
}

void Game::draw_ui()
{
    Texture* book_tex = asset_m.get_asset<Texture>(ASSETS_PATH"book.png");
    if (book_tex)
    {
        DrawTexturePro(*book_tex, {
            0, 0, (float)(book_tex->width), (float)(book_tex->height)
        }, {
            4, 6, 24, 24 
        },
        {0, 0}, 0, WHITE);

        Rectangle book_rect = {
            4, 6, 24, 20
        };

        Vector2 text_pos = {
            book_rect.x + book_rect.width,
            book_rect.y
        };

        static char buf[128];
        snprintf(buf, sizeof(buf) / sizeof(char), " : %d", missions_done);

        QueueTextEx(font, buf, text_pos, 22, 1.0f, WHITE);
    }

    box.draw();
    question_panel.draw(this, font);
    popup.draw(this, font);
}

void Game::draw_debug()
{
    draw_player_debug_overlay();
    draw_tilemap_debug_overlay();

    int y = 0;
    int fps = GetFPS();
    char buf[512];

    const int fontsz = 16;
    snprintf(buf, sizeof(buf), "FPS: %d", fps);
    DrawText(buf, 0, y, fontsz, RED);
    y += fontsz;

    ECS::TransformComponent* player_trans = world.transforms.get_component(player);
    assert(player_trans);

    snprintf(buf, sizeof(buf), "Player: %f, %f", ((player_trans->x)),
    ((player_trans->y)));
    DrawText(buf, 0, y, fontsz, RED);
    y += fontsz;

    Color window_color = {
        (unsigned char)(255 * window_light.color.r),
        (unsigned char)(255 * window_light.color.g),
        (unsigned char)(255 * window_light.color.b),
        255
    };
    DrawRectangle((int)(window_light.x - camera.x), (int)(window_light.y - camera.y), 4, 4, window_color);
    DrawCircleLines((int)(window_light.x - camera.x), (int)(window_light.y - camera.y), window_light.radius, window_color);

    DrawCircleLines((int)(torch_light.x - camera.x), (int)(torch_light.y - camera.y), torch_light.radius, WHITE);
    DrawRectangle((int)(torch_light.x - camera.x), (int)(torch_light.y - camera.y), 4, 4, WHITE);
}

void Game::render_buffer()
{
    BeginDrawing();
    ClearBackground({20, 20, 20, 255});

    DrawTexturePro(draw_buf.tex.texture, 
    {0, 0, (float)(draw_buf.w), (float)(-(draw_buf.h))},
    {0, 0, (float)screen_width, (float)screen_height}, {0, 0}, 0, WHITE);

    draw_td_calls();

    EndDrawing();
}

void Game::draw_td_calls()
{
    for (const TextDrawCall& call : td_calls)
    {
        float scalex = ((float)(screen_width) / draw_buf.w);
        float scaley = ((float)(screen_height) / draw_buf.h);

        Vector2 pos;
        pos.x = call.pos.x * scalex;
        pos.y = call.pos.y * scaley;

        float fontsz = call.fontsz * scaley;

        DrawTextEx(call.font, call.text.c_str(), pos,
        fontsz, call.spacing, call.color);
    }

    td_calls.clear();
}

void Game::draw_tilemap()
{
    for (int tile_y = 0; tile_y < tilemap.get_height(); tile_y++)
    {
        for (int tile_x = 0; tile_x < tilemap.get_width(); tile_x++)
        {
            int id = tilemap.get_tile(tile_x, tile_y);
            if (id == EMPTY_TILE)
            {
                continue;
            }

            int x = tile_x * tile_width;
            int y = tile_y * tile_height;

            Rectangle tile_rect = {
                (float)x,
                (float)y,
                (float)tile_width,
                (float)tile_height
            };

            tile_rect.x -= floorf(camera.x);
            tile_rect.y -= floorf(camera.y);

            tile_rect.x = floorf(tile_rect.x);
            tile_rect.y = floorf(tile_rect.y);

            Tile* tile = asset_m.get_asset<Tile>(std::to_string(id));
            assert(tile);
            Texture2D* texture = asset_m.get_asset<Texture2D>(tile->texture_id);
            if (texture)
            {
                if (tile->sheet_w == 0)
                {
                    // Not a spritesheet, draw the entire texture
                    DrawTexturePro(*texture, {0, 0, (float)(texture->width), (float)(texture->height)},
                    tile_rect, {0, 0}, 0, WHITE);
                }
                else
                {
                    int sprite_w = texture->width / tile->sheet_w;
                    int sprite_h = texture->height / tile->sheet_h;
                    Rectangle src;
                    src.x = (float)(tile->sheet_x * sprite_w);
                    src.y = (float)(tile->sheet_y * sprite_h);
                    src.width = (float)sprite_w;
                    src.height = (float)sprite_h;

                    DrawTexturePro(*texture, src, tile_rect, {0, 0}, 0, WHITE);
                }
            }
            else
            {
                DrawRectangle(x, y, tile_width, tile_height, RED);
            }
        }
    }
}

void Game::draw_player_debug_overlay()
{
    ECS::TransformComponent* player_transform = world.transforms.get_component(player);
    ECS::CollisionComponent* player_collision = world.collisions.get_component(player);

    if (player_transform && player_collision)
    {
        Rectangle rect = {
            player_transform->x + player_collision->rect.x,
            player_transform->y + player_collision->rect.y,
            player_collision->rect.width,
            player_collision->rect.height
        };
        rect.x -= camera.x;
        rect.y -= camera.y;

        rect.x = floorf(rect.x);
        rect.y = floorf(rect.y);
        DrawRectangleLinesEx(rect, 1, RED);
    }
}

void Game::draw_tilemap_debug_overlay()
{
    if (debug_draw == false) return;

    for (int tile_y = 0; tile_y < tilemap.get_height(); tile_y++)
    {
        for (int tile_x = 0; tile_x < tilemap.get_width(); tile_x++)
        {
            int id = tilemap.get_tile(tile_x, tile_y);
            if (id == EMPTY_TILE)
            {
                continue;
            }

            Tile* tile = asset_m.get_asset<Tile>(std::to_string(id));
            assert(tile);

            if (tile->collidable == false)
            {
                continue;
            }

            int x = tile_x * tile_width;
            int y = tile_y * tile_height;

            Rectangle tile_rect = {
                (float)x,
                (float)y,
                (float)tile_width,
                (float)tile_height
            };

            DrawRectangleLinesEx(tile_rect, 1, RED);
        }
    }
}

void Game::load_tilemap(const std::string& filepath)
{
    mf_tilemap_t mf_map = mf_load_tilemap(filepath.c_str());

    for (mf_mapsz_t y = 0; y < mf_map.h; y++)
    {
        for (mf_mapsz_t x = 0; x < mf_map.w; x++)
        {
            mf_tileid_t tile = mf_tilemap_get(&mf_map, x, y);
            if (tile == MF_TILE_EMPTY)
            {
                tilemap.set_tile(x, y, EMPTY_TILE);
            }
            else
            {
                tilemap.set_tile(x, y, tile);
            }
        }
    }

    mf_tilemap_destroy(&mf_map);
}

void Game::load_tileset(const std::string& filepath)
{
    mf_tile_t* tiles;
    int tiles_count;
    mf_load_tileset(filepath.c_str(), &tiles, &tiles_count);

    for (int i = 0; i < tiles_count; i++)
    {
        mf_tile_t* mf_tile = tiles + i;
        Tile tile;
        tile.id = mf_tile->id;
        tile.texture_id = std::string(ASSETS_PATH) + mf_tile->texture_id;
        tile.sheet_x = mf_tile->sheet_x;
        tile.sheet_y = mf_tile->sheet_y;
        tile.sheet_w = mf_tile->sheet_w;
        tile.sheet_h = mf_tile->sheet_h;
        tile.collidable = mf_tile->collidable;

        asset_m.load_texture(tile.texture_id);
        asset_m.load_tile(tile);
    }

    mf_load_tileset_free(&tiles, tiles_count);
}

void Game::load_castle()
{
    load_tilemap(ASSETS_PATH"map.hgm");
    create_torch(520, 270);
    create_window(150, 225);
    create_table(450, 293);
    create_player(300, 180);

    torch_light.radius = 200;
    torch_light.color = {1.0f, 0.9f, 0.6f};
    torch_light.height = 70.0f;

    window_light.radius = 200;
    window_light.height = 200.0f;
    window_light.color = {172.0f / 255, 172.0f / 255, 193.0f / 255};

    ambient_attenuation = 0.05f;
}