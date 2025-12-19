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
    SetTargetFPS(60);
    
    asset_m.load_texture(ASSETS_PATH"knight/knight000.png");
    asset_m.load_texture(ASSETS_PATH"knight/knight001.png");
    asset_m.load_texture(ASSETS_PATH"knight/knight002.png");
    asset_m.load_texture(ASSETS_PATH"brackeys_platformer_assets/sprites/knight.png");

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
    {
        FrameAnimation anim;
        anim.set_sheet(ASSETS_PATH"brackeys_platformer_assets/sprites/knight.png", asset_m, 8, 8);
        anim.frames.push_back({0, 2});
        anim.frames.push_back({1, 2});
        anim.frames.push_back({2, 2});
        anim.frames.push_back({3, 2});
        anim.frames.push_back({4, 2});
        anim.frames.push_back({5, 2});
        anim.frames.push_back({6, 2});
        anim.frames.push_back({7, 2});

        anim.interval_ms = 70;

        asset_m.load_frame_animation("knight_walk", std::move(anim));
    }
    {
        FrameAnimation anim;
        anim.set_sheet(ASSETS_PATH"brackeys_platformer_assets/sprites/knight.png", asset_m, 8, 8);
        anim.frames.push_back({0, 0});
        anim.frames.push_back({1, 0});
//        anim.frames.push_back({2, 0});
//        anim.frames.push_back({3, 0});

        anim.interval_ms = 300;

        asset_m.load_frame_animation("knight_idle", std::move(anim));
    }

    ECS::DrawableComponent drawable;
    drawable.scale = 5;

    ECS::TransformComponent transform = {x, y};

    ECS::MovementComponent movement;
    movement.speed = 0.3f;
    movement.gravity = gravity * 2;

    ECS::CollisionComponent collision;
    collision.rect.x = 50;
    collision.rect.y = 60;
    collision.rect.width = 60; // default/fallback size
    collision.rect.height = 80;

    ECS::AnimatedDrawableComponent animated_drawable;
    animated_drawable.animation_id = "knight_idle";

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

    FrameAnimation* anim = asset_m.get_asset<FrameAnimation>("knight_walk");

    if (anim)
    {
        anim->update();

        Rectangle src = anim->get_current_frame_src();
        Rectangle dst = {
            100, 100,
            src.width * 4, src.height * 4
        };

        Texture2D* sheet = asset_m.get_asset<Texture2D>(anim->get_sheet());

        if (sheet)
        {
            DrawTexturePro(*sheet, src, dst, {0, 0}, 0, WHITE);
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

            DrawRectangle(x, y, tile_width, tile_height, BROWN);
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


