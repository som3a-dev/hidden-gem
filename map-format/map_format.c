#include "include/map_format.h"
#include "log.h"

#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HEADER 64
// The header is written at the top of the file, without the null terminator
static const char* map_header = "HGMFV01";

mf_tilemap_t mf_load_tilemap(const char* filepath)
{
    if (filepath == NULL)
    {
        return (mf_tilemap_t){0};
    }

    FILE* fp = fopen(filepath, "rb");
    if (fp == NULL)
    {
        return (mf_tilemap_t){0};
    }

    size_t header_sz = strlen(map_header);
    char header[MAX_HEADER];
    fread(header, sizeof(char), header_sz, fp);
    if (memcmp(map_header, header, header_sz) != 0)
    {
        LOG_WARNING("Loading map failed, Header doesn't match");
        return (mf_tilemap_t){0};
    }

    mf_tilemap_t map = {0};
    fread(&(map.w), sizeof(mf_mapsz_t), 1, fp);
    fread(&(map.h), sizeof(mf_mapsz_t), 1, fp);

    if ((map.w == 0) || (map.h == 0) || (map.w > MF_MAX_TILEMAP_WIDTH) || (map.h > MF_MAX_TILEMAP_HEIGHT))
    {
        LOG_WARNING("Loading map failed, Invalid map dimensions");
        return (mf_tilemap_t){0};
    }

    size_t sz = sizeof(mf_tileid_t) * (map.w * map.h);
    map.tiles = malloc(sz);
    memset(map.tiles, MF_TILE_EMPTY, sz);

    mf_tileid_t tile;
    mf_mapsz_t x = 0;
    mf_mapsz_t y = 0;
    while ((y < map.h))
    {
        if (feof(fp))
        {
            LOG_WARNING("Loading map failed, Premature End Of File reached");
            free(map.tiles);
            fclose(fp);
            return (mf_tilemap_t){0};
        }
        fread(&tile, sizeof(tile), 1, fp);
        if (tile == MF_TILE_ROW)
        {
            y++;
            x = 0;
            continue;
        }

        map.tiles[x + (y * map.w)] = tile;
        x++;
    }

    fclose(fp);
    return map;
}

bool mf_save_tilemap(const char* filepath, const mf_tilemap_t* map)
{
    if (filepath == NULL)
    {
        LOG_WARNING("Opening file '%s' for saving failed", filepath);
        return false;
    }

    assert(map);

    FILE* fp = fopen(filepath, "wb");
    if (fp == NULL)
    {
        LOG_WARNING("Opening file '%s' for saving failed", filepath);
        return false;
    }

    if (map->tiles == NULL)
    {
        LOG_WARNING("No map exists to save to file '%s'", filepath);
        return false;
    }

    fwrite(map_header, sizeof(char), strlen(map_header), fp);

    fwrite(&(map->w), sizeof(mf_mapsz_t), 1, fp);
    fwrite(&(map->h), sizeof(mf_mapsz_t), 1, fp);

    const mf_tileid_t* row = map->tiles;
    const mf_tileid_t row_seperator = MF_TILE_ROW;
    for (mf_mapsz_t i = 0; i < map->h; i++)
    {
        fwrite(row, sizeof(*row), map->w, fp);
        fwrite(&row_seperator, sizeof(row_seperator), 1, fp);
        row += map->w;
    }

    fclose(fp);
    return true;
}

mf_tilemap_t mf_tilemap_create(mf_mapsz_t w, mf_mapsz_t h)
{
    mf_tilemap_t map = {0};
    if (w < 0)
    {
        return map;
    }
    if (h < 0)
    {
        return map;
    }

    map.w = w;
    map.h = h;

    size_t sz = sizeof(mf_tileid_t) * (w * h);
    map.tiles = malloc(sz);
    memset(map.tiles, MF_TILE_EMPTY, sz);

    return map;
}

void mf_tilemap_destroy(mf_tilemap_t* map)
{
    if (map)
    {
        free(map->tiles);
        map->w = 0;
        map->h = 0;
    }
}

void mf_tilemap_set(mf_tilemap_t* map, mf_mapsz_t x, mf_mapsz_t y, mf_tileid_t tile)
{
    assert(map);
    if (map->tiles == NULL)
    {
        return;
    }

    if ((x < 0) || (x >= map->w))
    {
        return;
    }
    if ((y < 0) || (y >= map->h))
    {
        return;
    }

    map->tiles[x + (y * map->w)] = tile;
}

mf_tileid_t mf_tilemap_get(const mf_tilemap_t* map, mf_mapsz_t x, mf_mapsz_t y)
{
    assert(map);
    if (map->tiles == NULL)
    {
        return MF_TILE_INVALID;
    }

    if ((x < 0) || (x >= map->w))
    {
        return MF_TILE_INVALID;
    }
    if ((y < 0) || (y >= map->h))
    {
        return MF_TILE_INVALID;
    }

    return map->tiles[x + (y * map->w)];
}