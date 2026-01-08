#ifndef _EDIT_AREA_H
#define _EDIT_AREA_H

#include <raylib.h>

// the edit/play area is defined as offsets from the window in the 4 directions
// left right top bottom
typedef struct
{
    int left;
    int right;
    int top;
    int bottom;
    int border_thickness;
} edit_area_t;

// returns an {x, y, w, h} rect from the edit area, given the window/viewport width and height
Rectangle edit_area_get_rect(const edit_area_t* a, int window_w, int window_h);

#endif