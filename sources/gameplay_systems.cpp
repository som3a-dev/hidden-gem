#include "gameplay_systems.h"
#include "collision.h"

#include <iostream>
#include <math.h>

#include <raylib.h>

using namespace ECS;

namespace GameplaySystems
{
    void transform_update_system(ECS::World& world, const Game& game)
    {
        for (int entity : world.transforms.entities)
        {
            TransformComponent* transform = world.transforms.get_component(entity);
            DrawableComponent* drawable = world.drawables.get_component(entity);
            MovementComponent* movement = world.movements.get_component(entity);

            if (movement)
            {
                transform->x += movement->velocity.x * game.dt;
                transform->y += movement->velocity.y * game.dt;
            }

            if (drawable)
            {
                drawable->x = static_cast<int>(transform->x);
                drawable->y = static_cast<int>(transform->y);
            }
        }
    }

    void collision_update_system(ECS::World& world, const Game& game, const Tilemap& tilemap)
    {
        for (int entity : world.collisions.entities)
        {
            CollisionComponent* collision = world.collisions.get_component(entity);

            MovementComponent* movement = world.movements.get_component(entity);
            if (movement == nullptr) continue;
            TransformComponent* transform = world.transforms.get_component(entity);
            if (transform == nullptr) continue;

            Rectangle entity_rect = transform->get_rect();

            entity_rect.x += movement->velocity.x * game.dt;
            int collision_count = Collision::get_colliding_tiles(game, tilemap, entity_rect,
                                           collision->_colliding_tiles.data(),
                                           collision->_colliding_tiles.size());

//            for (Rectangle& tile_rect : collision->_colliding_tiles)
            for (int i = 0; i < collision_count; i++)
            {
                Rectangle& tile_rect = collision->_colliding_tiles[i];

                float tile_center_x = tile_rect.x + (tile_rect.width / 2);
                float entity_center_x = entity_rect.x + (entity_rect.width / 2);

                if (entity_center_x < tile_center_x)
                {
                    transform->x = tile_rect.x - transform->w;
                }
                else if (tile_center_x < entity_center_x)
                {
                    transform->x = tile_rect.x + tile_rect.width;
                }

                movement->velocity.x = 0;
                break;
            }

            entity_rect.x = transform->x; // Check the actual current x position, in case we snapped

            entity_rect.y += movement->velocity.y * game.dt;
            collision_count = Collision::get_colliding_tiles(game, tilemap, entity_rect,
                                           collision->_colliding_tiles.data(),
                                           collision->_colliding_tiles.size());

            collision->on_ground = false;
            for (int i = 0; i < collision_count; i++)
            {
                Rectangle& tile_rect = collision->_colliding_tiles[i];

                float tile_center_y = tile_rect.y + (tile_rect.height / 2);
                float entity_center_y = entity_rect.y + (entity_rect.height / 2);

                if (tile_center_y < entity_center_y)
                {
                    transform->y = tile_rect.y + tile_rect.height;
                }
                else if (entity_center_y < tile_center_y)
                {
                    transform->y = tile_rect.y - transform->h;
                    collision->on_ground = true;
                }

                movement->velocity.y = 0;
                break;
            }
        }
    }

    void animated_drawable_system(ECS::World& world, const AssetManager& asset_m)
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

                Texture2D* texture = asset_m.get_asset<Texture2D>(drawable->texture_path);
                drawable->w = texture->width;
                drawable->h = texture->height;
            }
        }
    }

    void render_drawable_system(ECS::World& world, const AssetManager& asset_m)
    {
        for (DrawableComponent& drawable : world.drawables.components)
        {
//            std::cout << "Draw " << drawable.texture_path << std::endl;
            Texture2D* texture = asset_m.get_asset<Texture2D>(drawable.texture_path);

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
                static_cast<float>(drawable.w) * drawable.scale,
                static_cast<float>(drawable.h) * drawable.scale
            };

            DrawTexturePro(*texture, src_rect, dst_rect, {0, 0}, 0, WHITE);
        }
    }

    void player_system(ECS::World& world)
    {
        for (int entity : world.players.entities)
        {
            PlayerComponent* player = world.players.get_component(entity);

            TransformComponent* transform = world.transforms.get_component(entity);
            if (transform == nullptr) continue;

            MovementComponent* movement = world.movements.get_component(entity);
            if (movement == nullptr) continue;

            CollisionComponent* collision = world.collisions.get_component(entity);
            if (collision == nullptr) continue;

            AnimatedDrawableComponent* animated_drawable =
            world.animated_drawables.get_component(entity);
            if (animated_drawable == nullptr) continue;

            DrawableComponent* drawable = world.drawables.get_component(entity);
            if (drawable == nullptr) continue;

            // Gravity
            if (collision->on_ground == false)
            {
                movement->velocity.y += movement->gravity;
            }
            else
            {
                movement->velocity.y = 0;
            }

            if (IsKeyDown(KEY_D))
            {
                movement->velocity.x += player->accel;
                if (movement->velocity.x > movement->speed)
                {
                    movement->velocity.x = movement->speed;
                }

                animated_drawable->anim.flip_h = false;
            }
            else if (IsKeyDown(KEY_A))
            {
                movement->velocity.x -= player->accel;
                if (movement->velocity.x < -(movement->speed))
                {
                    movement->velocity.x = -(movement->speed);
                }

                animated_drawable->anim.flip_h = true;
            }
            else
            {
                if (movement->velocity.x > 0)
                {
                    movement->velocity.x -= player->friction;
                    if (movement->velocity.x < 0) movement->velocity.x = 0;
                }

                else if (movement->velocity.x < 0)
                {
                    movement->velocity.x += player->friction;
                    if (movement->velocity.x > 0) movement->velocity.x = 0;
                }
            }

            if (IsKeyPressed(KEY_SPACE))
            {
                movement->velocity.y = -(player->jump_force);
            }

            if (IsKeyPressed(KEY_K))
            {
                transform->x = 100;
                transform->y = 100;
            }
        }
    }
}