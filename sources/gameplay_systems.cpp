#include "gameplay_systems.h"

#include <iostream>
#include <math.h>

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

    void movement_update_system(ECS::World& world, const Game& game, const Tilemap& tilemap)
    {
        for (int entity : world.movements.entities)
        {
            MovementComponent* movement = world.movements.get_component(entity);
            (void)movement;

            TransformComponent* transform = world.transforms.get_component(entity);

            if (transform == nullptr) continue;

            Rectangle entity_rect = transform->get_rect();
            
            transform->x += movement->velocity.x * game.dt;

            //Check for x collision
            if (movement->velocity.x)
            {
                for (int tile_y = 0; tile_y < tilemap.get_height(); tile_y++)
                {
                    for (int tile_x = 0; tile_x < tilemap.get_width(); tile_x++)
                    {
                        if (tilemap.get_tile(tile_x, tile_y) == EMPTY_TILE)
                        {
                            continue;
                        }

                        Rectangle tile_rect = {
                            (float)(tile_x) * game.tile_width,
                            (float)(tile_y) * game.tile_height,
                            (float)(game.tile_width),
                            (float)(game.tile_height)
                        };

                        if (CheckCollisionRecs(tile_rect, entity_rect))
                        {
                            float x_overlap = __min(entity_rect.x + entity_rect.width,
                                tile_rect.x + tile_rect.width) - __max(entity_rect.x, tile_rect.x);

                            float y_overlap = __min(entity_rect.y + entity_rect.height,
                                tile_rect.y + tile_rect.height) - __max(entity_rect.y, tile_rect.y);

                            std::cout << x_overlap << ", " << y_overlap << std::endl;
                            
                            if (y_overlap > x_overlap)
                            {
                                if (movement->velocity.x > 0)
                                {
                                    transform->x = tile_rect.x + transform->w;
                                }
                                else if (movement->velocity.x < 0)
                                {
                                    transform->x = tile_rect.x + tile_rect.width;
                                }
                            }

                            goto y_collision_check;
                        }
                    }
                }
            }

            y_collision_check:

            transform->y += movement->velocity.y * game.dt;
            //Check for y collision
            if (movement->velocity.y)
            {
                movement->on_ground = false; // gets set to false at the start, true by the end if still on ground this frame

                for (int tile_y = 0; tile_y < tilemap.get_height(); tile_y++)
                {
                    for (int tile_x = 0; tile_x < tilemap.get_width(); tile_x++)
                    {
                        if (tilemap.get_tile(tile_x, tile_y) == EMPTY_TILE)
                        {
                            continue;
                        }

                        Rectangle tile_rect = {
                            (float)(tile_x) * game.tile_width,
                            (float)(tile_y) * game.tile_height,
                            (float)(game.tile_width),
                            (float)(game.tile_height)
                        };

                        if (CheckCollisionRecs(tile_rect, entity_rect))
                        {
                            float x_overlap = __min(entity_rect.x + entity_rect.width,
                                tile_rect.x + tile_rect.width) - __max(entity_rect.x, tile_rect.x);

                            float y_overlap = __min(entity_rect.y + entity_rect.height,
                                tile_rect.y + tile_rect.height) - __max(entity_rect.y, tile_rect.y);

                            std::cout << x_overlap << ", " << y_overlap << std::endl;
                            
                            if (x_overlap > y_overlap)
                            {
                                if (movement->velocity.y < 0)
                                {
                                    transform->y = tile_rect.y + transform->h;
                                }
                                else if (movement->velocity.y > 0)
                                {
                                    transform->y = tile_rect.y - transform->h;
                                    movement->on_ground = true;
                                }
                            }

                            goto collision_check_done;
                        }
                    }
                }
            }

            collision_check_done:
            return;
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

                Texture2D* texture = asset_m.get_texture(drawable->texture_path);
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

            AnimatedDrawableComponent* animated_drawable =
            world.animated_drawables.get_component(entity);
            if (animated_drawable == nullptr) continue;

            DrawableComponent* drawable = world.drawables.get_component(entity);
            if (drawable == nullptr) continue;

            // Gravity
            if (movement->on_ground == false)
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
        }
    }
}