#include "question_panel.h"
#include "game.h"

#include <assert.h>

namespace UI
{
    QuestionPanel::QuestionPanel()
    {
        // TODO(): A default background
    }

    void QuestionPanel::set_scale(float scale)
    {
        background.scale = scale;
    }

    void QuestionPanel::set_rect(Rectangle rect)
    {
        background.center_x = (int)(rect.x + rect.width / 2);
        background.center_y = (int)(rect.y + rect.height / 2);
        background.w = (int)(rect.width);
        background.h = (int)(rect.height);
    }

    void QuestionPanel::set_background(const AssetManager& asset_m, const std::string& texture_id)
    {
        background.tex = asset_m.get_asset<Texture>(texture_id);
        assert(background.tex);
    }

    void QuestionPanel::flip()
    {
        if (!visible) return;

        background.flip();
    }

    void QuestionPanel::update(Game* game) 
    {
        if (!visible)
        {
            background.visible = false;
        }

        background.visible = true;
        background.update(game);

        if (background.state == PopupImageState::Shown)
        {
            option_rect.width = background.rect.width;
            option_rect.height = background.rect.height / 2.5f;
            option_rect.x = background.rect.x;
            option_rect.y = background.rect.y + (background.rect.height - option_rect.height);

            float button_w = option_rect.width / 2;
            float button_h = option_rect.height / 2;
            const int button_margin = 0;
            buttons[0].rect = {
                option_rect.x + button_margin, option_rect.y + button_margin,
                button_w, button_h
            };

            buttons[1].rect = {
                option_rect.x + button_margin, option_rect.y + option_rect.height - button_h - button_margin,
                button_w, button_h
            };

            buttons[2].rect = {
                option_rect.x + option_rect.width - button_w - button_margin,
                option_rect.y + button_margin,
                button_w, button_h
            };

            buttons[3].rect = {
                option_rect.x + option_rect.width - button_w - button_margin,
                option_rect.y + option_rect.height - button_h - button_margin,
                button_w, button_h
            };
        }

        for (Button& button : buttons)
        {
            button.visible = background.state == PopupImageState::Shown;
            button.update(game);

            button.set_texture(game->asset_m.get_asset<Texture>(ASSETS_PATH"wood.jpg"));
            button.outline = true;
        }
    }

    void QuestionPanel::draw(Game* game, const Font& font) const
    {
        if (!visible) return;

        background.draw(game, font);

        for (const Button& button : buttons)
        {
            button.draw(game, font);
        }
    }
}