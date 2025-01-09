#pragma once

typedef enum
{
    LOG_LVL_INFO = 1,
    LOG_LVL_WARNING = 2,
    LOG_LVL_ERROR = 3,
} LogLevel;

void debug_log(LogLevel lvl, const char *msg, ...);
