#ifndef _TILEMAP_H
#define _TILEMAP_H

#define TILE_EMPTY 0
#define TILE_INVALID -6767 // invalid position

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

// returns the tile at index (x, y)
int tilemap_get(const tilemap_t* map, int x, int y);

// sets the tile at index (x, y) to the specified value
// does not set if the value is an invalid tile id (like TILE_INVALID)
void tilemap_set(tilemap_t* map, int x, int y, int val);

#endif