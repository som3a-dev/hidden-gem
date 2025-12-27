#include "texture_animation.h"

TextureAnimation::TextureAnimation() 
{

}

TextureAnimation::~TextureAnimation()
{
}

void TextureAnimation::push_texture(const std::string path)
{
    textures.push_back(path);
}

void TextureAnimation::set_frame_interval(uint32_t new_frame_interval_ms)
{
    this->frame_interval_ms = new_frame_interval_ms;
}

Rectangle TextureAnimation::get_source_rect(const Texture2D& texture) const
{
    Rectangle rect;
    rect.x = 0;
    rect.y = 0;
    rect.width = static_cast<float>(texture.width);
    rect.height = static_cast<float>(texture.height);

    if (flip_h)
    {
        rect.width = -(rect.width);
    }
    if (flip_v)
    {
        rect.height = -(rect.height);
    }

    return rect;
}

void TextureAnimation::update()
{
    uint32_t time_ms = static_cast<uint32_t>(GetTime() * 1000);

    if ((time_ms - last_advance_time_ms) > frame_interval_ms)
    {
        texture_index++;
        last_advance_time_ms = time_ms;
    }

    if (texture_index >= textures.size())
    {
        texture_index = 0;
    }
}

