#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <locale.h>
#include <stdbool.h>
#include "syntax.h"
#include "console_utils.h"

#define MAX_LINES 10000
#define MAX_LINE_LENGTH 1000
#define MIN(a, b) (((size_t)(a) < (size_t)(b)) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MAX_CLIPBOARD_SIZE 10000

#define VERSION "1.2.1"

char** lines;
int num_lines = 0;
int current_line = 0;
size_t current_col = 0;
int top_line = 0;
int screen_height;
int screen_width;
int max_line_number_length;
char clipboard[MAX_CLIPBOARD_SIZE]; // Буфер для копирования текста
int clipboard_size = 0;
int max_line_number_length = 5;

HANDLE hConsole;
COORD cursorPosition;
CONSOLE_SCREEN_BUFFER_INFO csbi;

void init_console() {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE; // Hide cursor for cleaner output
    SetConsoleCursorInfo(hConsole, &cursorInfo);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
}

void set_cursor_position(int x, int y) {
    cursorPosition.X = x;
    cursorPosition.Y = y;
    SetConsoleCursorPosition(hConsole, cursorPosition);
}

void get_console_size() {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    screen_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    screen_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1 - 1; // Reserve 1 line for status
}

void apply_syntax_highlighting(const char* line, CHAR_INFO* buffer, int row, int start_col, SyntaxRule* syntax_rules, int syntax_rules_count) {
    int line_len = strlen(line);
    int i;

    // Создаем буфер для строки с заменой табуляций
    char expanded_line[2048]; // Предполагается, что длина строки меньше 2048 символов
    int expanded_len = 0;

    // Заменяем табуляции на пробелы
    for (i = 0; i < line_len; i++) {
        if (line[i] == '\t') {
            int spaces = 4 - (expanded_len % 4); // Количество пробелов для добавления
            for (int j = 0; j < spaces; j++) {
                expanded_line[expanded_len++] = ' ';
            }
        } else {
            expanded_line[expanded_len++] = line[i];
        }
    }
    expanded_line[expanded_len] = '\0'; // Завершаем строку

    // Инициализируем буфер с цветом по умолчанию (белый)
    for (i = 0; i < expanded_len; i++) {
        buffer[row * screen_width + start_col + i].Attributes = 15; // Белый текст
    }

    // Определяем комментарии и их диапазоны
    typedef struct {
        int start;
        int end;
        bool is_multiline;
    } CommentRange;

    CommentRange comments[2048];
    int num_comments = 0;

    i = 0;
    while (i < expanded_len) {
        // Обработка однострочных комментариев
        if (expanded_line[i] == '/' && i + 1 < expanded_len && (expanded_line[i + 1] == '/' || expanded_line[i + 1] == '*')) {
            if (expanded_line[i + 1] == '/') {
                // Записываем комментарий
                comments[num_comments].start = i;
                comments[num_comments].end = expanded_len;
                comments[num_comments].is_multiline = false;
                num_comments++;
                while (i < expanded_len) {
                    buffer[row * screen_width + start_col + i].Attributes = 8; // Темно-серый для однострочных комментариев
                    i++;
                }
            } else if (expanded_line[i + 1] == '*') {
                // Записываем начало многострочного комментария
                comments[num_comments].start = i;
                comments[num_comments].is_multiline = true;
                i += 2; // Пропускаем "/*"
                while (i < expanded_len) {
                    if (expanded_line[i] == '*' && i + 1 < expanded_len && expanded_line[i + 1] == '/') {
                        // Записываем конец многострочного комментария
                        comments[num_comments].end = i + 2;
                        num_comments++;
                        i += 2; // Пропускаем "*/"
                        break;
                    }
                    buffer[row * screen_width + start_col + i].Attributes = 8; // Темно-серый для многострочных комментариев
                    i++;
                }
            }
        } else {
            i++;
        }
    }

    // Подсветка ключевых слов и функций
    for (i = 0; i < syntax_rules_count; i++) {
        SyntaxRule rule = syntax_rules[i];
        char* ptr = strstr(expanded_line, rule.keyword);

        while (ptr != NULL) {
            int index = ptr - expanded_line;
            int keyword_len = strlen(rule.keyword);

            // Проверяем, что ключевое слово не является частью другого слова
            bool is_start_valid = (index == 0 || (!isalnum(expanded_line[index - 1]) && expanded_line[index - 1] != '_'));
            bool is_end_valid = (index + keyword_len >= expanded_len || (!isalnum(expanded_line[index + keyword_len]) && expanded_line[index + keyword_len] != '_'));

            // Проверяем, что ключевое слово не находится в комментарии
            bool in_comment = false;
            for (int j = 0; j < num_comments; j++) {
                if (index >= comments[j].start && index < comments[j].end) {
                    in_comment = true;
                    break;
                }
            }

            if (!in_comment && is_start_valid && is_end_valid) {
                // Проверяем, что это функция (слово + открывающая скобка)
                if (index + keyword_len < expanded_len && expanded_line[index + keyword_len] == '(') {
                    // Подсвечиваем функцию
                    for (int j = 0; j < keyword_len; j++) {
                        buffer[row * screen_width + start_col + index + j].Attributes = COLOR_FUNCTION; // Светло-синий для функций
                    }
                } else {
                    // Подсвечиваем ключевое слово
                    for (int j = 0; j < keyword_len; j++) {
                        buffer[row * screen_width + start_col + index + j].Attributes = rule.color;
                    }
                }
            }
            ptr = strstr(ptr + keyword_len, rule.keyword); // Продолжаем поиск с нового места
        }
    }

    // Подсветка блоков if
    for (i = 0; i < expanded_len; i++) {
        if (expanded_line[i] == 'i' && i + 1 < expanded_len && expanded_line[i + 1] == 'f' && 
            (i == 0 || isspace(expanded_line[i - 1]) || expanded_line[i - 1] == '_') && 
            i + 2 < expanded_len && expanded_line[i + 2] == '(') {
            int start = i;
            i += 3; // Пропускаем "if("
            while (i < expanded_len && expanded_line[i] != ')') {
                i++;
            }
            if (i < expanded_len) {
                int end = i + 1; // Включаем закрывающую скобку
                for (int j = start; j < end; j++) {
                    buffer[row * screen_width + start_col + j].Attributes = 10; // Зеленый для блоков if
                }
            }
        }
    }

    // Подсветка чисел
    i = 0;
    while (i < expanded_len) {
        if (isdigit(expanded_line[i])) {
            int start = i;
            while (i < expanded_len && isdigit(expanded_line[i])) {
                i++;
            }
            int end = i;
            for (int j = start; j < end; j++) {
                buffer[row * screen_width + start_col + j].Attributes = 11; // Светло-голубой для чисел
            }
        } else {
            i++;
        }
    }

    // Подсветка строк в двойных кавычках
    i = 0;
    while (i < expanded_len) {
        if (expanded_line[i] == '"') {
            int start = i;
            i++;
            while (i < expanded_len && expanded_line[i] != '"') {
                i++;
            }
            if (i < expanded_len) {
                int end = i;
                for (int j = start; j <= end; j++) {
                    buffer[row * screen_width + start_col + j].Attributes = 12; // Красный для строк
                }
            }
        }
        i++;
    }

    // Подсветка строк в одинарных кавычках
    i = 0;
    while (i < expanded_len) {
        if (expanded_line[i] == '\'') {
            int start = i;
            i++;
            while (i < expanded_len && expanded_line[i] != '\'') {
                i++;
            }
            if (i < expanded_len) {
                int end = i;
                for (int j = start; j <= end; j++) {
                    buffer[row * screen_width + start_col + j].Attributes = 13; // Пурпурный для строк в одинарных кавычках
                }
            }
        }
        i++;
    }

    // Копируем содержимое расширенной строки в буфер
    for (i = 0; i < expanded_len; i++) {
        buffer[row * screen_width + start_col + i].Char.AsciiChar = expanded_line[i];
    }
}

