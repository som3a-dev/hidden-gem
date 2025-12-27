#ifndef _FRAME_ANIMATION
#define _FRAME_ANIMATION

#include <string>
#include <vector>

#include "raylib.h"

struct AssetManager;

struct Frame
{
    int x = 0;
    int y = 0;
};

struct FrameAnimation
{
    std::string id;
    std::vector<Frame> frames;

    // the time each frame lasts in ms
    uint32_t interval_ms = 0;

    // sheet_width/height : width and height of the sheet in sprites
    // e.g a 8x8 sheet with 8 horizontal frames and 8 vertical frames 
    void set_sheet(const std::string& sheet_texture_id,
                   const AssetManager& asset_m,
                   int sheet_width,
                   int sheet_height);

    // Pushes all frames in a specified range of [x1, x2] and [y1, y2]
    void push_frame_interval(int x1, int x2, int y1, int y2);

    inline const std::string& get_sheet() const
    {
        return sheet_texture;
    }

    inline int get_frame_width() const
    {
        return frame_width;
    }

    inline int get_frame_height() const
    {
        return frame_height;
    }

    void update();

    Rectangle get_current_frame_src() const;

private:
    
    std::string sheet_texture;

    int frame_index = 0;

    uint32_t last_advance_time_ms = 0;

    int frame_width = 0;
    int frame_height = 0;
};

#endif