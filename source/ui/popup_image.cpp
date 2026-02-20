#include "popup_image.h"
#include "../game.h"

#include <assert.h>
#include <string.h>
#include <string>

const float popup_speed = 4.0f;

namespace UI
{
    void PopupImage::update(Game* game)
    {
        if (!visible)
        {
            return;
        }

        switch (state)
        {
            case PopupImageState::Popup:
            {
                anim_scale += popup_speed * game->dt;
                if (anim_scale > 1)
                {
                    anim_scale = 1;
                    state = PopupImageState::Shown;
                }
            } break;

            case PopupImageState::Popdown:
            {
                anim_scale -= popup_speed * game->dt;
                if (anim_scale < 0)
                {
                    anim_scale = 0;
                    state = PopupImageState::Hidden;
                }
            } break;

            default:
            {

            } break;
        }

        // Update rect
        float rl_scale = scale * anim_scale;

        if ((w == 0) || (h == 0))
        {
            rect.width =  tex->width * rl_scale;
            rect.height = tex->height * rl_scale;
        }
        else
        {
            rect.width =  w * rl_scale;
            rect.height = h * rl_scale;
        }

        rect.x = (float)(center_x) - rect.width / 2;
        rect.y = (float)(center_y) - rect.height / 2;
    }

    void PopupImage::draw(Game* game, const Font& font) const
    {
        assert(game);
        assert(tex);

        if (!visible)
        {
            return;
        }

        if (state == PopupImageState::Hidden)
        {
            return;
        }

        DrawTexturePro(*tex,
        {0, 0, (float)(tex->width), (float)(tex->height)},
        rect, {0,0}, 0, WHITE);
    }

    void PopupImage::flip()
    {
        switch (state)
        {
            case PopupImageState::Shown:
            {
                state = PopupImageState::Popdown;
            } break;

            case PopupImageState::Hidden:
            {
                state = PopupImageState::Popup;
            } break;

            default:
            {

            } break;
        }
    }
}