void fill_console_buffer() {
    COORD bufferSize = {screen_width, num_lines + 1}; // +1 for status line
    CHAR_INFO* buffer = (CHAR_INFO*)malloc(bufferSize.X * bufferSize.Y * sizeof(CHAR_INFO));
    
    // Initialize buffer
    for (int i = 0; i < bufferSize.X * bufferSize.Y; i++) {
        buffer[i].Char.AsciiChar = ' ';
        buffer[i].Attributes = 15; // White text
    }

    // Fill buffer with lines and line numbers (for display only, not for copying)
    for (int i = 0; i < num_lines; i++) {
        int line_number = i + 1;
        char line_num_str[10];
        snprintf(line_num_str, sizeof(line_num_str), "%d", line_number);

        // Calculate padding for line numbers
        int padding = max_line_number_length - strlen(line_num_str);
        int start_col = 0;

        // Fill line number with padding (for display only)
        for (int j = 0; j < padding; j++) {
            buffer[i * screen_width + start_col].Char.AsciiChar = ' ';
            buffer[i * screen_width + start_col].Attributes = 15; // White text
            start_col++;
        }

        for (size_t j = 0; j < strlen(line_num_str); j++) {
            buffer[i * screen_width + start_col + j].Char.AsciiChar = line_num_str[j];
            buffer[i * screen_width + start_col + j].Attributes = 15; // White text
        }

        start_col += strlen(line_num_str) + 3; // Space for number and additional padding

        // Apply syntax highlighting to line text (excluding line numbers)
        apply_syntax_highlighting(lines[i], buffer, i, start_col, syntax_rules, syntax_rules_count);

        // Highlight current line
        if (i == current_line) {
            for (int j = 0; j < screen_width; j++) {
                buffer[i * screen_width + j].Attributes = 240; // Background highlight
            }
        }
    }

    // Write buffer to console
    SMALL_RECT writeRegion = {0, 0, screen_width - 1, num_lines};
    COORD bufferCoord = {0, 0};
    WriteConsoleOutput(hConsole, buffer, bufferSize, bufferCoord, &writeRegion);

    // Draw the cursor with white character and black background
    int cursorX = current_col + max_line_number_length + 3;
    int cursorY = current_line - top_line;
    if (cursorX < screen_width && cursorY < num_lines) {
        int index = cursorY * screen_width + cursorX;

        // Save the original character at the cursor position
        CHAR_INFO cursor_char = buffer[index];
        
        // Set the character color to white and background to black for the cursor
        buffer[index].Attributes = 0x0F; // White on black

        // Write the buffer to the console again to draw the cursor
        WriteConsoleOutput(hConsole, buffer, bufferSize, bufferCoord, &writeRegion);
        
        // Restore the original character at the cursor position
        buffer[index] = cursor_char;
    }

    // Clean up
    free(buffer);
}

