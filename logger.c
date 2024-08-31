#include "logger.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <locale.h>
#include <stdlib.h>

// ���������� ���������� ��� �������� ��������� �����������
static int logging_enabled = 1;

// ����������� ������ �� UTF-8 � Windows-1251
// ��� ������� �������� ���������� � ������� ������������� ��� ����������� ������
char* utf8_to_win1251(const char *str) {
    // � �������� ��������� ��� ����������� ����������, ��������, iconv, ��� ����������� ���������
    // ����� ������������ ���������� ������, ������ ��� �������
    // ��� ����� ����������� ��� ������� ���������
    return strdup(str); // �������� ��� �� ��������� ����������
}

static const char* log_level_to_string(LogLevel level) {
    switch (level) {
        case LOG_INFO:    return "INFO";
        case LOG_WARNING: return "WARNING";
        case LOG_ERROR:   return "ERROR";
        default:          return "UNKNOWN";
    }
}

void init_logging(int enable) {
    logging_enabled = enable;
}

void log_message(LogLevel level, const char *message, ...) {
    if (!logging_enabled) {
        return; // ���� ����������� ���������, ������ �� ������
    }

    FILE *log_file = fopen("logfile.log", "a"); // ��������� ���� � ���������� WIN1251
    if (log_file == NULL) {
        perror("Error opening log file");
        return;
    }

    // ��������� ������ ��� ���������� ��������� �����
    setlocale(LC_CTYPE, "");

    // ��������� �������� �������
    time_t rawtime;
    struct tm *timeinfo;
    char time_str[20]; // ����� ��� ������ �������

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

    // ������ ������ ����������� � ������� � ����
    fprintf(log_file, "[%s] [%s] ", time_str, log_level_to_string(level));

    // ������ ���������������� ���������
    va_list args;
    va_start(args, message);
    char utf8_message[1024];
    vsnprintf(utf8_message, sizeof(utf8_message), message, args);

    // ����������� ��������� �� UTF-8 � Windows-1251
    char *win1251_message = utf8_to_win1251(utf8_message);
    fprintf(log_file, "%s\n", win1251_message);

    va_end(args);
    free(win1251_message);

    fclose(log_file);
}
