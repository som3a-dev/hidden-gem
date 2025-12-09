#include "sparse_set.h"

#define SPARSE_NO_COMPONENT -1

namespace ECS
{ 
    template<typename T>
    SparseSet<T>::SparseSet()
    {
        entity_lookup.resize(1);
        memset(entity_lookup.data(), SPARSE_NO_COMPONENT,
        sizeof(int) * entity_lookup.size());
    }

    template<typename T>
    void SparseSet<T>::add_component(int entity, const T& component)
    {
        if (entity >= entity_lookup.size())
        {
            // Expand the sparse set
            size_t old_size = entity_lookup.size();
            entity_lookup.resize(old_size + (entity - old_size + 1),
            SPARSE_NO_COMPONENT);
        }

        components.push_back(component);

        entities.push_back(entity);

        entity_lookup[entity] = (int)(entities.size() - 1);
    }

    template<typename T>
    T* SparseSet<T>::get_component(int entity) const
    {
        if (entity >= entity_lookup.size())
        {
            return nullptr;
        }

        int component_index = entity_lookup[entity];
        if (component_index != SPARSE_NO_COMPONENT)
        {
            return const_cast<T*>
            (components.data() + component_index);
        }

        return nullptr;
    }

    template<typename T>
    void SparseSet<T>::remove_component(int entity)
    {
        if (entity >= entity_lookup.size())
        {
            return;
        }

        int component_index = entity_lookup[entity];
        if (component_index == SPARSE_NO_COMPONENT)
        {
            return;
        }

        // Update dense component array
        T* component = components.data() + components.size()-1;
        components[component_index] = *component;

        // Update dense entities array
        int temp = entities[entities.size()-1];
        entities[component_index] = temp;
        entity_lookup[temp] = component_index;

        entity_lookup[entity] = SPARSE_NO_COMPONENT;

        entities.pop_back();
        components.pop_back();
    }
}