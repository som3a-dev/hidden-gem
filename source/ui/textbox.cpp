#include "textbox.h"


// TODO(omar): Update textbox to use the word wrapping code in popup image, and use QueueText

namespace UI
{
    void Textbox::set_box(Rectangle new_box)
    {
        box = new_box;
        og_box = new_box;
    }

    void Textbox::update()
    {
        if (!visible)
        {
            percent_visible = 0;
            box = og_box;
            return;
        }

        if (percent_visible == 1)
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                percent_visible = 0;
            }
        }

        if (IsKeyPressed(KEY_M))
        {
            percent_visible = 1;
        }

        sped_up = IsKeyDown(KEY_ENTER);
        if (percent_visible < 1)
        {
            int speed = chars_per_second;
            if (sped_up)
            {
                speed *= 2;
            }

            float show_speed = (float)(speed) / (float)(text.length());
            percent_visible += show_speed * GetFrameTime();
        }
        else if (percent_visible > 1)
        {
            percent_visible = 1;
        }
    }

    void Textbox::draw()
    {
        if (!visible)
        {
            return;
        }

        const int border = 1;
        const int fontsz = 12;

        DrawRectanglePro(box, {0, 0}, 0, {20, 20, 20, 255});
        DrawRectangleLinesEx(box, border, WHITE);

        float text_x = box.x + border * 2;
        float text_y = box.y + border * 2;

        Font font = GetFontDefault();

        float char_x = text_x;
        float char_y = text_y;

        float spacing = 2;

        int words_count;
        char** words = TextSplit(text.c_str(), ' ', &words_count);

        size_t chars_drawn = 0;
        for (int i = 0; i < words_count; i++)
        {
            const char* word = words[i];
            Vector2 word_sz = MeasureTextEx(font, word, fontsz, spacing);

            if ((char_x + word_sz.x) >= (box.x + box.width))
            {
                char_x = text_x;
                char_y += word_sz.y;
            }

            if ((char_y + word_sz.y) >= (box.y + box.height))
            {
                float new_h = ((char_y + word_sz.y) - (box.y + box.height)) + word_sz.y;
                box.height += new_h;
                char_y -= new_h;
                box.y -= new_h;
            }

            for (int j = 0; j < strlen(word); j++)
            {
                if (chars_drawn >= (text.length() * percent_visible))
                {
                    goto done_drawing;
                }

                DrawTextCodepoint(font, word[j], {char_x, char_y}, fontsz, WHITE);

                char chars[] = {word[j], '\0'};
                Vector2 size = MeasureTextEx(font, chars, fontsz, 0);
                char_x += size.x + spacing;

                if ((char_x + size.x) >= (box.x + box.width))
                {
                    char_x = text_x;
                    char_y += size.y;
                }

                chars_drawn++;
            }

            char chars[] = {' ', '\0'};
            Vector2 size = MeasureTextEx(font, chars, fontsz, 0);
            char_x += size.x;
        }

        done_drawing:
        return;

    /*    for (int i = 0; i < text.length() * percent_visible; i++)
        {
            DrawTextCodepoint(font, text[i], {char_x, char_y}, 24, GRAY);

            char chars[] = {text[i], '\0'};
            Vector2 size = MeasureTextEx(font, chars, 24, 0);
            char_x += size.x + spacing;

            if ((char_x + size.x) >= (box.x + box.width))
            {
                char_x = text_x;
                char_y += size.y;
            }
        }*/
    }
}