#include "dialog.h"
#include "log.h"

#ifdef _WIN32
#include <Windows.h>
#endif

char* dialog_select_file()
{
#ifdef _WIN32

    char* filepath = malloc(sizeof(char) * MAX_PATH);
    filepath[0] = '\0';

    char current_dir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, current_dir);

    OPENFILENAMEA o = {0};
    o.lStructSize = sizeof(OPENFILENAMEA);
    o.lpstrFile = filepath;
    o.nMaxFile = MAX_PATH;
    o.lpstrInitialDir = current_dir;
    o.Flags = OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&o) == FALSE)
    {
        LOG_INFO("Select file dialog failed, code: %d", CommDlgExtendedError());
        free(filepath);
        return NULL;
    }

    return filepath;
#endif
}
