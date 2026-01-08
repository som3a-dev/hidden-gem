/*
* An input action allows mapping multiple keys to a single action
* It uses raylib's keycodes
*/

#ifndef _INPUT_ACTION
#define _INPUT_ACTION

#include <stdbool.h>

#define INPUT_MAX_ACTION_KEYS 10

typedef struct
{
    int keys[INPUT_MAX_ACTION_KEYS];
    int key_count;
} input_action_t;

void input_action_add_key(input_action_t* a, int key);
bool input_action_is_pressed(const input_action_t* a);
bool input_action_is_held(const input_action_t* a);

#endif