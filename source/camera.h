#ifndef _CAMERA_H
#define _CAMERA_H

#include <raylib.h>

// The reason this is not called Camera is because raylib.h took the Camera name first
struct FollowCamera
{
    bool enabled = true;

    float x = 0;
    float y = 0;

    // The actual target we are following
    Vector2 target = {};

    // The borders of the area the target must occupy in screen space
    // If they move beyond them, we move
    float max_left = 0;
    float max_right = 0;

    void update(float dt);

    private:
    // The current target position we smoothly interpolate to
    Vector2 current_target = {};
};

#endif