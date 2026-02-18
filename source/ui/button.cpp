#include "button.h"
#include "game.h"

namespace UI
{
    void Button::update(Game* game)
    {
        if (!visible) return;

        Vector2 mouse = GetMousePosition();
        float scalex = (float)(game->draw_buf.w) / game->screen_width;
        float scaley = (float)(game->draw_buf.h) / game->screen_height;

        mouse.x *= scalex;
        mouse.y *= scaley;

        if (CheckCollisionPointRec(mouse, rect))
        {
            state |= BUTTON_HOVERED;

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                state |= BUTTON_PRESSED;
            }
            else
            {
                state &= ~BUTTON_PRESSED;
            }
        }
        else
        {
            state = BUTTON_IDLE;
        }
    }

    void Button::draw(Game* game, const Font& font) const
    {
        if (!visible) return;

        Color color = BEIGE;

        if (state & BUTTON_PRESSED)
        {
            color = RED;
        }
        else if (state & BUTTON_HOVERED)
        {
            color = BROWN;
        }

        DrawRectangleRec(rect, color);

        Vector2 text_sz = MeasureTextEx(font, text.c_str(), 16, 1);
        Vector2 text_pos;
        text_pos.x = rect.x + (rect.width / 2)  - text_sz.x / 2;
        text_pos.y = rect.y + (rect.height / 2) - text_sz.y / 2;

        game->QueueTextEx(font, text.c_str(), text_pos, 16, 1, BLACK);
    }
}