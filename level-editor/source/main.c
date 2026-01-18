#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "editor.h"
#include "log.h"

#include <stdio.h>

static void init_crtdbg();
static void dump_crtdbg();

int main(void)
{
    #ifdef _DEBUG
	init_crtdbg();
    #endif

    log_init();

	run_editor();

    log_close();

    #ifdef _DEBUG
	dump_crtdbg();
    #endif

	return 0;
}

static void init_crtdbg()
{
    _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
    _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
    _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}

static void dump_crtdbg()
{
	_CrtDumpMemoryLeaks();
}
