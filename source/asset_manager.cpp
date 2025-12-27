#include "asset_manager.h"

#include <assert.h>
#include <memory>
#include <iostream>

AssetManager::AssetManager()
{
    assert(textures.capacity != 0);
    textures.entries = new TableEntry<Texture2D>[textures.capacity];
    frame_animations.entries = new TableEntry<FrameAnimation>[frame_animations.capacity];
    tiles.entries = new TableEntry<Tile>[tiles.capacity];
}

AssetManager::~AssetManager()
{
    TableEntry<Texture2D>* entry = nullptr;
    for (int i = 0; i < textures.capacity; i++)
    {
        entry = (TableEntry<Texture2D>*)(textures.entries + i);

        if (entry->used)
        {
            UnloadTexture(entry->data);
        }
    }
}

void AssetManager::load_texture(const std::string& path)
{
    Texture2D texture = LoadTexture(path.c_str());
    if (IsTextureValid(texture) == false)
    {
        std::cout << "AssetManager: Loading texture '" << path << "' Failed.\n";
        return;
    }

    TableEntry<Texture2D> entry;
    entry.used = true;
    entry.path = path;
    entry.data = texture;

    if (textures.add_asset(entry) == false)
    {
        UnloadTexture(texture);
    }
}

void AssetManager::load_frame_animation(const std::string& id, const FrameAnimation& animation)
{
    TableEntry<FrameAnimation> entry;
    entry.used = true;
    entry.path = id;
    entry.data = animation;
    entry.data.id = id;

    frame_animations.add_asset(entry);
}

void AssetManager::load_tile(const Tile& tile)
{
    TableEntry<Tile> entry;
    entry.used = true;
    entry.data = tile;
    entry.path = std::to_string(tile.id);

    tiles.add_asset(entry);
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

void AssetManager::print_texture_table() const
{
    TableEntry<Texture2D>* entry = nullptr;
    for (int i = 0; i < textures.capacity; i++)
    {
        entry = textures.entries + i;
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
