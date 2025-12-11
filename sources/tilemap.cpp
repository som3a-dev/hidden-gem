#include "tilemap.h"

#include <cstdlib>
#include <memory>

Tilemap::~Tilemap()
{
    delete[] tiles;
}

void Tilemap::create(int w, int h)
{
    if ((w <= 0) || (h <= 0)) return;

    delete[] tiles;

    tiles = new int[w * h];
    memset(tiles, EMPTY_TILE, sizeof(int) * (w * h));

    width = w;
    height = h;
}

void Tilemap::clear()
{
    delete[] tiles;
    tiles = nullptr;

    width = 0;
    height = 0;
}

void Tilemap::set_tile(int x, int y, int id)
{
    if ((x < 0) || (x >= width)) return;
    if ((y < 0) || (y >= height)) return;

    tiles[x + (width * y)] = id;
}

int Tilemap::get_tile(int x, int y) const
{
    if ((x < 0) || (x >= width)) return INVALID_TILE;
    if ((y < 0) || (y >= height)) return INVALID_TILE;

    return tiles[x + (width * y)];
}