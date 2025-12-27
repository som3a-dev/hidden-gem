#include "collision.h"

#include <array>

namespace Collision
{
    int get_colliding_tiles(const Game& game,
                            const Tilemap& tilemap,
                            const Rectangle& entity_rect,
                            Rectangle* colliding_tiles,
                            size_t colliding_tiles_size)
    {
        if (colliding_tiles == nullptr)
        {
            return 0;
        }

        int i = 0;
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
                    if (i >= colliding_tiles_size)
                    {
                        return i;
                    }

                    colliding_tiles[i] = tile_rect;
                    i++;
                }
            }
        }

        return i;
    }
}