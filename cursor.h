#ifndef CURSOR_H
#define CURSOR_H

#include <windows.h>

void set_cursor_position(int x, int y, COORD *cursorPosition, HANDLE hConsole);

#endif // CURSOR_H
