#include "gameplay_systems.h"
#include "collision.h"

#include <assert.h>
#include <iostream>
#include <math.h>

#include <raylib.h>

using namespace ECS;

/*
    SOME NOTES

    - When calculating the position of a player, sometimes we get the collision position
    rather than just using the transform x and y, thats because the transform x and y are just
    wrong visually due to sprites having empty space,
    meanwhile the collision rect is hardcoded to be right with a lot of
    trial and error, i don't know how we should fix this and 
    don't care right now but this should be fixed in the future

    - FIX DELTA TIME, THE GAME IS FRAME DEPENDENT
*/

namespace GameplaySystems
{
    void transform_update_system(Game& game)
    {
        for (int entity : game.world.transforms.entities)
        {
            TransformComponent* transform = game.world.transforms.get_component(entity);
            DrawableComponent* drawable = game.world.drawables.get_component(entity);
            MovementComponent* movement = game.world.movements.get_component(entity);

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

    void collision_update_system(Game& game)
    {
        for (int entity : game.world.collisions.entities)
        {
            CollisionComponent* collision = game.world.collisions.get_component(entity);

            MovementComponent* movement = game.world.movements.get_component(entity);
            if (movement == nullptr) continue;
            TransformComponent* transform = game.world.transforms.get_component(entity);
            if (transform == nullptr) continue;
 
            Rectangle entity_rect = {
                transform->x + collision->rect.x,
                transform->y + collision->rect.y,
                collision->rect.width,
                collision->rect.height
            };

            entity_rect.x += movement->velocity.x * game.dt;
            int collision_count = Collision::get_colliding_tiles(game, game.tilemap, entity_rect,
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
                    transform->x = tile_rect.x - (entity_rect.width + collision->rect.x);
                }
                else if (tile_center_x < entity_center_x)
                {
                    transform->x = tile_rect.x + tile_rect.width - collision->rect.x;
                }

                movement->velocity.x = 0;
                break;
            }

            entity_rect.x = transform->x + collision->rect.x; // Check the actual current x position, in case we snapped

            entity_rect.y += movement->velocity.y * game.dt;
            collision_count = Collision::get_colliding_tiles(game, game.tilemap, entity_rect,
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
                    transform->y = tile_rect.y + tile_rect.height - collision->rect.y;
                }
                else if (entity_center_y < tile_center_y)
                {
                    transform->y = tile_rect.y - (entity_rect.height + collision->rect.y);
                    collision->on_ground = true;
                }

                movement->velocity.y = 0;
                break;
            }
        }
    }

    void interactable_update_system(Game& game)
    {
        // TODO(omar): maybe this entire model should be changed, and we should have
        // players looking for nearby interactables, storing the data, handling everything
        // and interactables should only have a interaction radius and no system
        for (int entity : game.world.interactables.entities)        
        {
            InteractableComponent* comp = game.world.interactables.get_component(entity);
            TransformComponent* trans = game.world.transforms.get_component(entity);

            assert(trans);

            for (int player : game.world.players.entities)
            {
                TransformComponent* player_trans = game.world.transforms.get_component(player);
                CollisionComponent* player_col = game.world.collisions.get_component(player);
                assert(player_trans);
                assert(player_col);

                Vector2 player_pos = {
                    player_trans->x + player_col->rect.x,
                    player_trans->y + player_col->rect.y
                };

                Vector2 delta = {player_pos.x - trans->x, player_pos.y - trans->y};
                double dist = sqrt(pow(delta.x, 2) + pow(delta.y, 2));

                if (dist < 70)
                {
                    comp->active = true;
                    comp->close_player_id = player;

                    comp->popup_scale += 0.1f; 
                    if (comp->popup_scale > 1)
                    {
                        comp->popup_scale = 1;
                    }

                    if (IsKeyPressed(KEY_K))
                    {
                        // TODO(): Remove this damn hack
                        game.question_panel.flip();
                    }

                    break;
                }
                else
                {
                    comp->active = false;
                    comp->close_player_id = ENTITY_INVALID;

                    comp->popup_scale -= 0.1f;
                    if (comp->popup_scale < 0)
                    {
                        comp->popup_scale = 0;
                    }

                }
            }
        }
    }

