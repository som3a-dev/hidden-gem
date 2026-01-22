#include "dialog.h"
#include "log.h"

#ifdef _WIN32
#include <Windows.h>
#endif

char* dialog_select_file(DIALOG_SELECT_FILE_TYPE type)
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
    if (type == DIALOG_SELECT_FILE_OPEN)
    {
        o.Flags = OFN_FILEMUSTEXIST;
        if (GetOpenFileNameA(&o) == FALSE)
        {
            LOG_INFO("Select file dialog failed, code: %d", CommDlgExtendedError());
            free(filepath);
            return NULL;
        }
    }
    else if (type == DIALOG_SELECT_FILE_SAVE)
    {
        o.Flags = OFN_CREATEPROMPT;
        if (GetSaveFileNameA(&o) == FALSE)
        {
            LOG_INFO("Select file dialog failed, code: %d", CommDlgExtendedError());
            free(filepath);
            return NULL;
        }
    }
    else
    {
        LOG_ERROR("Invalid file select dialog type specified, no dialog opened.");
        free(filepath);
        return NULL;
    }

    return filepath;
#endif
}
