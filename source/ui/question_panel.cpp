#include "question_panel.h"

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

    void QuestionPanel::set_background(const AssetManager& asset_m, const std::string& texture_id)
    {
        background.tex = asset_m.get_asset<Texture>(texture_id);
    }

    void QuestionPanel::flip()
    {
        if (!visible) return;

        background.flip();
    }

    void QuestionPanel::update(float dt)
    {
        if (!visible) return;

        background.visible = true;
        background.update(dt);
    }

    void QuestionPanel::draw(Game* game, const Font& font, const GameDrawBuffer& draw_buf) const
    {
        if (!visible) return;

        background.draw(game, font, draw_buf);
    }
};