/*
* This file defines 'Tile', a tile in a tileset. NOT a tile in the game world
* this stores data about the tile, everything from its id to properties
* for any Tile stored in the asset manager, its id/path is just its numeric id
* as a string
*
* When we parse a tileset json with the map format library, we convert the map format's tiles into our Tile
*/

#ifndef _TILE_H
#define _TILE_H

#include <string>
#include "map_format.h"

struct Tile
{
    int id;
    std::string texture_id; // the texture id of the tile in the tileset json with ASSETS_PATH prefixed

    // The position of the tile's sprite in the spritesheet, if texture_id points to a sprite sheet
    int sheet_x;
    int sheet_y;
    // The size of the sheet in tiles
    int sheet_w;
    int sheet_h;
};

#endif