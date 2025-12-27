#include "game.h"

void Game::create_player(float x, float y)
{
    {
        FrameAnimation anim;
        anim.set_sheet(ASSETS_PATH"brackeys_platformer_assets/sprites/knight.png", asset_m, 8, 8);
/*        anim.frames.push_back({0, 2});
        anim.frames.push_back({1, 2});
        anim.frames.push_back({2, 2});
        anim.frames.push_back({3, 2});
        anim.frames.push_back({4, 2});
        anim.frames.push_back({5, 2});
        anim.frames.push_back({6, 2});
        anim.frames.push_back({7, 2});*/
        anim.push_frame_interval(0, 7, 2, 2);

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

void Game::create_torch(float x, float y)
{
    ECS::TransformComponent transform = {x, y};
    ECS::DrawableComponent drawable;
    drawable.scale = 3;
    transform.y -= 56 * drawable.scale;

    ECS::AnimatedDrawableComponent animated_drawable;
    animated_drawable.animation_id = "torch_idle";

    int entity = world.create_entity();
    world.transforms.add_component(entity, std::move(transform));
    world.drawables.add_component(entity, std::move(drawable));
    world.animated_drawables.add_component(entity, std::move(animated_drawable));

    light.x = transform.x + 64;
    light.y = transform.y + 32;
}