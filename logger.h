#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>

typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} LogLevel;

// Функция для инициализации логирования
void init_logging(int enable);

// Функция для записи сообщения в лог
void log_message(LogLevel level, const char *message, ...);

#endif // LOGGER_H
