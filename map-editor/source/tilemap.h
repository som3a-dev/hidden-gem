#ifndef _TILEMAP_H
#define _TILEMAP_H

#include "map_format.h"

#define TILE_EMPTY MF_TILE_EMPTY

#define TILE_INVALID MF_TILE_INVALID

/*
*
* a 2D array of tile ids
*
*/
typedef struct
{
    int width;
    int height;
    int* _tiles;
} tilemap_t;

// frees the previous tilemap data and creates a new cleared tilemap
void tilemap_create(tilemap_t* map, int width, int height);

// deletes the tilemap data and NULLs it
void tilemap_delete(tilemap_t* map);

void tilemap_resize(tilemap_t* map, int width, int height);

// returns the tile at index (x, y)
int tilemap_get(const tilemap_t* map, int x, int y);

// sets the tile at index (x, y) to the specified value
// does not set if the value is an invalid tile id (like TILE_INVALID)
void tilemap_set(tilemap_t* map, int x, int y, int val);

// returns the array storing the tilemap data
const int* tilemap_get_data(const tilemap_t* map);

#endif