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
    void transform_update_system(Game& game);

    void collision_update_system(Game& game);

    void interactable_update_system(Game& game);

    void animated_drawable_system(Game& game);

    void render_drawable_system(Game& game);

    void draw_ui_system(Game& game);

    void player_system(Game& game);
};

#endif