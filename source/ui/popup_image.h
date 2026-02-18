#ifndef _POPUP_IMAGE_H
#define _POPUP_IMAGE_H

#include <raylib.h>
#include "../game_draw_buffer.h"

struct Game;

namespace UI
{
    enum class PopupImageState
    {
        Shown,
        Hidden,
        Popup,
        Popdown
    };

    struct PopupImage
    {
        bool visible = false;
        PopupImageState state = PopupImageState::Hidden;

        int center_x = 0;
        int center_y = 0;

        // if 0, the size of the texture is used
        int w = 0;
        int h = 0;

        // the rect of the image this frame, origin top left
        Rectangle rect = {0};

        // TODO(): Should UI elements store the texture, or the texture id
        Texture* tex = NULL;
        float scale = 1;

        void update(Game* game);
        void draw(Game* game, const Font& font) const;

        // Flip the state to either show or hide the popup
        void flip();

    private:
        float anim_scale = 0;
    };
}

#endif