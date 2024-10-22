#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <dirent.h>
#include <signal.h>
#include <direct.h>  // для _getcwd
#include <string.h>
#include <sys/stat.h>  // для получения информации о файле
#include <time.h>
#include "explorer.h"
#include "main.h"

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13
#define KEY_ESC 27
#define KEY_CTRL_Q 17

// Цвета для отображения
#define COLOR_DEFAULT 7
#define COLOR_SELECTED 112
#define COLOR_DIRECTORY 9

FileEntry entries[256];
int totalEntries = 0;
int currentSelection = 0;
char currentPath[MAX_PATH];

// Для хранения предыдущего выбора
int previousSelection = -1;

// Функция для скрытия и отображения курсора
void setCursorVisibility(int visible) {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = visible;  // скрыть (0) или показать (1)
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

void setConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void moveCursor(int x, int y) {
    COORD coord = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void clearLine() {
    DWORD written;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    FillConsoleOutputCharacter(hConsole, ' ', csbi.dwSize.X, csbi.dwCursorPosition, &written);
    SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
}

// Функция для форматирования размера файлов в человекочитаемый вид
void formatFileSize(long long size, char* out) {
    if (size < 1024) {
        sprintf(out, "%lld B", size);
    } else if (size < 1024 * 1024) {
        sprintf(out, "%.2f KB", size / 1024.0);
    } else if (size < 1024 * 1024 * 1024) {
        sprintf(out, "%.2f MB", size / (1024.0 * 1024));
    } else {
        sprintf(out, "%.2f GB", size / (1024.0 * 1024 * 1024));
    }
}

// Функция для получения информации о файле
void getFileInfo(const char *path, FileEntry *entry) {
    WIN32_FILE_ATTRIBUTE_DATA fileData;
    if (GetFileAttributesEx(path, GetFileExInfoStandard, &fileData)) {
        SYSTEMTIME creationTime, modTime;
        FileTimeToSystemTime(&fileData.ftCreationTime, &creationTime);
        FileTimeToSystemTime(&fileData.ftLastWriteTime, &modTime);

        // Записываем даты
        sprintf(entry->creationDate, "%02d-%02d-%d", creationTime.wDay, creationTime.wMonth, creationTime.wYear);
        sprintf(entry->modificationDate, "%02d-%02d-%d", modTime.wDay, modTime.wMonth, modTime.wYear);

        // Записываем размер файла
        LARGE_INTEGER fileSize;
        fileSize.LowPart = fileData.nFileSizeLow;
        fileSize.HighPart = fileData.nFileSizeHigh;
        entry->fileSize = fileSize.QuadPart;
    } else {
        // Если не удалось получить информацию
        strcpy(entry->creationDate, "Unknown");
        strcpy(entry->modificationDate, "Unknown");
        entry->fileSize = -1;
    }
}

void drawDirectory() {
    moveCursor(0, 0);
    clearLine();
    printf("Path: %s\n", currentPath);

    // Очищаем старое содержимое
    for (int i = totalEntries; i < 256; i++) {
        moveCursor(0, i + 1);
        clearLine();
    }

    // Перемещаем курсор для отображения файлов с доп. информацией
    for (int i = 0; i < totalEntries; i++) {
        moveCursor(0, i + 1);  // +1, так как первая строка - это путь
        clearLine();
        
        if (i == currentSelection) {
            setConsoleColor(COLOR_SELECTED);
        } else if (entries[i].isDir) {
            setConsoleColor(COLOR_DIRECTORY);
        } else {
            setConsoleColor(COLOR_DEFAULT);
        }

        char fileSizeStr[20];
        formatFileSize(entries[i].fileSize, fileSizeStr);
        printf("%-30s %10s %20s %20s\n", entries[i].name, fileSizeStr, entries[i].creationDate, entries[i].modificationDate);

        setConsoleColor(COLOR_DEFAULT);
    }
}

void updateSelection() {
    if (previousSelection != -1 && previousSelection != currentSelection) {
        moveCursor(0, previousSelection + 1);
        clearLine();

        if (entries[previousSelection].isDir) {
            setConsoleColor(COLOR_DIRECTORY);
        } else {
            setConsoleColor(COLOR_DEFAULT);
        }

        char fileSizeStr[20];
        formatFileSize(entries[previousSelection].fileSize, fileSizeStr);
        printf("%-30s %10s %20s %20s\n", entries[previousSelection].name, fileSizeStr, entries[previousSelection].creationDate, entries[previousSelection].modificationDate);
    }

    moveCursor(0, currentSelection + 1);
    setConsoleColor(COLOR_SELECTED);
    clearLine();

    char fileSizeStr[20];
    formatFileSize(entries[currentSelection].fileSize, fileSizeStr);
    printf("%-30s %10s %20s %20s\n", entries[currentSelection].name, fileSizeStr, entries[currentSelection].creationDate, entries[currentSelection].modificationDate);

    setConsoleColor(COLOR_DEFAULT);

    previousSelection = currentSelection;
}

void loadDirectory(const char *path) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    char searchPath[MAX_PATH];

    snprintf(searchPath, sizeof(searchPath), "%s\\*", path);
    
    hFind = FindFirstFile(searchPath, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error opening directory: %s\n", path);
        return;
    }

    totalEntries = 0;
    if (strcmp(path, "C:\\") != 0) {
        strcpy(entries[totalEntries].name, "..");
        entries[totalEntries].isDir = 1;
        totalEntries++;
    }

    do {
        if (findFileData.cFileName[0] != '.') {
            strcpy(entries[totalEntries].name, findFileData.cFileName);
            entries[totalEntries].isDir = (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;

            char fullPath[MAX_PATH];
            snprintf(fullPath, sizeof(fullPath), "%s\\%s", path, findFileData.cFileName);
            getFileInfo(fullPath, &entries[totalEntries]);

            totalEntries++;
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
}

void navigateUp() {
    char *lastSlash = strrchr(currentPath, '\\');
    if (lastSlash) {
        *lastSlash = '\0';
    } else {
        strcpy(currentPath, "C:\\");
    }
}

void handleEnter() {
    if (strcmp(entries[currentSelection].name, "..") == 0) {
        navigateUp();  // Поднимаемся на уровень выше
    } else if (entries[currentSelection].isDir) {
        // Спускаемся в подкаталог
        char newPath[MAX_PATH];
        snprintf(newPath, sizeof(newPath), "%s\\%s", currentPath, entries[currentSelection].name);
        strcpy(currentPath, newPath);
    } else {
        // Если выбранный элемент — файл, формируем путь
        // Используем только текущий каталог, если не поднялись
        char selectedFilePath[MAX_PATH];
        snprintf(selectedFilePath, sizeof(selectedFilePath), "%s\\%s", currentPath, entries[currentSelection].name);
        
        printf("Opening file: %s\n", selectedFilePath);  // Выводим путь к файлу
        return;  // Выход из функции
    }
    
    loadDirectory(currentPath);  // Загружаем директорию после перехода
    currentSelection = 0;  // Сброс выбора
    previousSelection = -1;  // Обновляем предыдущее выделение
    drawDirectory();  // Перерисовываем интерфейс
}

const char* startExplorer() {
    static char selectedFilePath[MAX_PATH];  // Для хранения пути к выбранному файлу

    setCursorVisibility(0);  // Скрыть курсор

    // Проверяем текущую директорию
    if (_getcwd(currentPath, sizeof(currentPath)) == NULL) {
        perror("Failed to get current directory");
        return NULL;
    }

    loadDirectory(currentPath);  // Загружаем содержимое директории
    drawDirectory();  // Отрисовываем интерфейс

    while (1) {
        int c = _getch();  // Читаем нажатие клавиши
        if (c == 224) {  // Спецклавиши (стрелки)
            c = _getch();  // Второй код для стрелок
            if (c == KEY_UP && currentSelection > 0) {
                currentSelection--;  // Сдвиг выбора вверх
            } else if (c == KEY_DOWN && currentSelection < totalEntries - 1) {
                currentSelection++;  // Сдвиг выбора вниз
            }
            updateSelection();  // Обновляем выделение в интерфейсе
        } else if (c == KEY_ENTER) {
            handleEnter();  // Обработка нажатия Enter
            if (!entries[currentSelection].isDir) {  // Если выбранный элемент — файл
                // Формируем путь к файлу с нужным форматом
                snprintf(selectedFilePath, sizeof(selectedFilePath), "%s\\%s", currentPath, entries[currentSelection].name);
                
                setCursorVisibility(1);  // Показываем курсор
                return selectedFilePath;  // Возвращаем путь к выбранному файлу
            }
        } else if (c == KEY_ESC) {
            navigateUp();  // Переход на уровень выше
            loadDirectory(currentPath);  // Загружаем директорию после перехода
            currentSelection = 0;  // Сброс выбора
            previousSelection = -1;  // Обновляем предыдущее выделение
            drawDirectory();  // Перерисовываем интерфейс
        } else if (c == KEY_CTRL_Q) {
            return NULL;  // Возвращаем NULL при выходе без выбора файла
        }
    }

    setCursorVisibility(1);  // Показываем курсор при выходе
    return NULL;  // Возвращаем NULL при выходе без выбора файла
}

