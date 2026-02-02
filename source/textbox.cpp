#include "textbox.h"

void Textbox::update()
{
    if (percent_visible == 1)
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            percent_visible = 0;
        }
    }

    if (IsKeyPressed(KEY_K))
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
    const int border = 4;

    DrawRectanglePro(box, {0, 0}, 0, BLACK);
    DrawRectangleLinesEx(box, border, WHITE);

    float text_x = box.x + border * 2;
    float text_y = box.y + border * 2;

    Font font = GetFontDefault();

    float char_x = text_x;
    float char_y = text_y;

    float spacing = 2;

    int words_count;
    char** words = TextSplit(text.c_str(), ' ', &words_count);

    int space_width;
    {
        char space[] = {' ', '\0'};
        space_width = MeasureText(space, 24);
    }

    size_t chars_drawn = 0;
    for (int i = 0; i < words_count; i++)
    {
        const char* word = words[i];
        Vector2 word_sz = MeasureTextEx(font, word, 24, spacing);

        if ((char_x + word_sz.x) >= (box.x + box.width))
        {
            char_x = text_x;
            char_y += word_sz.y;
        }

        if ((char_y + word_sz.y) >= (box.y + box.height))
        {
            box.height += ((char_y + word_sz.y) - (box.y + box.height)) + word_sz.y;
        }

        for (int j = 0; j < strlen(word); j++)
        {
            if (chars_drawn >= (text.length() * percent_visible))
            {
                goto done_drawing;
            }

            DrawTextCodepoint(font, word[j], {char_x, char_y}, 24, GRAY);

            char chars[] = {word[j], '\0'};
            Vector2 size = MeasureTextEx(font, chars, 24, 0);
            char_x += size.x + spacing;

            if ((char_x + size.x) >= (box.x + box.width))
            {
                char_x = text_x;
                char_y += size.y;
            }

            chars_drawn++;
        }

        char chars[] = {' ', '\0'};
        Vector2 size = MeasureTextEx(font, chars, 24, 0);
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