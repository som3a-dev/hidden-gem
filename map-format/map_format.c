#include "include/map_format.h"
#include "vendor/cJSON.h"
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

void mf_load_tileset(const char* filepath, mf_tile_t** out_tiles, int* out_count)
{
    if ((out_tiles == NULL) || out_count == NULL)
    {
        LOG_ERROR("Invalid parameters passed to mf_load_tileset");
        return;
    }

    if (filepath == NULL)
    {
        goto fail_file;
    }

    FILE* fp = fopen(filepath, "r");
    if (fp == NULL)
    {
        goto fail_file;
    }

    fseek(fp, 0, SEEK_END);
    long bufsz = ftell(fp) + 1;
    fseek(fp, 0, SEEK_SET);

    char* buf = malloc(sizeof(char) * bufsz);
    fread(buf, sizeof(char), bufsz, fp);
    buf[bufsz-1] = '\0';

    cJSON* json = cJSON_Parse(buf);

    if (json == NULL)
    {
        goto invalid_format;
    }

    if (cJSON_IsArray(json) == false)
    {
        goto invalid_format;
    }

    int tile_count = cJSON_GetArraySize(json);
    *out_tiles = calloc(tile_count, sizeof(mf_tile_t));
    *out_count = tile_count;

    mf_tile_t* tile = (*out_tiles);

    for (int i = 0; i < tile_count; i++)
    {
        cJSON* item = cJSON_GetArrayItem(json, i);
        if (cJSON_IsObject(item) == false)
        {
            goto invalid_format;
        }

        // Mandatory properties first (id, texture_id)
        cJSON* id_item = cJSON_GetObjectItem(item, "id");
        if (cJSON_IsNumber(id_item) == false)
        {
            goto invalid_format;
        }
        cJSON* texture_id_item = cJSON_GetObjectItem(item, "texture_id");
        if (cJSON_IsString(texture_id_item) == false)
        {
            goto invalid_format;
        }

        // TODO(): find a way to check if id is not an integer
        int id = (int)(cJSON_GetNumberValue(id_item));
        char* texture_id = cJSON_GetStringValue(texture_id_item);

        tile->id = id;
        tile->texture_id = malloc(sizeof(char) * (strlen(texture_id)+1));
        strcpy(tile->texture_id, texture_id);

        // Non mandatory properties
        cJSON* sheet_pos_item = cJSON_GetObjectItem(item, "sheet_position");
        if (sheet_pos_item)
        {
            if (cJSON_IsArray(sheet_pos_item) == false)
            {
                goto invalid_format;
            }
            if (cJSON_GetArraySize(sheet_pos_item) != 2)
            {
                goto invalid_format;
            }

            cJSON* sheet_x_item = cJSON_GetArrayItem(sheet_pos_item, 0);
            cJSON* sheet_y_item = cJSON_GetArrayItem(sheet_pos_item, 1);

            if ((cJSON_IsNumber(sheet_x_item) == false) || (cJSON_IsNumber(sheet_y_item) == false))
            {
                goto invalid_format;
            }

            tile->sheet_x = (int)(cJSON_GetNumberValue(sheet_x_item));
            tile->sheet_y = (int)(cJSON_GetNumberValue(sheet_y_item));
       
            // A sheet size must be specified alongside a sheet position
            cJSON* sheet_size_item = cJSON_GetObjectItem(item, "sheet_size");
            if (cJSON_IsArray(sheet_size_item) == false)
            {
                goto invalid_format;
            }
            if (cJSON_GetArraySize(sheet_size_item) != 2)
            {
                goto invalid_format;
            }

            cJSON* sheet_w_item = cJSON_GetArrayItem(sheet_size_item, 0);
            cJSON* sheet_h_item = cJSON_GetArrayItem(sheet_size_item, 1);

            if ((cJSON_IsNumber(sheet_w_item) == false) || (cJSON_IsNumber(sheet_h_item) == false))
            {
                goto invalid_format;
            }

            tile->sheet_w = (int)(cJSON_GetNumberValue(sheet_w_item));
            tile->sheet_h = (int)(cJSON_GetNumberValue(sheet_h_item));
        }

        tile++;
    }

    // success
    LOG_INFO("Loaded tileset file '%s' successfully", filepath);
    goto free;

    invalid_format:
    LOG_WARNING("Invalid tileset json format");
    mf_load_tileset_free(out_tiles, *(out_count));
    goto free;

    fail_file:
    LOG_ERROR("Invalid tileset json file passed");
    *out_tiles = NULL;
    *out_count = 0;
    return;

    free:
    free(buf);
    fclose(fp);
    cJSON_Delete(json);
}

void mf_load_tileset_free(mf_tile_t** tiles, int tile_count)
{
    if (tiles == NULL)
    {
        LOG_ERROR("Invalid paramaters passed to mf_load_tileset_free");
        return;
    }

    if (((*tiles) == NULL) || (tile_count <= 0))
    {
        return;
    }

    for (int i = 0; i < tile_count; i++)
    {
        mf_tile_t* tile = (*tiles) + i;
        free(tile->texture_id);
    }

    free(*tiles);
    *tiles = NULL;
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