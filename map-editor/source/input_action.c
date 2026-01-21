#include "input_action.h"

#include <assert.h>
#include <raylib.h>

void input_action_add_key(input_action_t* a, int key)
{
    assert(a);

    if (a->key_count >= INPUT_MAX_ACTION_KEYS)
    {
        return;
    }

    a->keys[a->key_count] = key;
    a->key_count++;
}

bool input_action_is_pressed(const input_action_t* a)
{
    if (a == NULL) return false;

    for (int i = 0; i < a->key_count; i++)
    {
        if (IsKeyPressed(a->keys[i]))
        {
            return true;
        }
    }

    return false;
}

bool input_action_is_held(const input_action_t* a)
{
    if (a == NULL) return false;

    for (int i = 0; i < a->key_count; i++)
    {
        if (IsKeyDown(a->keys[i]))
        {
            return true;
        }
    }

    return false;
}