#include "world.h"

namespace ECS
{
    int World::create_entity()
    {
        return next_entity_id++;
    }

    void World::destroy_entity(int entity)
    {
        transforms.remove_component(entity);
        movements.remove_component(entity);
        players.remove_component(entity);
    }
}