#ifndef CONSOLE_UTILS_H
#define CONSOLE_UTILS_H

#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <conio.h>

// Устанавливает символ в указанную позицию на экране
void setPixel(int x, int y, wchar_t symbol);

// Выводит текст в центр консоли
void centerText(wchar_t *text);

// Очищает консоль
void clearConsole();

// Скрывает курсор
void hideCursor();

// Отображает курсор
void showCursor();

#endif // CONSOLE_UTILS_H
