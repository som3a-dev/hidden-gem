#ifndef _TEXTBOX_H
#define _TEXTBOX_H

#include <raylib.h>

#include <string>

struct Textbox
{
    Rectangle box;
    std::string text;

    float percent_visible = 1; // percentage of text that is visible, from 0 to 1
    int chars_per_second = 12; // how many characters are shown per second

    bool sped_up = false; // if true, text is written twice as fast as chars_per_second

    bool show_text = false; // if true, gradually show more of the text

    void update();
    void draw();
};

#endif