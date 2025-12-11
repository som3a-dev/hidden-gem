/*
* systems as in the systems of an entity component system,
* where the core of the game entity logic resides
*/

#ifndef _GAMEPLAY_SYSTEMS
#define _GAMEPLAY_SYSTEMS

#include "asset_manager.h"
#include "ecs/world.h"
#include "tilemap.h"
#include "game.h"

namespace GameplaySystems
{
    void transform_update_system(ECS::World& world);

    void movement_update_system(ECS::World& world, const Game& game, const Tilemap& tilemap);

    void animated_drawable_system(ECS::World& world, const AssetManager& asset_m);


    void render_drawable_system(ECS::World& world, const AssetManager& asset_m);
    void player_system(ECS::World& world);
};

#endif