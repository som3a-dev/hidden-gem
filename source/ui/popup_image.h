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

        // TODO(): Should UI elements store the texture, or the texture id
        Texture* tex = NULL;
        float scale = 1;

        void update(float dt);
        void draw(Game* game, const Font& font, const GameDrawBuffer& draw_buf) const;

        // Flip the state to either show or hide the popup
        void flip();

    private:
        float anim_scale = 0;
    };
}

#endif