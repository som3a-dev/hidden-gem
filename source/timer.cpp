#include "timer.h"

#include <raylib.h>

void Timer::start()
{
    running = true;
    start_ms = GetTime() * 1000;
}

void Timer::stop()
{
    running = false;
}

void Timer::update()
{
    if (running)
    {
        double current_ms = GetTime() * 1000;
        if ((current_ms - start_ms) >= duration_ms)
        {
            if (on_timeout)
            {
                on_timeout(parent, user_data);
            }

            running = false;
            if (!one_shot)
            {
                start();
            }
        }
    }
}
