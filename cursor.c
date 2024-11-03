#include "cursor.h"

void set_cursor_position(int x, int y, COORD *cursorPosition, HANDLE hConsole) {
    cursorPosition->X = x;
    cursorPosition->Y = y;
    SetConsoleCursorPosition(hConsole, *cursorPosition);
}
