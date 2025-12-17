#ifndef _ASSET_MANAGER_H
#define _ASSET_MANAGER_H

#include "raylib.h"

#include <string>
#include <vector>

#include "frame_animation.h"

// the asset manager stores assets in a hash table
// it uses linear probing for collision handling

// to add a new asset to it, you need to add a get_table<ASSET_TYPE>
// and a load_ASSET_TYPE function
// and a Table<ASSET_TYPE> member table
// and initialize the table.entries in the asset manager's constructor
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

    template<>
    const Table<FrameAnimation>& get_table<FrameAnimation>() const { return frame_animations; }

    Table<Texture2D> textures;
    Table<FrameAnimation> frame_animations;

    static uint32_t hash_string(const std::string& str);
public:

    AssetManager();
    ~AssetManager();

    template<typename T>
    T* get_asset(const std::string& path) const;

    void load_texture(const std::string& path);
    void load_frame_animation(const std::string& id, const FrameAnimation& animation);

    void print_texture_table() const;
};

#include "asset_manager.tpp"

#endif