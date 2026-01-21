#include "editor_camera.h"

#define CAMERA_SPEED 4

void editor_update_camera(editor_state_t* s)
{
    input_action_t camera_left = {0};
    input_action_add_key(&camera_left, KEY_A);

    input_action_t camera_right = {0};
    input_action_add_key(&camera_right, KEY_D);

    input_action_t camera_down = {0};
    input_action_add_key(&camera_down, KEY_S);

    input_action_t camera_up = {0};
    input_action_add_key(&camera_up, KEY_W);

    if (input_action_is_held(&camera_right))
    {
        s->camera_x += CAMERA_SPEED;
    }
    else if (input_action_is_held(&camera_left))
    {
        s->camera_x -= CAMERA_SPEED;
    }
    if (input_action_is_held(&camera_down))
    {
        s->camera_y += CAMERA_SPEED;
    }
    else if (input_action_is_held(&camera_up))
    {
        s->camera_y -= CAMERA_SPEED;
    }
}