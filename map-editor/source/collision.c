#include "collision.h"

bool is_pos_in_rect(float x, float y, Rectangle r)
{
    return ((x >= r.x) && (x <= (r.x + r.width)) &&
            (y >= r.y) && (y <= (r.y + r.height)));
}
