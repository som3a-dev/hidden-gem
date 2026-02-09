#include "game.h"
#include "gameplay_systems.h"

#include "map_format.h"

#include "vendor/json.hpp"

#include <raymath.h>

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

//    ToggleFullscreen();

    screen_width = GetScreenWidth();
    screen_height = GetScreenHeight();

    draw_buf.w = 640;
    draw_buf.h = 360;
    draw_buf.tex = LoadRenderTexture(draw_buf.w, draw_buf.h);

    gravity = 1000.0f;

    SetTargetFPS(60);
    
    asset_m.load_texture(ASSETS_PATH"brackeys_platformer_assets/sprites/knight.png");
    asset_m.load_texture(ASSETS_PATH"brackeys_platformer_assets/sprites/block.png");
    asset_m.load_texture(ASSETS_PATH"normal_map.png");
    asset_m.load_texture(ASSETS_PATH"torch.png");
    asset_m.load_texture(ASSETS_PATH"emote22-smol.png");
    asset_m.load_texture(ASSETS_PATH"papyrus.jpg");
    asset_m.load_texture(ASSETS_PATH"table2.png");

    FrameAnimation anim;
    anim.set_sheet(ASSETS_PATH"torch.png", asset_m, 4, 2);
    anim.interval_ms = 100;
    anim.push_frame_interval(0, 3, 0, 3);

    asset_m.load_frame_animation("torch_idle", std::move(anim));

    load_tileset(ASSETS_PATH"tileset.json");

    tilemap.create(screen_width / tile_width * 2, screen_height / tile_height * 2);
    load_tilemap(ASSETS_PATH"map.hgm");

    create_torch(520, 270);
    create_table(450, 293);
    create_player(300, 180);

//    ToggleFullscreen();

    shader = LoadShader(ASSETS_PATH"shaders/vertex.vs", ASSETS_PATH"shaders/fragment.fs");
    light.radius = 800;
    light.color = {1.0f, 0.9f, 0.6f};
    light.height = 70.0f;
    ambient_attenuation = 0.2f;

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

//    font = LoadFont(ASSETS_PATH"ThaleahFat.ttf");
    if (!IsFontValid(font)) 
    {
        font = GetFontDefault();
    }

    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);

    mission_popup.tex = asset_m.get_asset<Texture>(ASSETS_PATH"papyrus.jpg");
    mission_popup.scale = 0.5f;
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

void Game::update()
{
    if (WindowShouldClose())
    {
        running = false;
        return;
    }

    dt = GetFrameTime();

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

    if (light.x > screen_width)
    {
        light.x = -100;
    }

    if (IsKeyDown(KEY_UP))
    {
        light.radius += 5;
    }
    else if (IsKeyDown(KEY_DOWN))
    {
        light.radius -= 5;
    }

    if (IsKeyDown(KEY_RIGHT))
    {
        light.height += 5;
    }
    else if (IsKeyDown(KEY_LEFT))
    {
        light.height -= 5;
    }

    if (light.radius < 0)
    {
        light.radius = 0;
    }
    if (light.height < 0)
    {
        light.height = 0;
    }

    using namespace GameplaySystems;
    player_system(world, dt);

    collision_update_system(world, *this, tilemap);
    transform_update_system(world, *this);
    interactable_update_system(world, *this);
    animated_drawable_system(world, asset_m);

    current_normal_map = ASSETS_PATH"normal_map.png";

    mission_popup.visible = true;

    box.update(); 
    mission_popup.update(dt);
}

void Game::draw()
{
    if (IsWindowResized())
    {
        screen_width = GetScreenWidth();
        screen_height = GetScreenHeight();
    }

    const Texture2D* normal_map = asset_m.get_asset<Texture2D>(current_normal_map);

    BeginTextureMode(draw_buf.tex);
    ClearBackground({3, 3, 3, 255});

    static bool use_shader = true;
    if (IsKeyPressed(KEY_I))
    {
        use_shader = !use_shader;
    }

    if (use_shader)
    {
        BeginShaderMode(shader);
    }

    light.set_uniforms(shader, 5);
    light.set_uniforms(shader, 1);
    if (normal_map)
    {
        int normal_map_uniform = GetShaderLocation(shader, "normalMap");
        SetShaderValueTexture(shader, normal_map_uniform, *normal_map);
    }

    int ambient_uniform = GetShaderLocation(shader, "ambientAtt");
    SetShaderValue(shader, ambient_uniform, &ambient_attenuation, SHADER_UNIFORM_FLOAT);

    draw_tilemap();

    GameplaySystems::render_drawable_system(world, asset_m);

    if (use_shader)
    {
        EndShaderMode();
    }

    GameplaySystems::draw_ui_system(world, *this);

//    DrawCircleLines((int)(light.x), (int)(light.y), light.radius, WHITE);
//    DrawRectangle((int)(light.x), (int)(light.y), 4, 4, WHITE);

    if (debug_draw)
    {
        draw_player_debug_overlay();
        draw_tilemap_debug_overlay();
    }

    int y = 0;
    int fps = GetFPS();
    char buf[512];

    const int fontsz = 16;
    snprintf(buf, sizeof(buf), "FPS: %d", fps);
    DrawText(buf, 0, y, fontsz, RED);
    y += fontsz;

    snprintf(buf, sizeof(buf), "Light Radius: %f", light.radius);
    DrawText(buf, 0, y, fontsz, RED);
    y += fontsz;

    snprintf(buf, sizeof(buf), "Light Height: %f", light.height);
    DrawText(buf, 0, y, fontsz, RED);

    box.draw();
    mission_popup.draw(font, draw_buf);

//    EndDrawing();
    EndTextureMode();

    BeginDrawing();
    ClearBackground({20, 20, 20, 255});

    DrawTexturePro(draw_buf.tex.texture, 
    {0, 0, (float)(draw_buf.w), (float)(-(draw_buf.h))},
    {0, 0, (float)screen_width, (float)screen_height}, {0, 0}, 0, WHITE);

    EndDrawing();
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

            Tile* tile = asset_m.get_asset<Tile>(std::to_string(id));
            assert(tile);

            {
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