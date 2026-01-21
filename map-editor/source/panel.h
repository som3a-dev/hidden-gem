#ifndef _PANEL_H 
#define _PANEL_H

#include <raylib.h>

// A panel's layout is defined as insets and a border thickness (since it can affect layout in our case)
// Insets are defined as offsets from the window/viewport in the 4 directions
// left right top bottom
typedef struct
{
    int left;
    int right;
    int top;
    int bottom;
} panel_layout_t;

// returns an {x, y, w, h} rect from the insets, given the window/viewport width and height
Rectangle panel_layout_get_rect(const panel_layout_t* a, int window_w, int window_h,
                                int border_thickness);

#endif