// Функция для копирования строки без номера
void copy_line_without_number() {
    // Убедитесь, что `current_line` не превышает число строк
    if (current_line >= num_lines) {
        clipboard[0] = '\0'; // Если строка недоступна, очистите буфер
        clipboard_size = 0;
        return;
    }

    // Расчет длины строки без учета номера строки
    int line_length = strlen(lines[current_line]);
    char number_str[10];
    int number_length = sprintf(number_str, "%d", current_line + 1); // Длина номера строки
    int padding = 3; // Дополнительный отступ после номера

    // Если текущая строка пустая
    if (line_length <= number_length + padding) {
        clipboard[0] = '\0'; // Очистите буфер, если в строке нет текста
        clipboard_size = 0;
        return;
    }

    // Копирование текста строки без номера и отступа
    int text_start_index = number_length + padding;
    int text_length = line_length - text_start_index;

    if (text_length > 0) {
        strncpy(clipboard, &lines[current_line][text_start_index], MIN(text_length, MAX_CLIPBOARD_SIZE));
        clipboard_size = MIN(text_length, MAX_CLIPBOARD_SIZE);
        clipboard[clipboard_size] = '\0'; // Завершающий нулевой символ
    } else {
        clipboard[0] = '\0'; // Если нет текста для копирования
        clipboard_size = 0;
    }
}

void update_cursor_position() {
    // Update console cursor position (not visible in this case, just for consistency)
    set_cursor_position(current_col + max_line_number_length + 3, current_line - top_line);
}

