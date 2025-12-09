#include "gameplay_systems.h"

#include <iostream>

#include <raylib.h>

using namespace ECS;

namespace GameplaySystems
{
    void transform_update_system(ECS::World& world)
    {
        for (int entity : world.transforms.entities)
        {
            TransformComponent* transform = world.transforms.get_component(entity);
            DrawableComponent* drawable = world.drawables.get_component(entity);

            if (drawable)
            {
                drawable->x = static_cast<int>(transform->x);
                drawable->y = static_cast<int>(transform->y);
            }
        }
    }

    void animated_drawable_system(ECS::World& world)
    {
        for (int entity : world.animated_drawables.entities)
        {
            AnimatedDrawableComponent* animated_drawable =
            world.animated_drawables.get_component(entity);

            DrawableComponent* drawable = world.drawables.get_component(entity);

            animated_drawable->anim.update();

            if (drawable)
            {
                // update the drawable to draw the current frame of the animation
                drawable->texture_path = animated_drawable->anim.get_current_texture();
                drawable->flip_h = animated_drawable->anim.flip_h;
                drawable->flip_v = animated_drawable->anim.flip_v;
            }
        }
    }

    void render_drawable_system(ECS::World& world, const AssetManager& asset_m)
    {
        for (DrawableComponent& drawable : world.drawables.components)
        {
//            std::cout << "Draw " << drawable.texture_path << std::endl;
            Texture2D* texture = asset_m.get_texture(drawable.texture_path);

            if (texture == nullptr) continue;

            Rectangle src_rect = {
                0,
                0,
                static_cast<float>(texture->width),
                static_cast<float>(texture->height)
            };

            if (drawable.flip_h) src_rect.width  = -(src_rect.width);
            if (drawable.flip_v) src_rect.height = -(src_rect.height);

            Rectangle dst_rect = {
                static_cast<float>(drawable.x),
                static_cast<float>(drawable.y),
                static_cast<float>(drawable.w),
                static_cast<float>(drawable.h)
            };

            DrawTexturePro(*texture, src_rect, dst_rect, {0, 0}, 0, WHITE);
        }
    }

    void player_system(ECS::World& world, float dt)
    {
        for (int entity : world.players.entities)
        {
            TransformComponent* transform = world.transforms.get_component(entity);
            if (transform == nullptr) continue;

            MovementComponent* movement = world.movements.get_component(entity);
            if (movement == nullptr) continue;

            AnimatedDrawableComponent* animated_drawable =
            world.animated_drawables.get_component(entity);
            if (animated_drawable == nullptr) continue;

            if (IsKeyDown(KEY_D))
            {
                transform->x += movement->speed * dt;
                animated_drawable->anim.flip_h = false;
            }
            else if (IsKeyDown(KEY_A))
            {
                transform->x -= movement->speed * dt;
                animated_drawable->anim.flip_h = true;
            }

            if (IsKeyDown(KEY_S))
            {
                transform->y += movement->speed * dt;
            }
            else if (IsKeyDown(KEY_W))
            {
                transform->y -= movement->speed * dt;
            }
        }
    }
}