#include "panel.h"

Rectangle panel_layout_get_rect(const panel_layout_t* a, int window_w, int window_h,
                                int border_thickness)
{
    if (a == NULL)
    {
        return (Rectangle){0};
    }

    return (Rectangle){
        (float)(0 + a->left) - border_thickness,
        (float)(0 + a->top) -  border_thickness,
        (float)(window_w - a->right - a->left) + border_thickness * 2,
        (float)(window_h - a->bottom - a->top) + border_thickness * 2
    };
}