void insert_char(char c) {
    size_t line_length = strlen(lines[current_line]);
    if (current_col < MAX_LINE_LENGTH - 1) {
        if (current_col >= line_length) {
            lines[current_line][current_col] = c;
        } else {
            memmove(&lines[current_line][current_col + 1], &lines[current_line][current_col], line_length - current_col + 1);
            lines[current_line][current_col] = c;
        }
        current_col++;
        fill_console_buffer(); // Redraw entire buffer
        update_cursor_position();
    }
}

void insert_tab() {
    const char TAB_SPACES[4] = "    ";
    if (strlen(lines[current_line]) < MAX_LINE_LENGTH - 4) {
        memmove(&lines[current_line][current_col + 4], &lines[current_line][current_col], MAX_LINE_LENGTH - current_col - 4);
        memcpy(&lines[current_line][current_col], TAB_SPACES, 4);
        current_col += 4;
        fill_console_buffer(); // Redraw entire buffer
        update_cursor_position();
    }
}

void delete_char() {
    if (current_col > 0) {
        size_t line_length = strlen(lines[current_line]);
        memmove(&lines[current_line][current_col - 1], &lines[current_line][current_col], line_length - current_col + 1);
        current_col--;
        fill_console_buffer(); // Redraw entire buffer
        update_cursor_position();
    } else if (current_line > 0) {
        size_t prev_len = strlen(lines[current_line - 1]);
        strcat(lines[current_line - 1], lines[current_line]);
        free(lines[current_line]);
        memmove(&lines[current_line], &lines[current_line + 1], (num_lines - current_line - 1) * sizeof(char*));
        num_lines--;
        current_line--;
        current_col = prev_len;
        fill_console_buffer(); // Redraw entire buffer
        update_cursor_position();
    }
}

void delete_after_cursor() {
    size_t line_length = strlen(lines[current_line]);

    if (current_col < line_length) {
        // Удаление символа после курсора в текущей строке
        memmove(&lines[current_line][current_col], &lines[current_line][current_col + 1], line_length - current_col);
        lines[current_line][line_length - 1] = '\0'; // Обновляем конец строки
    } else if (current_line < num_lines - 1) {
        // Объединение текущей строки с следующей строкой
        strcat(lines[current_line], lines[current_line + 1]);

        // Удаление строки
        free(lines[current_line + 1]);
        memmove(&lines[current_line + 1], &lines[current_line + 2], (num_lines - current_line - 2) * sizeof(char*));
        num_lines--;
    }
    fill_console_buffer(); // Перерисовать весь буфер
    update_cursor_position();
}

void new_line() {
    if (num_lines < MAX_LINES - 1) {
        num_lines++;
        memmove(&lines[current_line + 2], &lines[current_line + 1], (num_lines - current_line - 1) * sizeof(char*));
        lines[current_line + 1] = malloc(MAX_LINE_LENGTH);
        strcpy(lines[current_line + 1], &lines[current_line][current_col]);
        lines[current_line][current_col] = '\0';
        current_line++;
        current_col = 0;
        fill_console_buffer(); // Redraw entire buffer
        update_cursor_position();
    }
}

void save_file(const char* filename) {
    FILE* file = fopen(filename, "w");
    clearConsole();  // Очищаем экран перед выводом текста
    hideCursor();    // Прячем курсор
    if (file) {
        for (int i = 0; i < num_lines; i++) {
            fprintf(file, "%s\n", lines[i]);
        }
        fclose(file);
        set_cursor_position(0, screen_height);

        wchar_t text[] = L"File saved successfully.";
        centerText(text, TXT_GREEN, BACK_BLACK);  // Выводим текст в центр консоли
    } else {
        set_cursor_position(0, screen_height);
        wchar_t text[] = L"Error saving file.";
        centerText(text, TXT_RED, BACK_BLACK);  // Выводим текст в центр консоли
    }
    Sleep(500);

    // Очистка консоли и перерисовка интерфейса после паузы
    clearConsole();
    fill_console_buffer();
    
    showCursor();    // Показываем курсор перед завершением программы
}

void clear_console() {
    COORD coordScreen = {0, 0};
    DWORD cCharsWritten;
    DWORD dwConSize;

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(hConsole, ' ', dwConSize, coordScreen, &cCharsWritten);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
    SetConsoleCursorPosition(hConsole, coordScreen);
}

