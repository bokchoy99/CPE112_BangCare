#ifndef LOGGER_H
#define LOGGER_H

typedef enum {
    LOG_INFO,    /* General operational events (patient arrival, bed assigned). */
    LOG_WARNING, /* Non-critical issues (bed full, ID not found).               */
    LOG_ERROR,   /* Failures requiring attention (malloc failed).               */
    LOG_SYSTEM   /* System-level state changes (tick advance, init).            */
} LogLevel;

void initLogger(void);
void logEvent(LogLevel level, const char* module, const char* message);

#endif