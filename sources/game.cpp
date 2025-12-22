#include "game.h"
#include "gameplay_systems.h"

#include "vendor/json.hpp"

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

    load_tileset(ASSETS_PATH"tileset.json");

    tilemap.create(screen_width / tile_width, screen_height / tile_height);
    load_tilemap(ASSETS_PATH"map.txt");

    create_player(100, 100);
}

void Game::destroy()
{
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

    using namespace GameplaySystems;
    player_system(world);

    collision_update_system(world, *this, tilemap);
    transform_update_system(world, *this);
    animated_drawable_system(world, asset_m);
}

void Game::draw()
{
    BeginDrawing();

    ClearBackground({60, 60, 60, 255});

    draw_tilemap();

    GameplaySystems::render_drawable_system(world, asset_m);

    if (debug_draw)
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

    int fps = GetFPS();
    char buf[12];
    snprintf(buf, sizeof(buf), "%d", fps);
    DrawText(buf, 0, 0, 24, RED);

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
                    DrawTexturePro(*texture, {0, 0, (float)(texture->width), (float)(texture->height)},
                    tile_rect, {0, 0}, 0, WHITE);
                }
                else
                {
                    DrawRectangle(x, y, tile_width, tile_height, BLACK);
                }
            }

            if (debug_draw)
            {
                DrawRectangleLinesEx(tile_rect, 2, RED);
            }
        }
    }
}

void Game::load_tilemap(const std::string& filepath)
{
    std::ifstream stream(filepath);
    
    std::string line;
    int x = 0;
    int y = 0;
    while (getline(stream, line))
    {
        std::cout << line << std::endl;
        for (char tile : line)
        {
            if ((tile >= '0') && (tile <= '9'))
            {
                tilemap.set_tile(x, y, tile - '0');
            }

            x++;
        }
        y++;
        x = 0;
    }
}

void Game::load_tileset(const std::string& filepath)
{
    using namespace nlohmann;

    std::ifstream file(filepath);
    if (file.is_open())
    {
        json data = json::parse(file);
        for (const auto& tile : data)
        {
            if (tile.contains("id") == false)
            {
                continue;
            }
            if (tile["id"].is_number_integer() == false)
            {
                continue;
            }

            int id = tile["id"];

            if (tile.contains("texture_id"))
            {
                if (tile["texture_id"].is_string())
                {
                    std::string texture_id = tile["texture_id"];
                    texture_id.insert(0, ASSETS_PATH);

                    asset_m.load_texture(texture_id);

                    Tile tile_data;
                    tile_data.id = id;
                    tile_data.texture_id = texture_id;

                    asset_m.load_tile(tile_data);
                }
            }
        }
    }
}