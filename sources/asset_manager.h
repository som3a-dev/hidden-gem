#ifndef _ASSET_MANAGER_H
#define _ASSET_MANAGER_H

#include "raylib.h"

#include <string>
#include <vector>

struct SpriteSheet;

// the asset manager stores assets in a hash table
// it uses linear probing for collision handling
struct AssetManager
{
private:

    template<typename T>
    struct TableEntry
    {
        bool used = false;
        std::string path;
        T data;
    };

    template<typename T>
    struct Table
    {
        TableEntry<T>* entries = nullptr;
        int capacity = 30;

        void add_asset(const TableEntry<T>& asset);

        TableEntry<T>* get_asset(const std::string& path) const;

        ~Table();

    private:
        bool add_entry(const TableEntry<T>& new_entry);
    };

    template<typename T>
    const Table<T>& get_table() const; 

    template<>
    const Table<Texture2D>& get_table<Texture2D>() const { return textures; }

    Table<Texture2D> textures;

    static uint32_t hash_string(const std::string& str);
public:

    AssetManager();
    ~AssetManager();

    template<typename T>
    T* get_asset(const std::string& path) const;

    void load_texture(const std::string& path);

    void print_texture_table() const;
};

#include "asset_manager.tpp"

#endif