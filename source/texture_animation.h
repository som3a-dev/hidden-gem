#ifndef _TEXTURE_ANIMATION
#define _TEXTURE_ANIMATION

#include "raylib.h"

#include <string>
#include <vector>

// NOTE(): as we will mostly use sprite sheets at this point, TextureAnimation is obsolete
// If we find any use for it it should be cleaned up to match FrameAnimation
// Or explore a generic approach

// stores an array of textures and an index
// advances the index every X milliseconds
struct TextureAnimation
{
    // public for convenience. You can also manipulate this vector directly
    // (add/remove textures, reorder, etc.) if needed
    std::vector<std::string> textures;

    bool flip_h = false; // flip horizontal
    bool flip_v = false; // flip vertical

    uint32_t frame_interval_ms;

    TextureAnimation();

    // NOTE: Unloads all the textures in the array
    ~TextureAnimation();

    // convenience/helper function, not required to use
    void push_texture(const std::string texture);

    void set_frame_interval(uint32_t new_frame_interval_ms);

    // returns a non const reference to the current texture
    // (the texture that should be drawn this frame)
    inline std::string get_current_texture() const
    {
        if (textures.empty())
        {
            return "";
        }

        return textures[texture_index];
    }

    // returns the correct source rect to use when drawing
    // accounts for if flip_h or flip_v are enabled
    Rectangle TextureAnimation::get_source_rect(const Texture2D& texture) const;

    // advances the animation if its time to do so
    // should be called every frame
    void update(); 

private:
    int texture_index = 0;
    uint32_t last_advance_time_ms = 0;
};

#endif