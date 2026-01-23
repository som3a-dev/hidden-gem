#ifndef _TILESET_H
#define _TILESET_H

#include "asset.h"

#define TILES_CAPACITY 100
#define SET_TILE_EMPTY 0 // if the id of tile_t is this value, it is an empty tileset tile (no tile)

typedef struct
{
    int id;
    char texture_path[ASSET_MAX_PATH]; // the texture_id in the tileset json, prefixed by ASSETS_PATH

    int sheet_x;
    int sheet_y;
    int sheet_w;
    int sheet_h;
} tile_t;

typedef struct
{
    tile_t tiles[TILES_CAPACITY];
    int tiles_count;
} tileset_t;

void tileset_add_tile(tileset_t* set, int id, const char* texture_path, int sheet_x, int sheet_y, int sheet_w, int sheet_h);
const tile_t* tileset_get_tile(const tileset_t* set, int id);

#endif