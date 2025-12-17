#include "frame_animation.h"

#include "asset_manager.h"

void FrameAnimation::set_sheet(const std::string& sheet_texture_id,
                const AssetManager& asset_m,
                int sheet_width,
                int sheet_height)
{
    Texture2D* texture = asset_m.get_asset<Texture2D>(sheet_texture_id);
    if (texture == nullptr) return;

    frame_width  = texture->width / sheet_width;
    frame_height = texture->height / sheet_height;  

    sheet_texture = sheet_texture_id;
}

void FrameAnimation::update()
{
    uint32_t time_ms = static_cast<uint32_t>(GetTime() * 1000);

    if ((time_ms - last_advance_time_ms) > interval_ms)
    {
        frame_index++;
        last_advance_time_ms = time_ms;
    }

    if (frame_index >= frames.size())
    {
        frame_index = 0;
    }
}

Rectangle FrameAnimation::get_current_frame_src() const
{
    if (frames.empty()) return {};

    Vector2 frame_pos = frames[frame_index];
    return {
        (float)(frame_width * frame_pos.x),
        (float)(frame_height * frame_pos.y),
        (float)frame_width,
        (float)frame_height
    };
}
