#include "asset.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>

typedef struct
{
    bool used;
    char path[ASSET_MAX_PATH];
    Texture2D texture;
} texture_entry_t;

static texture_entry_t* textures = NULL;
static int textures_capacity = 0;

static uint32_t hash(const char* str);

enum
{
    ADD_ENTRY_ADDED,  // Adding an entry succeded
    ADD_ENTRY_EXISTS, // Entry for that asset already exists
    ADD_ENTRY_FULL    // Adding entry failed, table is full
};

static int add_texture(const char* path)
{
    if (textures == NULL)
    {
        textures_capacity = 10;
        textures = calloc(textures_capacity, sizeof(texture_entry_t));
    }

    uint32_t index = hash(path) % textures_capacity;

    texture_entry_t* e = textures + index;
    assert(e);
    if (e->used == false)
    {
        strcpy(e->path, path);
        e->texture = LoadTexture(path);
        e->used = true;
        return ADD_ENTRY_ADDED;
    }
    else if (strcmp(e->path, path) == 0)
    {
        return ADD_ENTRY_EXISTS;
    }
    else
    {
        for (int i = index+1; i < textures_capacity; i++)
        {
            e++;

            if (e->used == false)
            {
                strcpy(e->path, path);
                e->texture = LoadTexture(path);
                e->used = true;
                return ADD_ENTRY_ADDED;
            }
        }
    }

    return ADD_ENTRY_FULL;
}

void asset_load_texture(const char* path)
{
    if (path == NULL) return;

    int c = add_texture(path);

    if (c == ADD_ENTRY_FULL) // Expand and rehash
    {
        int old_capacity = textures_capacity;
        texture_entry_t* old_table = textures;

        textures_capacity *= 2;
        textures = calloc(textures_capacity, sizeof(texture_entry_t));

        for (int i = 0; i < old_capacity; i++)
        {
            texture_entry_t* e = old_table + i;

            assert(e);
            if (e->used)
            {
                uint32_t index = hash(e->path);
                textures[index] = *e;
            }
        }

        add_texture(path);
    }
}

Texture* asset_get_texture(const char* path)
{
    if (path == NULL) return NULL;

    uint32_t index = hash(path) % textures_capacity;

    texture_entry_t* e = textures + index;
    assert(e);
    if ((e->used) && (strcmp(path, e->path) == 0))
    {
        return &(e->texture);
    }
    else
    {
        for (int i = index+1; i < textures_capacity; i++)
        {
            e = textures + i;
            assert(e);
            if ((e->used) && (strcmp(path, e->path) == 0))
            {
                return &(e->texture);
            }
        }
    }

    return NULL;
}

void asset_delete_assets()
{
    for (int i = 0; i < textures_capacity; i++)
    {
        texture_entry_t* e = textures + i;
        assert(e);
        if (e->used)
        {
            UnloadTexture(e->texture);
        }
    }
    free(textures);
    textures = NULL;
    textures_capacity = 0;
}

static uint32_t hash(const char* str)
{
    uint32_t hash = 2166136261u;
    for (int i = 0; i < strlen(str); i++)
    { 
        hash = (hash ^ str[i]) * 16777619u;
    }

    return hash;
}