    void animated_drawable_system(Game& game)
    {
        for (int entity : game.world.animated_drawables.entities)
        {
            AnimatedDrawableComponent* animated_drawable =
            game.world.animated_drawables.get_component(entity);

            DrawableComponent* drawable = game.world.drawables.get_component(entity);

            if (animated_drawable->animation_id.empty())
            {
                continue;
            }
            else if (animated_drawable->animation.id != animated_drawable->animation_id)
            {
                FrameAnimation* og_animation = game.asset_m.get_asset<FrameAnimation>
                (animated_drawable->animation_id);

                assert(og_animation);
                animated_drawable->animation = *(og_animation);
            }

            if (drawable)
            {
                // update the drawable to draw the current frame of the animation
                drawable->texture_path = animated_drawable->animation.get_sheet();

                drawable->w = animated_drawable->animation.get_frame_width();
                drawable->h = animated_drawable->animation.get_frame_height();

                drawable->source = animated_drawable->animation.get_current_frame_src();
            }

            animated_drawable->animation.update();
        }
    }

    void render_drawable_system(Game& game)
    {
        for (DrawableComponent& drawable : game.world.drawables.components)
        {
//            std::cout << "Draw " << drawable.texture_path << std::endl;
            Texture2D* texture = game.asset_m.get_asset<Texture2D>(drawable.texture_path);

            if (texture == nullptr) continue;

            Rectangle src_rect = {
                0,
                0,
                static_cast<float>(texture->width),
                static_cast<float>(texture->height)
            };

            if (drawable.source.width)
            {
                src_rect = drawable.source;
            }

            if (drawable.flip_h) src_rect.width  = -(src_rect.width);
            if (drawable.flip_v) src_rect.height = -(src_rect.height);

            Rectangle dst_rect = {
                static_cast<float>(drawable.x),
                static_cast<float>(drawable.y),
                static_cast<float>(drawable.w) * drawable.scale,
                static_cast<float>(drawable.h) * drawable.scale
            };

            if ((drawable.w == 0) || (drawable.h == 0))
            {
                // Use texture's size
                dst_rect.width = src_rect.width * drawable.scale;
                dst_rect.height = src_rect.height * drawable.scale;

                drawable.w = (int)(src_rect.width);
                drawable.h = (int)(src_rect.height);
            }
            
            dst_rect.x -= floorf(game.camera.x);
            dst_rect.y -= floorf(game.camera.y);

            dst_rect.x = floorf(dst_rect.x);
            dst_rect.y = floorf(dst_rect.y);

            DrawTexturePro(*texture, src_rect, dst_rect, {0, 0}, 0, WHITE);
        }
    }

