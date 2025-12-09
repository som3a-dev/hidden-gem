#include "asset_manager.h"

#include <assert.h>
#include <cstdio>
#include <memory>
#include <iostream>

AssetManager::AssetManager()
{
    assert(textures_capacity != 0);
//    textures = (TextureEntry*)calloc(textures_capacity, sizeof(TextureEntry));
    textures = new TextureEntry[textures_capacity];
}

AssetManager::~AssetManager()
{
    TextureEntry* entry = nullptr;
    for (int i = 0; i < textures_capacity; i++)
    {
        entry = (TextureEntry*)(textures + i);

        if (entry->used)
        {
            UnloadTexture(entry->texture);
        }
    }

//    free(textures);
    delete[] textures;
}

void AssetManager::load_texture(const std::string& path)
{
    Texture2D texture = LoadTexture(path.c_str());
    if (IsTextureValid(texture) == false)
    {
        std::cout << "AssetManager: Loading texture '" << path << "' Failed.\n";
        return;
    }

    TextureEntry entry;
    entry.used = true;
    entry.path = path;
    entry.texture = texture;

    // TODO(omar): differentiate between the function failing
    // due to the table being full
    // or failing due to the entry having a duplicate (same path)
    // if its the latter we shouldn't even expand
    if (add_entry_to_table(entry) == false)
    {
        // TODO: handle not finding any empty spot in the array

        // Expand table and rehash entries
        int old_capacity = textures_capacity;

        textures_capacity *= 2;
        TextureEntry* new_table = new TextureEntry[textures_capacity];

        for (int i = 0; i < old_capacity; i++)
        {
            TextureEntry* old_entry = textures + i;
            if (old_entry->used == false)
            {
                continue;
            }

            uint32_t new_index = hash_string(old_entry->path) % textures_capacity;
            new_table[new_index] = *old_entry;
        }

        delete[] textures;
        textures = new_table;

        // Add the texture to the table now that we've expanded
        add_entry_to_table(entry);
    }
}

Texture2D* AssetManager::get_texture(const std::string& path) const
{
    uint32_t index = hash_string(path) % textures_capacity;
    TextureEntry* entry = (TextureEntry*)(textures + index);

    if (entry->path == path)
    {
        return &(entry->texture);
    }
    else
    {
        for (int i = index+1; i < textures_capacity; i++)
        {
            entry++;

            if ((entry->used) && (path == entry->path))
            {
                return &(entry->texture);
            }
        }
    }
    
    return nullptr;
}

uint32_t AssetManager::hash_string(const std::string& str)
{
    uint32_t hash = 2166136261u;
    for (char c : str)
    { 
        hash = (hash ^ c) * 16777619u;
    }

    return hash;
}

bool AssetManager::add_entry_to_table(const TextureEntry& new_entry)
{
    uint32_t index = hash_string(new_entry.path) % textures_capacity;

    TextureEntry* entry = textures + index;

    if (entry->used == false)
    {
        *entry = new_entry;
        return true;
    }
    else if (entry->path == new_entry.path)
    {
        // Don't load the same asset twice
        return false;
    }
    else
    {
        // Collision, do linear probing
        for (int i = index+1; i < textures_capacity; i++)
        {
            entry++;

            if (entry->used == false)
            {
                *entry = new_entry;
                return true;
            }
        }
    }

    return false;
}

void AssetManager::print_texture_table() const
{
    TextureEntry* entry = nullptr;
    for (int i = 0; i < textures_capacity; i++)
    {
        entry = textures + i;
        if (entry->used)
        {
            printf("Entry %d : %s\n", i, entry->path.c_str());
        }
        else
        {
            printf("Entry %d : NONE\n", i);
        }
    }
}
