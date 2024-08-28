#ifndef CONSOLE_UTILS_H
#define CONSOLE_UTILS_H

#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <conio.h>
#include "syntax.h"

// Устанавливает символ в указанную позицию на экране
void setPixel(int x, int y, wchar_t symbol, const char* text_color, const char* background_color);

// Выводит текст в центр консоли
void centerText(wchar_t *text, const char* text_color, const char* background_color);

// Очищает консоль
void clearConsole();

// Скрывает курсор
void hideCursor();

// Отображает курсор
void showCursor();

#endif // CONSOLE_UTILS_H
