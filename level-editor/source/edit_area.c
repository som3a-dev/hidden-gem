#include "edit_area.h"

Rectangle edit_area_get_rect(const edit_area_t* a, int window_w, int window_h)
{
    if (a == NULL)
    {
        return (Rectangle){0};
    }

    return (Rectangle){
        (float)(0 + a->left),
        (float)(0 + a->top),
        (float)(window_w - a->right - a->left),
        (float)(window_h - a->bottom - a->top)
    };
}
