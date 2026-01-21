#include "log.h"

#include <stdarg.h>
#include <stdio.h>

#define BUF_SIZE 512

static FILE* fp = NULL;
const char* log_filename = "log.txt";

void log_init()
{
    if (fp)
    {
        fclose(fp);
        fp = NULL;
    }

    fp = fopen(log_filename, "w");
}

void log_close()
{
    fclose(fp);
    fp = NULL;
}

void _log_msg(int level, const char* msg_format, ...)
{
    char buf[BUF_SIZE];

    const char* level_str = "(UNKNOWN LOG LEVEL)";
    switch (level)
    {
        case _LOG_INFO:
        {
            level_str = "INFO";
        } break;

        case _LOG_WARNING:
        {
            level_str = "WARNING";
        } break;

        case _LOG_ERROR:
        {
            level_str = "ERROR";
        } break;

        case _LOG_CRITICAL:
        {
            level_str = "CRITICAL";
        } break;
    }

    snprintf(buf, BUF_SIZE, "LOG [%s]: ", level_str);
    fprintf(stderr, buf);
    if (fp)
    {
        fprintf(fp, buf);
    }

    // Print the message itself
    va_list args;
    va_start(args, msg_format);
    vfprintf(stderr, msg_format, args);
    if (fp)
    {
        vfprintf(fp, msg_format, args);
    }
    va_end(args);

    fprintf(stderr, "\n");
    if (fp)
    {
        fprintf(fp, "\n");
    }
}