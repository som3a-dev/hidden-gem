#ifndef _COLLISION_H
#define _COLLISION_H

#include "raylib.h"

#include "tilemap.h"
#include "game.h"

#include <vector>

namespace Collision
{
    // returns the amount of colliding tiles
    int get_colliding_tiles(const Game& game,
                            const Tilemap& tilemap,
                            const Rectangle& entity_rect,
                            Rectangle* colliding_tiles /*buffer for the result*/,
                            size_t colliding_tiles_size /*size of the buffer*/);
}

#endif