    void draw_ui_system(Game& game)
    {
        Texture* interact_tex = game.asset_m.get_asset<Texture>(ASSETS_PATH"emote22-smol.png");
        int interact_tex_width = interact_tex->width * 3;
        int interact_tex_height = interact_tex->height * 3;

        for (int entity : game.world.interactables.entities)
        {
            InteractableComponent* comp = game.world.interactables.get_component(entity);
            TransformComponent* trans = game.world.transforms.get_component(entity);
            DrawableComponent* drawable = game.world.drawables.get_component(entity);

            assert(trans);
            assert(drawable);

            if (comp->popup_scale)
            {
                const float scale = 1;

                Vector2 popup_pos;
                popup_pos.x = trans->x +
                (drawable->w * drawable->scale / 2) - (interact_tex_width * scale / 2);
                popup_pos.y = trans->y - interact_tex_height * scale;

                const float float_strength = 8.0f;
                const float speed = 4;

                popup_pos.y = popup_pos.y + std::sin((float)(GetTime()) * speed) * float_strength;

                popup_pos.x -= game.camera.x;
                popup_pos.y -= game.camera.y;

//                printf("%f", popup_pos.y);

//                printf(", %f\n", roundf(popup_pos.y));

                DrawTexturePro(*interact_tex, {0, 0, (float)(interact_tex->width), (float)(interact_tex->height)},
                {roundf(popup_pos.x), roundf(popup_pos.y),
                (float)(interact_tex_width * comp->popup_scale * scale), (float)(interact_tex_height * comp->popup_scale * scale)},
                {0, 0}, 0, {180, 180, 180, 255});
            }
        }

        for (int player : game.world.players.entities)
        {
            for (int interactable : game.world.interactables.entities)
            {
                InteractableComponent* interact = game.world.interactables.get_component(interactable);

                if (interact->close_player_id == player)
                {
                    TransformComponent* trans = game.world.transforms.get_component(player);
                    CollisionComponent* col = game.world.collisions.get_component(player);

                    assert(trans);
                    assert(col);

                    int text_x = (int)(trans->x + col->rect.x + col->rect.width - game.camera.x);
                    int text_y = (int)(trans->y + col->rect.y - game.camera.y); 

                    game.QueueTextEx(game.font, "[K] Interact", {(float)(text_x + 1), (float)(text_y + 1)}, 10, 1, DARKGRAY);
                    game.QueueTextEx(game.font, "[K] Interact", {(float)text_x, (float)text_y}, 10, 1, WHITE);
                    break;
                }
            }
        }
    }

    void player_system(Game& game)
    {
        for (int entity : game.world.players.entities)
        {
            PlayerComponent* player = game.world.players.get_component(entity);

            TransformComponent* transform = game.world.transforms.get_component(entity);
            if (transform == nullptr) continue;

            MovementComponent* movement = game.world.movements.get_component(entity);
            if (movement == nullptr) continue;

            CollisionComponent* collision = game.world.collisions.get_component(entity);
            if (collision == nullptr) continue;

            AnimatedDrawableComponent* animated_drawable =
            game.world.animated_drawables.get_component(entity);
            if (animated_drawable == nullptr) continue;

            DrawableComponent* drawable = game.world.drawables.get_component(entity);
            if (drawable == nullptr) continue;

            // Gravity
            if (collision->on_ground == false)
            {
                movement->velocity.y += movement->gravity * game.dt;
                if (movement->velocity.y > movement->gravity)
                {
                    movement->velocity.y = movement->gravity;
                }
            }
            else
            {
                movement->velocity.y = 0;
            }

            if (IsKeyDown(KEY_D))
            {
                movement->velocity.x += player->accel * game.dt;
                if (movement->velocity.x > movement->speed)
                {
                    movement->velocity.x = movement->speed;
                }

                drawable->flip_h = false;
                animated_drawable->animation_id = "knight_walk";
            }
            else if (IsKeyDown(KEY_A))
            {
                movement->velocity.x -= player->accel * game.dt;
                if (movement->velocity.x < -(movement->speed))
                {
                    movement->velocity.x = -(movement->speed);
                }

                drawable->flip_h = true;
                animated_drawable->animation_id = "knight_walk";
            }
            else
            {
                if (movement->velocity.x > 0)
                {
                    movement->velocity.x -= player->friction * game.dt;
                    if (movement->velocity.x < 0) movement->velocity.x = 0;
                }

                else if (movement->velocity.x < 0)
                {
                    movement->velocity.x += player->friction * game.dt;
                    if (movement->velocity.x > 0) movement->velocity.x = 0;
                }

                animated_drawable->animation_id = "knight_idle";
            }

            if (IsKeyPressed(KEY_SPACE))
            {
                movement->velocity.y = -(player->jump_force);
            }
        }
    }
}