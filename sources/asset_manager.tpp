#include "asset_manager.h"

template<typename T>
AssetManager::Table<T>::~Table()
{
    if (entries)
    {
        delete[] entries;
    }
}

template<typename T>
T* AssetManager::get_asset(const std::string& path) const
{
    TableEntry<T>* entry = get_table<T>().get_asset(path);
    if (entry)
    {
        return &(entry->data);
    }

    return nullptr;
}

template<typename T>
AssetManager::TableEntry<T>* AssetManager::Table<T>::get_asset(const std::string& path) const
{
    uint32_t index = hash_string(path) % capacity;

    TableEntry<T>* entry = (TableEntry<T>*)(entries + index);

    if (entry->path == path)
    {
        return entry;
    }
    else
    {
        for (int i = index+1; i < capacity; i++)
        {
            entry++;

            if ((entry->used) && (path == entry->path))
            {
                return entry;
            }
        }
    }
    
    return nullptr;
}

template<typename T>
void AssetManager::Table<T>::add_asset(const TableEntry<T>& asset)
{
    // TODO(omar): differentiate between the function failing
    // due to the table being full
    // or failing due to the entry having a duplicate (same path)
    if (add_entry(asset) == false)
    {
        // Expand table and rehash entries
        int old_capacity = capacity;

        capacity *= 2;
        TableEntry<T>* new_table = new TableEntry<T>[capacity];

        // TODO(omar): USE add_entry DURING REHASH
        for (int i = 0; i < old_capacity; i++)
        {
            TableEntry<T>* old_entry = entries + i;
            if (old_entry->used == false)
            {
                continue;
            }

            uint32_t new_index = hash_string(old_entry->path) % capacity;
            new_table[new_index] = *old_entry;
        }

        delete[] entries;
        entries = new_table;

        // Add the texture to the table now that we've expanded
        add_entry(asset);
    }
}

template<typename T>
bool AssetManager::Table<T>::add_entry(const TableEntry<T>& new_entry)
{
    uint32_t index = hash_string(new_entry.path) % capacity;

    TableEntry<T>* entry = entries + index;

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
        for (int i = index+1; i < capacity; i++)
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