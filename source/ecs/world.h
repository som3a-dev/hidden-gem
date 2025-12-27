#ifndef _ECS_WORLD
#define _ECS_WORLD

#include <array>
#include <string>
#include <vector>

#include "raylib.h"
#include "sparse_set.h"
#include "../frame_animation.h"

namespace ECS
{
    struct TransformComponent
    {
        TransformComponent() {};
        TransformComponent(float _x, float _y)
        {
            x = _x;
            y = _y;
        }

        float x = 0;
        float y = 0;
    };

    struct MovementComponent
    {
        float speed = 0;
        float gravity = 0;

        Vector2 velocity = {0};
    };

    struct CollisionComponent
    {
        Rectangle rect = {0};
        bool on_ground = false;

        // Used by the system

        // we handle only the first collision for now
        static constexpr int _max_colliding_tiles = 1;
        std::array<Rectangle, _max_colliding_tiles> _colliding_tiles;
    };

    struct DrawableComponent 
    {
        std::string texture_path;
        int x = 0;
        int y = 0;
        int w = 0;
        int h = 0;

        Rectangle source = {};

        bool flip_h = false;
        bool flip_v = false;

        float scale = 1;
    };

    struct AnimatedDrawableComponent
    {
        std::string animation_id; // id of a FrameAnimation in the asset manager

        FrameAnimation animation = {}; // a copy of the above FrameAnimation to modify
                                       // this is what is used to draw
    };

    struct PlayerComponent
    {
        float accel = 0;
        float friction = 0;
        float jump_force = 0;
    };

    struct World
    {
        std::vector<int> entities;

        SparseSet<TransformComponent> transforms;

        SparseSet<MovementComponent> movements;

        SparseSet<CollisionComponent> collisions;

        SparseSet<DrawableComponent> drawables;

        SparseSet<AnimatedDrawableComponent> animated_drawables;

        SparseSet<PlayerComponent> players;

        int next_entity_id = 0;

        int create_entity();
        void destroy_entity(int entity);
    };
}

#endif