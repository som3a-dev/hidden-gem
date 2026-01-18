#ifndef _LOG_H
#define _LOG_H

enum
{
    _LOG_INFO,
    _LOG_WARNING,
    _LOG_ERROR,
    _LOG_CRITICAL
};

void log_init();
void log_close();
void _log_msg(int level, const char* msg_format, ...);


#define LOG_INFO(msg, ...) _log_msg(_LOG_INFO, msg, ##__VA_ARGS__)
#define LOG_WARNING(msg, ...) _log_msg(_LOG_WARNING, msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) _log_msg(_LOG_ERROR, msg, ##__VA_ARGS__)
#define LOG_CRITICAL(msg, ...) _log_msg(_LOG_CRITICAL, msg, ##__VA_ARGS__)

#endif