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
#define KEY_CTRL_Q 17 // Ctrl + Q
#define KEY_CTRL_F 6  // Ctrl + F
#define KEY_CTRL_C 3  // Ctrl + C
#define KEY_CTRL_D 4  // Ctrl + D
#define KEY_CTRL_R 18 // Ctrl + R

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

// Добавим пайпы для ввода/вывода
HANDLE hReadPipe, hWritePipe;
PROCESS_INFORMATION piProcInfo;
STARTUPINFO siStartInfo;

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
            size_t result = snprintf(fullPath, sizeof(fullPath), "%s\\%s", path, findFileData.cFileName);
			if (result >= sizeof(fullPath)) {
				// Обработка ошибки
			}
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
        int result = snprintf(newPath, sizeof(newPath), "%s\\%s", currentPath, entries[currentSelection].name);
		if ((size_t)result >= sizeof(newPath)) {
			// Обработка ошибки
		}
        strcpy(currentPath, newPath);
    } else {
        // Если выбранный элемент — файл, формируем путь
        // Используем только текущий каталог, если не поднялись
        char selectedFilePath[MAX_PATH];
        size_t result = snprintf(selectedFilePath, sizeof(selectedFilePath), "%s\\%s", currentPath, entries[currentSelection].name);
		if (result >= sizeof(selectedFilePath)) {
			// Обработка ошибки
		}
        
        printf("Opening file: %s\n", selectedFilePath);  // Выводим путь к файлу
        return;  // Выход из функции
    }
    
    loadDirectory(currentPath);  // Загружаем директорию после перехода
    currentSelection = 0;  // Сброс выбора
    previousSelection = -1;  // Обновляем предыдущее выделение
    drawDirectory();  // Перерисовываем интерфейс
}

void displayMessageTemporarily(const char *message) {
    moveCursor(0, 10);  // Позиция для вывода сообщения
    clearLine();        // Очищаем строку
    printf("%s\n", message);
    Sleep(1000);        // Ждем 1 секунду
    loadDirectory(currentPath);  // Обновляем содержимое каталога
    drawDirectory();    // Перерисовываем интерфейс
}

void copyDirectory() {
    char source[MAX_PATH], destination[MAX_PATH];
    printf("Enter the source directory: ");
    scanf("%s", source);
    printf("Enter the destination directory: ");
    scanf("%s", destination);

    // Функция для копирования каталога
    // Здесь должен быть код для копирования каталога, в зависимости от вашей логики
}

void createFile() {
    char fileName[MAX_PATH];
    printf("Enter the name of the file to create: ");
    scanf("%s", fileName);

    // Формируем полный путь к файлу
    char fullPath[MAX_PATH];
    size_t result = snprintf(fullPath, sizeof(fullPath), "%s\\%s", currentPath, fileName);
	if (result >= sizeof(fullPath)) {
		// Обработка ошибки
	}

    FILE *file = fopen(fullPath, "w");
    if (file) {
        fclose(file);
        displayMessageTemporarily("File created successfully."); // Изменение здесь
    } else {
        displayMessageTemporarily("Error creating file."); // Изменение здесь
    }
}

