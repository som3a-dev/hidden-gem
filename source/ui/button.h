#ifndef _BUTTON_H
#define _BUTTON_H

#include <raylib.h>
#include <string>

struct Game;

namespace UI
{
    enum 
    {
        BUTTON_IDLE = 0,
        BUTTON_HOVERED = 1 << 1,
        BUTTON_PRESSED = 1 << 2
    };

    struct Button
    {
        bool visible = false;

        Rectangle rect = {0};
        int state = BUTTON_IDLE;

        bool outline = false;

        Color text_color = WHITE;

        void (*on_press)(void* parent, Button* button) = nullptr;
        void* parent = nullptr;

        std::string text = "Button";

        void update(Game* game);
        void draw(Game* game, const Font& font) const;

        // Sets the background texture of the button
        void set_texture(Texture* tex);

        private:
        Texture* texture = nullptr;
    };
}

#endif