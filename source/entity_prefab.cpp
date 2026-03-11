#include "game.h"

void Game::create_player(float x, float y)
{
/*    {
        FrameAnimation anim;
        anim.set_sheet(ASSETS_PATH"brackeys_platformer_assets/sprites/knight.png", asset_m, 8, 8);
        anim.push_frame_interval(0, 7, 2, 2);

        anim.interval_ms = 70;

        asset_m.load_frame_animation("knight_walk", std::move(anim));
    }
    {
        FrameAnimation anim;
        anim.set_sheet(ASSETS_PATH"brackeys_platformer_assets/sprites/knight.png", asset_m, 8, 8);
        anim.frames.push_back({0, 0});
        anim.frames.push_back({1, 0});

        anim.interval_ms = 300;

        asset_m.load_frame_animation("knight_idle", std::move(anim));
    } */

    const char* sheet = ASSETS_PATH"player/soldier-plain.png";
    FrameAnimation anim;
    ECS::CollisionComponent collision;
    ECS::DrawableComponent drawable;
    drawable.scale = 2.0f;

    anim.set_sheet(sheet, asset_m, 8, 7);
    {
        anim.frames.clear();
        anim.push_frame_interval(1, 3, 1, 1);
        anim.interval_ms = 300;

        asset_m.load_frame_animation("soldier-idle", anim);

        collision.rect = {
            0, 0,
            (float)anim.get_frame_width() * drawable.scale, (float)anim.get_frame_height() * drawable.scale
        };
    }
    {
        anim.frames.clear();
/*        anim.frames.push_back({1, 2});
        anim.frames.push_back({3, 2});
        anim.frames.push_back({4, 2});
        anim.frames.push_back({6, 2});*/
        anim.push_frame_interval(1, 6, 2, 2);
        anim.interval_ms = 100;

        asset_m.load_frame_animation("soldier-walk", anim);
    }



    ECS::TransformComponent transform = {x, y};

    ECS::MovementComponent movement;
    movement.speed = 120.0f;
    movement.gravity = gravity;

//    collision.rect.x = 32 * (2.5f / 3);
//    collision.rect.y = 48 * (2.5f / 3);
//    collision.rect.width = 32 * (2.5f / 3); 
//    collision.rect.height = 36 * (2.5f / 3);

    ECS::AnimatedDrawableComponent animated_drawable;
    animated_drawable.animation_id = "soldier-idle";

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

    torch_light.x = transform.x;
    torch_light.y = transform.y;
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

void Game::create_window(float x, float y) 
{
    ECS::TransformComponent transform;
    transform.x = x;
    transform.y = y;

    window_light.x = x;
    window_light.y = y;

    ECS::DrawableComponent drawable;
    drawable.scale = 0.5f;
    drawable.texture_path = ASSETS_PATH"win_wood.png";

    int entity = world.create_entity();
    world.transforms.add_component(entity, std::move(transform));
    world.drawables.add_component(entity, std::move(drawable));
}