void createDirectory() {
    char dirName[MAX_PATH];
    printf("Enter the name of the directory to create: ");
    scanf("%s", dirName);

    // Формируем полный путь к каталогу
    char fullPath[MAX_PATH];
    size_t result = snprintf(fullPath, sizeof(fullPath), "%s\\%s", currentPath, dirName);
	if (result >= sizeof(fullPath)) {
		// Обработка ошибки
	}

    if (CreateDirectory(fullPath, NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {
        displayMessageTemporarily("Directory created successfully."); // Изменение здесь
    } else {
        displayMessageTemporarily("Error creating directory."); // Изменение здесь
    }
}

void confirmDelete(const char* name) {
    char fullPath[MAX_PATH];
    size_t result = snprintf(fullPath, sizeof(fullPath), "%s\\%s", currentPath, name);
	if (result >= sizeof(fullPath)) {
		// Обработка ошибки
	}

    int confirm = 0;  // 0 - no, 1 - yes
    while (1) {
        moveCursor(0, 10);  // Позиция для вывода сообщения
        printf("Are you sure you want to delete '%s'? (Y/N) ", fullPath);
        int c = _getch();
        if (c == 'Y' || c == 'y') {
            confirm = 1;
            break;
        } else if (c == 'N' || c == 'n') {
            confirm = 0;
            break;
        }
    }

    if (confirm) {
        // Сначала пробуем удалить каталог, если он не пустой, то удаляем файлы
        if (RemoveDirectory(fullPath)) {
            displayMessageTemporarily("Deleted directory successfully."); // Изменение здесь
        } else {
            // Если не удалось удалить как каталог, пробуем удалить как файл
            if (DeleteFile(fullPath)) {
                displayMessageTemporarily("Deleted file successfully."); // Изменение здесь
            } else {
                displayMessageTemporarily("Error deleting."); // Изменение здесь
            }
        }
    }
}


void handleDelete() {
    const char* selectedPath = entries[currentSelection].name; // Получаем путь к выделенному элементу
    confirmDelete(selectedPath);
}

void initTerminalEmulator() {
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE; // Позволяем потомкам наследовать дескрипторы
    saAttr.lpSecurityDescriptor = NULL;

    // Создаем пайпы
    if (!CreatePipe(&hReadPipe, &hWritePipe, &saAttr, 0)) {
        perror("CreatePipe failed");
        return;
    }

    // Создаем процесс для эмуляции терминала
    ZeroMemory(&siStartInfo, sizeof(siStartInfo));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdInput = hReadPipe;
    siStartInfo.hStdOutput = hWritePipe;
    siStartInfo.hStdError = hWritePipe;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // Запускаем консоль
    if (!CreateProcess("C:\\Windows\\System32\\cmd.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo)) {
        perror("CreateProcess failed");
        return;
    }
    
    // Закрываем ненужные дескрипторы
    CloseHandle(hWritePipe);
}

void sendToTerminal(const char* command) {
    DWORD written;
    WriteFile(hReadPipe, command, strlen(command), &written, NULL);
    WriteFile(hReadPipe, "\n", 1, &written, NULL); // Отправляем новую строку
}

void readFromTerminal() {
    char buffer[4096];
    DWORD read;
    
    // Читаем вывод из терминала
    while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &read, NULL) && read > 0) {
        buffer[read] = '\0'; // Завершаем строку
        printf("%s", buffer); // Выводим на экран
    }
}

void handleCtrlR() {
    // Эмулируем терминал
    initTerminalEmulator();
    
    // Цикл для обработки ввода
    while (1) {
        if (_kbhit()) {
            int c = _getch();
            if (c == KEY_ESC) {
                // Закрываем эмулятор при нажатии ESC
                break;
            } else {
                // Обработка ввода команд
                char command[256];
                printf("Enter command: ");
                fgets(command, sizeof(command), stdin); // Получаем команду
                sendToTerminal(command); // Отправляем команду
                readFromTerminal(); // Читаем ответ
            }
        }
    }

    // Завершаем процесс
    TerminateProcess(piProcInfo.hProcess, 0);
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);
    CloseHandle(hReadPipe);
}

char* startExplorer() {
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
				size_t result = snprintf(selectedFilePath, sizeof(selectedFilePath), "%s\\%s", currentPath, entries[currentSelection].name);
				if (result < sizeof(selectedFilePath)) {
					// Обработка ошибки
				}
                
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
        } else if (c == KEY_CTRL_F) {
            // Вызов функции создания файла
            system("cls");
            createFile(); 
        } else if (c == KEY_CTRL_C) {
            // Вызов функции копирования каталога
            system("cls");
            createDirectory();  
        } else if (c == KEY_CTRL_D) {
            // Вызов функции удаления с подтверждением
            system("cls");
            confirmDelete(entries[currentSelection].name);
        } else if (c == KEY_CTRL_R) {
            // Открытие интерфейса работы с командной строкой
            // TODO: Реализовать функциональность
            // Открытие интерфейса работы с cmd
            // TODO: Implement cmd interface
            // system("cls");
            //handleCtrlR();  // Обрабатываем Ctrl + R
            // ОТключен ведь сломан, плюс нет смысла пока.
        }

    }

    setCursorVisibility(1);  // Показываем курсор при выходе
    return NULL;  // Возвращаем NULL при выходе без выбора файла
}