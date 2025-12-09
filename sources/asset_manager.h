#ifndef _ASSET_MANAGER_H
#define _ASSET_MANAGER_H

#include "raylib.h"

#include <string>
#include <vector>

struct AssetManager
{
    AssetManager();
    ~AssetManager();

    void load_texture(const std::string& path);
    Texture2D* get_texture(const std::string& path) const;

    void print_texture_table() const;
private:

    struct TextureEntry
    {
        bool used = false;
        std::string path;
        Texture2D texture;
    };

    // the asset manager stores assets in a hash table
    // it uses linear probing for collision handling

    TextureEntry* textures = nullptr;
    int textures_capacity = 30;

    bool add_entry_to_table(const TextureEntry& new_entry);

    static uint32_t hash_string(const std::string& str);
};

#endif