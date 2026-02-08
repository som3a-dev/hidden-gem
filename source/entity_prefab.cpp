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
    drawable.scale = 2.5f;

    ECS::TransformComponent transform = {x, y};

    ECS::MovementComponent movement;
    movement.speed = 150.0f;
    movement.gravity = gravity;

    ECS::CollisionComponent collision;
    collision.rect.x = 32 * (2.5f / 3);
    collision.rect.y = 48 * (2.5f / 3);
    collision.rect.width = 32 * (2.5f / 3); 
    collision.rect.height = 36 * (2.5f / 3);

    ECS::AnimatedDrawableComponent animated_drawable;
    animated_drawable.animation_id = "knight_idle";

    ECS::PlayerComponent player_component;
    player_component.accel = movement.speed * 4;
    player_component.friction = movement.speed * 4;
    player_component.jump_force = 300.0f;

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
    drawable.scale = 1;

    ECS::AnimatedDrawableComponent animated_drawable;
    animated_drawable.animation_id = "torch_idle";

    int entity = world.create_entity();
    world.transforms.add_component(entity, std::move(transform));
    world.drawables.add_component(entity, std::move(drawable));
    world.animated_drawables.add_component(entity, std::move(animated_drawable));

    light.x = transform.x + 24;
    light.y = transform.y + 12;
}

void Game::create_table(float x, float y)
{
    ECS::TransformComponent transform = {x, y};
    ECS::DrawableComponent drawable;
    drawable.scale = 0.2f;
    drawable.texture_path = ASSETS_PATH"table2.png";

    ECS::InteractableComponent interactable;

    int entity = world.create_entity();
    world.transforms.add_component(entity, std::move(transform));
    world.drawables.add_component(entity, std::move(drawable));
    world.interactables.add_component(entity, std::move(interactable));
}