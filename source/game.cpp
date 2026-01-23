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
    screen_width = 1280;
    screen_height = 720;
    tile_width = 64;
    tile_height = 64;

    gravity = 0.01f;
    
    InitWindow(screen_width, screen_height, "Hidden GEM");
    SetTargetFPS(60);
    
    asset_m.load_texture(ASSETS_PATH"brackeys_platformer_assets/sprites/knight.png");
    asset_m.load_texture(ASSETS_PATH"brackeys_platformer_assets/sprites/block.png");
    asset_m.load_texture(ASSETS_PATH"normal_map.png");
    asset_m.load_texture(ASSETS_PATH"torch.png");

    FrameAnimation anim;
    anim.set_sheet(ASSETS_PATH"torch.png", asset_m, 4, 2);
    anim.interval_ms = 100;
    anim.push_frame_interval(0, 3, 0, 3);

    asset_m.load_frame_animation("torch_idle", std::move(anim));

    load_tileset(ASSETS_PATH"tileset.json");

    tilemap.create(screen_width / tile_width * 2, screen_height / tile_height * 2);
    load_tilemap(ASSETS_PATH"map.hgm");

    create_player(100, 100);

    shader = LoadShader(ASSETS_PATH"shaders/vertex.vs", ASSETS_PATH"shaders/fragment.fs");
    light.radius = 1200;
    light.color = {1.0f, 0.7f, 0.4f};
    light.height = 400.0f;
    light.ambient_attenuation = 0.03f;
}

void Game::destroy()
{
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

    if (IsKeyPressed(KEY_L))
    {
        debug_draw = !debug_draw;
    }

    dt = GetFrameTime() * 1000;

//    light.x += 0.1f * dt;
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
    player_system(world);

    collision_update_system(world, *this, tilemap);
    transform_update_system(world, *this);
    animated_drawable_system(world, asset_m);

    current_normal_map = ASSETS_PATH"normal_map.png";
}

void Game::draw()
{
    Texture2D* normal_map = asset_m.get_asset<Texture2D>(current_normal_map);

    BeginDrawing();
    ClearBackground({0, 0, 0, 255});

    BeginShaderMode(shader);

    light.set_uniforms(shader);
    if (normal_map)
    {
        int normal_map_uniform = GetShaderLocation(shader, "normalMap");
        SetShaderValueTexture(shader, normal_map_uniform, *normal_map);
    }

    draw_tilemap();

    GameplaySystems::render_drawable_system(world, asset_m);

    EndShaderMode();

    DrawCircleLines((int)(light.x), (int)(light.y), light.radius, WHITE);
//    DrawRectangle((int)(light.x), (int)(light.y), 4, 4, WHITE);

    if (debug_draw)
    {
        draw_player_debug_overlay();
        draw_tilemap_debug_overlay();
    }

    int y = 0;
    int fps = GetFPS();
    char buf[512];

    snprintf(buf, sizeof(buf), "FPS: %d", fps);
    DrawText(buf, 0, y, 24, RED);
    y += 24;

    snprintf(buf, sizeof(buf), "Light Radius: %f", light.radius);
    DrawText(buf, 0, y, 24, RED);
    y += 24;

    snprintf(buf, sizeof(buf), "Light Height: %f", light.height);
    DrawText(buf, 0, y, 24, RED);

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
            if (tile)
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
        DrawRectangleLinesEx(rect, 2, RED);
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

            int x = tile_x * tile_width;
            int y = tile_y * tile_height;

            Rectangle tile_rect = {
                (float)x,
                (float)y,
                (float)tile_width,
                (float)tile_height
            };

            DrawRectangleLinesEx(tile_rect, 2, RED);
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
            else if (tile == 3)
            {
                create_torch((float)(x * tile_width), (float)(y * tile_height));
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

        asset_m.load_texture(tile.texture_id);
        asset_m.load_tile(tile);
    }

    mf_load_tileset_free(&tiles, tiles_count);
}