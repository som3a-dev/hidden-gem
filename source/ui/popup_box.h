#ifndef _POPUP_IMAGE_H
#define _POPUP_IMAGE_H

#include <raylib.h>

#include <string>

#include "button.h"

struct Game;

namespace UI
{
    enum class PopupBoxState
    {
        Shown,
        Hidden,
        Popup,
        Popdown
    };

    struct PopupBox
    {
        bool visible = false;

        int center_x = 0;
        int center_y = 0;

        // if set to 0, the size of the texture is used
        int w = 0;
        int h = 0;

        float scale = 1;

        Texture* bg_texture = NULL;

        // Used if there is no bg_texture set
        Color bg_color = BLACK;
        Color text_color = WHITE;

        std::string header = "Header";
        std::string body = "Body";

        bool animate_typing = true;
        bool confirm_box = false;

        float percent_visible = 0; // percentage of the body's text that is visible, from 0 to 1
        int chars_per_second = 12; // how many characters are shown per second
        bool sped_up = false; // if true, the body's text gets typed twice as fast as chars_per_second

        Button yes_button;
        Button no_button;

        PopupBox();
        void update(Game* game);
        void draw(Game* game, const Font& font) const;

        // Flip the state to either show or hide the popup
        void flip();

        Rectangle get_rect() const
        {
            return rect;
        }

        PopupBoxState get_state() const
        {
            return state;
        }

        // Callbacks
        static void on_yes_press(void* p, Button* button);
        static void on_no_press(void* p, Button* button);

    private:
        float anim_scale = 0;

        PopupBoxState state = PopupBoxState::Hidden;

        // the rect of the image this frame, origin top left
        Rectangle rect = {0};
    };
}

#endif