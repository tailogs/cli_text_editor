#include "console_utils.h"

// Устанавливаем символ в указанную позицию на экране
void setPixel(int x, int y, wchar_t symbol, const char* text_color, const char* background_color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD position = {x, y};

    SetConsoleCursorPosition(hConsole, position);
    _setmode(_fileno(stdout), _O_U16TEXT);
    wprintf(L"\033[0;%s;%sm%lc%s", text_color, background_color, symbol, CMD_RESET_COLOR);
}

// Вывод текста в центр консоли
void centerText(wchar_t *text, const char* text_color, const char* background_color) {
    int textLength = wcslen(text);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    int leftPadding = (consoleWidth - textLength) / 2;
    int topPadding = (consoleHeight - 1) / 2;

    setlocale(LC_ALL, "");
    _setmode(_fileno(stdout), _O_U16TEXT);

    for (int i = 0; i < textLength; i++) {
        setPixel(leftPadding + i, topPadding, text[i], text_color, background_color);
    }
}

// Очистка консоли
void clearConsole() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD cellCount;
    DWORD count;
    COORD homeCoords = {0, 0};

    if (hConsole == INVALID_HANDLE_VALUE) return;

    // Получаем количество ячеек в буфере
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    // Заполняем буфер пробелами
    if (!FillConsoleOutputCharacter(hConsole, (TCHAR)' ', cellCount, homeCoords, &count)) return;

    // Заполняем буфер атрибутами пробела
    if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoords, &count)) return;

    // Перемещаем курсор домой
    SetConsoleCursorPosition(hConsole, homeCoords);
}

// Скрытие курсора
void hideCursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

// Отображение курсора
void showCursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void replaceSpacesWithTabs(char *str) {
    char buffer[1024]; // Буфер для хранения результата
    int i = 0, j = 0;

    while (str[i] != '\0') {
        // Проверяем, есть ли 4 пробела подряд
        if (str[i] == ' ' && str[i + 1] == ' ' && str[i + 2] == ' ' && str[i + 3] == ' ') {
            buffer[j++] = '\t'; // Заменяем 4 пробела на таб
            i += 4; // Пропускаем 4 пробела
        } else {
            buffer[j++] = str[i++]; // Копируем текущий символ
        }
    }
    buffer[j] = '\0'; // Завершаем строку

    strcpy(str, buffer); // Копируем результат обратно в исходную строку
}