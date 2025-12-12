#include "game.h"
#include "gameplay_systems.h"

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
    SetTargetFPS(70);
    
    asset_m.load_texture(ASSETS_PATH"knight/knight000.png");
    asset_m.load_texture(ASSETS_PATH"knight/knight001.png");
    asset_m.load_texture(ASSETS_PATH"knight/knight002.png");

    asset_m.print_texture_table();

    tilemap.create(screen_width / tile_width, screen_height / tile_height);

/*    for (int i = 0; i < tilemap.get_width(); i++)
    {
        tilemap.set_tile(i, tilemap.get_height() - 3, 1);
    }*/

    load_tilemap(ASSETS_PATH"map.txt");

    create_player(100, 100);
}

void Game::destroy()
{
    CloseWindow();
}

void Game::create_player(float x, float y)
{
    ECS::MovementComponent movement;
    movement.speed = 0.3f;
    movement.gravity = gravity * 2;

    ECS::DrawableComponent drawable;
    drawable.x = 0;
    drawable.y = 0;
    drawable.scale = 5;
//    drawable.w = 64;
//    drawable.h = 64;

    ECS::TransformComponent transform(x, y, 64, 64);
    Texture2D* base_texture = asset_m.get_texture(ASSETS_PATH"knight/knight000.png");
    transform.w = (int)(base_texture->width  * drawable.scale);
    transform.h = (int)(base_texture->height * drawable.scale);

    ECS::CollisionComponent collision;

    ECS::AnimatedDrawableComponent animated_drawable;
    animated_drawable.anim.set_frame_interval(100);
    animated_drawable.anim.push_texture(ASSETS_PATH"knight/knight000.png");
//    animated_drawable.anim.push_texture(ASSETS_PATH"knight/knight001.png");

    ECS::PlayerComponent player_component;
    player_component.accel = 0.03f;
    player_component.friction = 0.03f;
    player_component.jump_force = 0.5f;

    player = world.create_entity();
    world.transforms.add_component(player, std::move(transform));

    world.movements.add_component(player, std::move(movement));

    world.collisions.add_component(player, std::move(collision));

    world.drawables.add_component(player, std::move(drawable));

    world.animated_drawables.add_component(player, std::move(animated_drawable));

    world.players.add_component(player, std::move(player_component));
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

    // Draw tilemap
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

            DrawRectangle(x, y, tile_width, tile_height, BROWN);
            if (debug_draw)
            {
                DrawRectangleLinesEx(tile_rect, 2, RED);
            }
        }
    }

    GameplaySystems::render_drawable_system(world, asset_m);

    if (debug_draw)
    {
        ECS::TransformComponent* player_transform = world.transforms.get_component(player);
        if (player_transform)
        {
            DrawRectangleLinesEx(player_transform->get_rect(), 2, RED);
        }
    }

    int fps = GetFPS();
    char buf[12];
    snprintf(buf, sizeof(buf), "%d", fps);
    DrawText(buf, 0, 0, 24, RED);

    EndDrawing();
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
            if (tile == '1')
            {
                tilemap.set_tile(x, y, 1);
            }

            x++;
        }
        y++;
        x = 0;
    }
}


