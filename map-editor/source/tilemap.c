#include "tilemap.h"

#include <memory.h>
#include <stdbool.h>
#include <stdlib.h>

void tilemap_create(tilemap_t *map, int width, int height)
{
    if (map == NULL) {
        return;
    }

    size_t tiles_size = sizeof(*(map->_tiles)) * (width * height);
    map->_tiles = malloc(tiles_size);
    map->width = width;
    map->height = height;

    memset(map->_tiles, TILE_EMPTY, tiles_size);
}

void tilemap_delete(tilemap_t *map)
{
    if (map == NULL) {
        return;
    }

    free(map->_tiles);
    map->_tiles = NULL;
    map->width = 0;
    map->height = 0;
}

void tilemap_resize(tilemap_t* map, int width, int height)
{
    if (map == NULL)
    {
        return;
    }
    if ((width <= 0) || (height <= 0))
    {
        return;
    }
    if ((width == map->width) && (height == map->height))
    {
        return;
    }

    tilemap_t new_map;
    tilemap_create(&new_map, width, height);
    tilemap_t old_map = *map;

    for (int y = 0; y < old_map.height; y++)
    {
        for (int x = 0; x < old_map.width; x++)
        {
            tilemap_set(&new_map, x, y, tilemap_get(&old_map, x, y));
        }
    }

    *map = new_map;
    tilemap_delete(&old_map);
}

int tilemap_get(const tilemap_t *map, int x, int y)
{
    if (map == NULL) {
        return TILE_INVALID;
    }
    if (map->_tiles == NULL) {
        return TILE_INVALID;
    }

    if ((x < 0) || (x >= map->width)) {
        return TILE_INVALID;
    }
    if ((y < 0) || (y >= map->height)) {
        return TILE_INVALID;
    }

    return map->_tiles[x + (y * map->width)];
}

void tilemap_set(tilemap_t* map, int x, int y, int val)
{
    if (map == NULL) {
        return;
    }
    if (map->_tiles == NULL) {
        return;
    }

    if ((x < 0) || (x >= map->width)) {
        return;
    }
    if ((y < 0) || (y >= map->height)) {
        return;
    }

    if (val == TILE_INVALID)
    {
        return;
    }

    map->_tiles[x + (y * map->width)] = val;
}

const int* tilemap_get_data(const tilemap_t* map)
{
    if (map == NULL)
    {
        return NULL;
    }

    return map->_tiles;
}