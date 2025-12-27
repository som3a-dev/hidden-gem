/*
* This file defines 'Tile', a tile in a tileset. NOT a tile in the game world
* this stores data about the tile, everything from its id to properties
* for any Tile stored in the asset manager, its id/path is just its numeric id
* as a string
*/

#ifndef _TILE_H
#define _TILE_H

#include <string>

struct Tile
{
    int id;
    std::string texture_id;
};

#endif