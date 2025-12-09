#include "game.h"
#include "gameplay_systems.h"

#include <iostream>
#include <utility>

void Game::init()
{
    screen_width = 1280;
    screen_height = 720;
    
    InitWindow(screen_width, screen_height, "Hidden GEM");
    SetTargetFPS(70);
    
    asset_m.load_texture(ASSETS_PATH"knight/knight000.png");
    asset_m.load_texture(ASSETS_PATH"knight/knight001.png");
    asset_m.load_texture(ASSETS_PATH"knight/knight002.png");

    asset_m.print_texture_table();

    create_player(0, 0);

/*    for (int i = 1000; i < 2000; i++)
    {
        create_player(static_cast<float>(i * 4), static_cast<float>(i * 2));
    }*/
}

void Game::destroy()
{
    CloseWindow();
}

void Game::create_player(float x, float y)
{
    ECS::TransformComponent transform = {x, y, 256, 256};
    ECS::MovementComponent movement;
    movement.speed = 0.3f;

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
    world.transforms.add_component(player, std::move(transform));
    world.movements.add_component(player, std::move(movement));
    world.drawables.add_component(player, std::move(drawable));
    world.animated_drawables.add_component(player, std::move(animated_drawable));
    world.players.add_component(player, std::move(player_tag));
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

    dt = GetFrameTime() * 1000;

    using namespace GameplaySystems;
    player_system(world, dt);

    transform_update_system(world);
    animated_drawable_system(world);
}

void Game::draw()
{
    BeginDrawing();

    ClearBackground({60, 60, 60, 255});

    GameplaySystems::render_drawable_system(world, asset_m);

    int fps = GetFPS();
    char buf[12];
    snprintf(buf, sizeof(buf), "%d", fps);
    DrawText(buf, 0, 0, 48, RED);

    EndDrawing();
}

