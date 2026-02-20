#ifndef _TIMER_H
#define _TIMER_H

struct Timer
{
    int duration_ms = 1000;
    bool one_shot = true;

    void start();
    void stop();
    void update();

    bool is_running()
    {
        return running;
    }

    void (*on_timeout)(void* parent, void* user_data) = nullptr;
    void* parent = nullptr;
    void* user_data = nullptr;

private:
    
    double start_ms = 0;
    bool running = false;
};

#endif