void initialize_lines() {
    lines = malloc(MAX_LINES * sizeof(char*));
    for (int i = 0; i < MAX_LINES; i++) {
        lines[i] = malloc(MAX_LINE_LENGTH);
        lines[i][0] = '\0'; // Инициализация пустыми строками
    }
    num_lines = 0; // Устанавливаем начальное количество строк
}

void cleanup_lines() {
    for (int i = 0; i < MAX_LINES; i++) {
        free(lines[i]); // Освобождение памяти для каждой строки
    }
    free(lines); // Освобождение массива строк
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    enableVirtualTerminalProcessing(hConsole);

    // Обработка флагов версии
    if (argc > 1) {
        if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--v") == 0 ||
            strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-version") == 0) {
            printf(TXT_ONLY(TXT_GREEN) "-> VERSION:\n" CMD_RESET_COLOR);
            printf(TXT_ONLY(TXT_GREEN) "    | CLITE VERSION: %s" CMD_RESET_COLOR, VERSION);
            return 0;
        } else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--h") == 0 ||
            strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-help") == 0) {
            printf(TXT_ONLY(TXT_MAGENTA));
            printf("-> HELP:\n");
            printf("    | DEVELOPER: TAILOGS\n");
            printf("    | VERSION: %s\n", VERSION);
            printf("    | COMMANDS:\n");
            printf("        -h, --h, -help, --help\n");
            printf("            | PRINT HELP\n");
            printf("        -v, --v, -version, --version\n");
            printf("            | PRINT VERSION");
            printf(CMD_RESET_COLOR);
            return 0;
        } 
    }

    if (argc != 2) {
        char usage[100]; 
        sprintf(usage, "     | Usage: %s <filename>", argv[0]);
        print_colored_symbol_and_text("|", TXT_BLACK, BACK_WHITE, " -> ERROR:", TXT_LIGHT_WHITE, BACK_LIGHT_RED);
        printf("\n");
        print_colored_symbol_and_text(".", TXT_BLACK, BACK_WHITE, usage, TXT_LIGHT_WHITE, BACK_LIGHT_RED);
        return 1;
    }

    init_console();
    get_console_size();
    clear_console(); 

    // Определение расширения файла
    char* filename = argv[1];
    char* extension = strrchr(filename, '.');

    if (extension) {
        if (strcmp(extension, ".c") == 0 || strcmp(extension, ".cpp") == 0) {
            add_syntax_rules_c_and_cpp();
        } else if (strcmp(extension, ".py") == 0) {
            add_syntax_rules_python();
        } else if (strcmp(extension, ".rb") == 0) {
            add_syntax_rules_ruby();
        } else if (strcmp(extension, ".js") == 0) {
            add_syntax_rules_javascript();
        } else if (strcmp(extension, ".java") == 0) {
            add_syntax_rules_java();
        } else if (strcmp(extension, ".php") == 0) {
            add_syntax_rules_php();
        } else if (strcmp(extension, ".kt") == 0) {
            add_syntax_rules_kotlin();
        } else if (strcmp(extension, ".rs") == 0) {
            add_syntax_rules_rust();
        } else if (strcmp(extension, ".swift") == 0) {
            add_syntax_rules_swift();
        } else if (strcmp(extension, ".pl") == 0) {
            add_syntax_rules_perl();
        } else if (strcmp(extension, ".hs") == 0) {
            add_syntax_rules_haskell();
        } else if (strcmp(extension, ".go") == 0) {
            add_syntax_rules_go();
        } else if (strcmp(extension, ".ts") == 0) {
            add_syntax_rules_typescript();
        } else if (strcmp(extension, ".scala") == 0) {
            add_syntax_rules_scala();
        } else if (strcmp(extension, ".lua") == 0) {
            add_syntax_rules_lua();
        } else if (strcmp(extension, ".dart") == 0) {
            add_syntax_rules_dart();
        } else if (strcmp(extension, ".ex") == 0) {
            add_syntax_rules_elixir();
        } else if (strcmp(extension, ".r") == 0) {
            add_syntax_rules_r();
        } else if (strcmp(extension, ".m") == 0 || strcmp(extension, ".mm") == 0) {
            add_syntax_rules_objective_c();
        } else if (strcmp(extension, ".mat") == 0) {
            add_syntax_rules_matlab();
        } else if (strcmp(extension, ".sh") == 0) {
            add_syntax_rules_shell();
        } else if (strcmp(extension, ".groovy") == 0) {
            add_syntax_rules_groovy();
        } else if (strcmp(extension, ".bat") == 0) {
            add_syntax_rules_batch();
        } else if (strcmp(extension, ".ps1") == 0) {
            add_syntax_rules_powershell();
        } else if (strcmp(extension, ".fs") == 0) {
            add_syntax_rules_fsharp();
        } else if (strcmp(extension, ".cs") == 0) {
            add_syntax_rules_csharp();
        } else if (strcmp(extension, ".html") == 0 || strcmp(extension, ".htm") == 0) {
            add_syntax_rules_html();
        } else if (strcmp(extension, ".css") == 0) {
            add_syntax_rules_css();
        }
    }
    
    initialize_lines(); // Инициализация строк

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        // Файл не существует, создаём новый файл с одной пустой строкой
        file = fopen(argv[1], "w");
        if (file) {
            fprintf(file, "\n"); // Добавляем пустую строку в файл
            fclose(file);
        } else {
            perror("Error creating file");
            cleanup_lines(); // Освобождение ресурсов перед выходом
            return 1;
        }

        // Переоткрываем файл для чтения
        file = fopen(argv[1], "r");
        if (!file) {
            perror("Error reopening file");
            cleanup_lines(); // Освобождение ресурсов перед выходом
            return 1;
        }
    }

    // Чтение содержимого файла
    while (fgets(lines[num_lines], MAX_LINE_LENGTH, file) && num_lines < MAX_LINES) {
        lines[num_lines][strcspn(lines[num_lines], "\n")] = 0;
        num_lines++;
    }
    fclose(file);

    max_line_number_length = max_line_number_length;
    fill_console_buffer(); // Изначальная отрисовка

    while (true) {
        int c = _getch();
        if (c == 0 || c == 224) { // Обработка стрелок и других расширенных клавиш
            c = _getch();
            switch (c) {
                case 72: // Вверх
                    if (current_line > top_line) {
                        current_line--;
                        current_col = MIN(current_col, strlen(lines[current_line]));
                        fill_console_buffer();
                        update_cursor_position();
                    }
                    break;
                case 80: // Вниз
                    if (current_line < num_lines - 1) {
                        current_line++;
                        current_col = MIN(current_col, strlen(lines[current_line]));
                        fill_console_buffer();
                        update_cursor_position();
                    }
                    break;
                case 75: // Влево
                    if (current_col > 0) {
                        current_col--;
                        fill_console_buffer();
                        update_cursor_position();
                    } else if (current_line > 0) {
                        current_line--;
                        current_col = strlen(lines[current_line]);
                        fill_console_buffer();
                        update_cursor_position();
                    }
                    break;
                case 77: // Вправо
                    if (current_col < strlen(lines[current_line])) {
                        current_col++;
                        fill_console_buffer();
                        update_cursor_position();
                    } else if (current_line < num_lines - 1) {
                        current_line++;
                        current_col = 0;
                        fill_console_buffer();
                        update_cursor_position();
                    }
                    break;
                case 83: // Удаление
                    delete_after_cursor();
                    break;
            }
        } else if (c == 13) { // Enter
            new_line();
        } else if (c == 8) { // Backspace
            delete_char();
        } else if (c == 9) { // Tab
            insert_tab();
        } else if (c == 19) { // Ctrl+S
            save_file(argv[1]);
        } else if (c == 17) { // Ctrl+Q
            break;
        } else if (c == 3) { // Ctrl+C
            //copy_line_without_number();
            //printf("Line copied to clipboard.\n");
        } else if (c >= 32 && c <= 126) { // Обычные печатные символы
            insert_char(c);
        }
    }

    // Восстановление видимости курсора перед выходом
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    clear_console(); 

    cleanup_lines(); // Освобождение ресурсов перед выходом

    return 0;
}
