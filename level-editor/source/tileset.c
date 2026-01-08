#include "tileset.h"

#include <assert.h>
#include <string.h>

void tileset_add_tile(tileset_t* set, int id, const char* texture_path)
{
    assert(set);

    if (set->tiles_count >= TILES_CAPACITY)
    {
        return;
    }

    tile_t* tile = set->tiles + set->tiles_count;
    tile->id = id;
    strcpy(tile->texture_path, texture_path);

    set->tiles_count++;
}

const tile_t* tileset_get_tile(const tileset_t* set, int id)
{
    assert(set);

    if ((id-1) < 0) return NULL;
    if ((id-1) >= TILES_CAPACITY) return NULL;

    return set->tiles + (id-1);
}