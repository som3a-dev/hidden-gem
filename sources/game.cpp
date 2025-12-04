#include "game.h"

void Game::init()
{
    screen_width = 1280;
    screen_height = 720;
    
    InitWindow(screen_width, screen_height, "Hidden GEM");
    SetTargetFPS(60);

    texture = LoadTexture(ASSETS_PATH"normal_crate.png"); 

    texture_speed = 3.0f;
}

void Game::destroy()
{
    UnloadTexture(texture);
    CloseWindow();
}

void Game::loop()
{
    running = true;
    while (running)
    {
        update();
        draw();
    }
}

void Game::update()
{
    if (WindowShouldClose())
    {
        running = false;
        return;
    }

    if (IsKeyDown(KEY_D))
    {
        texture_pos.x += texture_speed;
    }
    else if (IsKeyDown(KEY_A))
    {
        texture_pos.x -= texture_speed;
    }

    if (IsKeyDown(KEY_S))
    {
        texture_pos.y += texture_speed;
    }
    else if (IsKeyDown(KEY_W))
    {
        texture_pos.y -= texture_speed;
    }

}

void Game::draw()
{
    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawTextureEx(texture, texture_pos, 0, 5.0f, WHITE);

    const char* text = "Hidden GEM";
    const Vector2 text_size = MeasureTextEx(GetFontDefault(), text, 20, 1);
    int text_x = static_cast<int>(screen_width / 2 - text_size.x / 2);
    int text_y = static_cast<int>(screen_height * 0.6);

    DrawText(text, text_x, text_y, 20, BLACK);

    EndDrawing();
}
