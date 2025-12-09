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

    animation.set_frame_interval(50);
    animation.push_texture(ASSETS_PATH"knight/knight000.png");
    animation.push_texture(ASSETS_PATH"knight/knight001.png");
    animation.push_texture(ASSETS_PATH"knight/knight002.png");

    sprite_speed = 3.0f;

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

    animation.update();
}

void Game::draw()
{
    BeginDrawing();

    ClearBackground(RAYWHITE);

    GameplaySystems::render_drawable_system(world, asset_m);

/*    std::string texture_path = animation.get_current_texture();
    Texture2D* texture = asset_m.get_texture(texture_path);
    ECS::TransformComponent* player_transform = world.transforms.get_component(player);
    if (texture && player_transform)
    {
        Rectangle src_rect = animation.get_source_rect(*texture);
        Rectangle dst_rect = {
                            player_transform->x,
                            player_transform->y,
                            static_cast<float>(player_transform->w),
                            static_cast<float>(player_transform->h)
                            };

        DrawTexturePro(*texture, src_rect, dst_rect, {0, 0}, 0, WHITE);
    }*/

    EndDrawing();
}

