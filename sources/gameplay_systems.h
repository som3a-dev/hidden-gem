/*
* systems as in the systems of an entity component system,
* where the core of the game entity logic resides
*/

#ifndef _GAMEPLAY_SYSTEMS
#define _GAMEPLAY_SYSTEMS

#include "asset_manager.h"
#include "ecs/world.h"

namespace GameplaySystems
{
    // Updates any entities with a drawable
    // and a transform (sets the position of the drawable to match the transform)
    void transform_update_system(ECS::World& world);

    void animated_drawable_system(ECS::World& world);

    void render_drawable_system(ECS::World& world, const AssetManager& asset_m);
    void player_system(ECS::World& world, float dt);
};

#endif