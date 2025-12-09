#define _CRTDBG_MAP_ALLOC

#include "game.h"

#include <cstdlib>
#include <cstdio>
#include <crtdbg.h>

static int run()
{
    Game game = {0};
    game.init();
    game.loop();
    game.destroy();

    return 0;
}

int main(void)
{
    _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
    _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
    _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    
    int code = run();

    _CrtDumpMemoryLeaks();

    return code;
}


//NEXT(): Animation, Animated sprite (DONE)
//NEXT(): Asset loading and retrieving (DONE)
//NEXT(): ECS (DONE)