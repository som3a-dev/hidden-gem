#include "camera.h"

#include <math.h>

void FollowCamera::update(float dt)
{
    if (!enabled) return;

    const float smoothing = 5;
    const float left_border = current_target.x + max_left; 
    const float right_border = current_target.x + max_right;

    if (target.x > right_border)
    {
        current_target.x += (target.x - right_border);
    }
    if (target.x < left_border)
    {
        current_target.x -= (left_border - target.x);
    }
    current_target.x = floorf(current_target.x);

    x = ((x) * (smoothing - 1) + current_target.x) / smoothing;
}
