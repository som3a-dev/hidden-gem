#include "game.h"
#include "gameplay_systems.h"

#include <iostream>

void Game::init()
{
    screen_width = 1280;
    screen_height = 720;
    
    InitWindow(screen_width, screen_height, "Hidden GEM");
    SetTargetFPS(60);

    asset_m.load_texture(ASSETS_PATH"knight/knight000.png");
    asset_m.print_texture_table();
    asset_m.load_texture(ASSETS_PATH"knight/knight001.png");
    asset_m.print_texture_table();
    asset_m.load_texture(ASSETS_PATH"knight/knight002.png");
    asset_m.print_texture_table();

    create_player();
}

void Game::destroy()
{
    CloseWindow();
}

void Game::create_player()
{
    ECS::TransformComponent transform = {50, 50, 256, 256};
    ECS::MovementComponent movement;
    movement.speed = 4.0f;

    ECS::DrawableComponent drawable;
    drawable.texture_path = ASSETS_PATH"knight/knight002.png";
    drawable.x = 0;
    drawable.y = 0;
    drawable.w = 100;
    drawable.h = 100;

    ECS::AnimatedDrawableComponent animated_drawable;
    animated_drawable.anim.set_frame_interval(50);
    animated_drawable.anim.push_texture(ASSETS_PATH"knight/knight000.png");
    animated_drawable.anim.push_texture(ASSETS_PATH"knight/knight001.png");
    animated_drawable.anim.push_texture(ASSETS_PATH"knight/knight002.png");

    ECS::PlayerComponent player_tag;

    player = world.create_entity();
    world.transforms.add_component(player, transform);
    world.movements.add_component(player, movement);
    world.drawables.add_component(player, drawable);
    world.animated_drawables.add_component(player, animated_drawable);
    world.players.add_component(player, player_tag);
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

    using namespace GameplaySystems;
    player_system(world);

    transform_update_system(world);
    animated_drawable_system(world);
}

void Game::draw()
{
    BeginDrawing();

    ClearBackground(RAYWHITE);

    GameplaySystems::render_drawable_system(world, asset_m);

    EndDrawing();
}

