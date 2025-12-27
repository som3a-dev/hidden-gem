#ifndef _ECS_SPARSE_SET
#define _ECS_SPARSE_SET

#include <vector>

namespace ECS
{
    template<typename T>
    struct SparseSet
    {
        SparseSet();

        std::vector<T> components;
        std::vector<int> entities;
        std::vector<int> entity_lookup;

        void add_component(int entity, const T& component);

        T* get_component(int entity) const;

        void remove_component(int entity);
    };
}

#include "sparse_set.tpp"

#endif