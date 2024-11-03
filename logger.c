#include "logger.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <locale.h>
#include <stdlib.h>

// Глобальная переменная для хранения состояния логирования
static int logging_enabled = 1;
static char *path = NULL;

// Конвертация строки из UTF-8 в Windows-1251
// Эта функция является упрощенной и требует корректировки для полноценной работы
char* utf8_to_win1251(const char *str) {
    // В реальной программе вам понадобится библиотека, например, iconv, для конвертации кодировок
    // Здесь представлена упрощенная версия, только для примера
    // Вам нужно реализовать эту функцию корректно
    return strdup(str); // Замените это на настоящую реализацию
}

static const char* log_level_to_string(LogLevel level) {
    switch (level) {
        case LOG_INFO:    return "INFO";
        case LOG_WARNING: return "WARNING";
        case LOG_ERROR:   return "ERROR";
        default:          return "UNKNOWN";
    }
}

void init_logging(int enable, char *path_temp) {
    logging_enabled = enable;

    // Освобождаем предыдущую память, если она была выделена
    if (path != NULL) {
        free(path);
    }

    // Выделяем память для path
    path = malloc(strlen(path_temp) + 1); // +1 для нуль-терминатора
    if (path == NULL) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        return;
    }

    // Копируем строку
    strcpy(path, path_temp);
    strcat(path, "\\logfile.log");
}

void log_message(LogLevel level, const char *message, ...) {
    if (!logging_enabled) {
        return; // Если логирование выключено, ничего не делаем
    }

    FILE *log_file = fopen(path, "a"); // Открываем файл с кодировкой WIN1251
    if (log_file == NULL) {
        perror("Error opening log file");
        return;
    }

    // Установка локали для корректной обработки строк
    setlocale(LC_CTYPE, "");

    // Получение текущего времени
    time_t rawtime;
    struct tm *timeinfo;
    char time_str[20]; // Буфер для строки времени

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

    // Запись уровня логирования и времени в файл
    fprintf(log_file, "[%s] [%s] ", time_str, log_level_to_string(level));

    // Запись форматированного сообщения
    va_list args;
    va_start(args, message);
    char utf8_message[1024];
    vsnprintf(utf8_message, sizeof(utf8_message), message, args);

    // Конвертация сообщения из UTF-8 в Windows-1251
    char *win1251_message = utf8_to_win1251(utf8_message);
    fprintf(log_file, "%s\n", win1251_message);

    va_end(args);
    free(win1251_message);

    fclose(log_file);
}
