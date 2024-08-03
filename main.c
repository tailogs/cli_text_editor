#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>

#define MAX_LINES 10000
#define MAX_LINE_LENGTH 1000
#define MAX_SYNTAX_RULES 100
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MAX_CLIPBOARD_SIZE 10000

char** lines;
int num_lines = 0;
int current_line = 0;
int current_col = 0;
int top_line = 0;
int screen_height;
int screen_width;
int max_line_number_length;
char clipboard[MAX_CLIPBOARD_SIZE]; // Буфер для копирования текста
int clipboard_size = 0;

HANDLE hConsole;
COORD cursorPosition;
CONSOLE_SCREEN_BUFFER_INFO csbi;

// Syntax highlighting rules
typedef struct {
    char* keyword;
    int color;
} SyntaxRule;

SyntaxRule syntax_rules[MAX_SYNTAX_RULES];
int syntax_rules_count = 0;

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

void set_console_color(int color) {
    SetConsoleTextAttribute(hConsole, color);
}

void get_console_size() {
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    screen_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    screen_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1 - 1; // Reserve 1 line for status
}

int calculate_max_line_number_length() {
    return 5; // Fixed length for up to 99999 lines
}

void apply_syntax_highlighting(const char* line, CHAR_INFO* buffer, int row, int start_col) {
    int col = start_col;
    int line_len = strlen(line);
    int i;

    // Initialize buffer with default color (white)
    for (i = 0; i < line_len; i++) {
        buffer[row * screen_width + start_col + i].Attributes = 15; // White text
    }

    // Apply syntax highlighting rules for comments
    i = 0;
    while (i < line_len) {
        if (line[i] == '/' && i + 1 < line_len && line[i + 1] == '/') {
            // Single-line comment
            int start = i;
            while (i < line_len) {
                buffer[row * screen_width + start_col + i].Attributes = 8; // Dark gray for single-line comments
                i++;
            }
        } else if (line[i] == '/' && i + 1 < line_len && line[i + 1] == '*') {
            // Multi-line comment
            int start = i;
            i += 2; // Skip "/*"
            while (i < line_len) {
                if (line[i] == '*' && i + 1 < line_len && line[i + 1] == '/') {
                    i += 2; // Skip "*/"
                    break;
                }
                buffer[row * screen_width + start_col + i].Attributes = 8; // Dark gray for multi-line comments
                i++;
            }
            if (i == line_len) {
                // Handle case where comment goes to the end of the line
                buffer[row * screen_width + start_col + line_len - 1].Attributes = 8;
            }
        } else {
            i++;
        }
    }

    // Apply syntax highlighting rules for other elements
    for (i = 0; i < syntax_rules_count; i++) {
        SyntaxRule rule = syntax_rules[i];
        char* ptr = strstr(line, rule.keyword);
        while (ptr != NULL) {
            int index = ptr - line;
            int keyword_len = strlen(rule.keyword);
            for (int j = 0; j < keyword_len; j++) {
                buffer[row * screen_width + start_col + index + j].Attributes = rule.color;
            }
            ptr = strstr(ptr + keyword_len, rule.keyword);
        }
    }

    // Highlight numbers
    i = 0;
    while (i < line_len) {
        if (isdigit(line[i])) {
            int start = i;
            while (i < line_len && isdigit(line[i])) {
                i++;
            }
            int end = i;
            for (int j = start; j < end; j++) {
                buffer[row * screen_width + start_col + j].Attributes = 11; // Light blue for numbers
            }
        } else {
            i++;
        }
    }

    // Highlight strings between double quotes
    i = 0;
    while (i < line_len) {
        if (line[i] == '"') {
            int start = i;
            i++;
            while (i < line_len && line[i] != '"') {
                i++;
            }
            if (i < line_len) {
                int end = i;
                for (int j = start; j <= end; j++) {
                    buffer[row * screen_width + start_col + j].Attributes = 12; // Red for strings
                }
                i++; // Move past the closing quote
            }
        } else {
            i++;
        }
    }

    // Highlight strings between single quotes
    i = 0;
    while (i < line_len) {
        if (line[i] == '\'') {
            int start = i;
            i++;
            while (i < line_len && line[i] != '\'') {
                i++;
            }
            if (i < line_len) {
                int end = i;
                for (int j = start; j <= end; j++) {
                    buffer[row * screen_width + start_col + j].Attributes = 13; // Magenta for single-quoted strings
                }
                i++; // Move past the closing quote
            }
        } else {
            i++;
        }
    }

    // Copy the line content into the buffer
    for (i = 0; i < line_len; i++) {
        buffer[row * screen_width + start_col + i].Char.AsciiChar = line[i];
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

        for (int j = 0; j < strlen(line_num_str); j++) {
            buffer[i * screen_width + start_col + j].Char.AsciiChar = line_num_str[j];
            buffer[i * screen_width + start_col + j].Attributes = 15; // White text
        }

        start_col += strlen(line_num_str) + 3; // Space for number and additional padding

        // Apply syntax highlighting to line text (excluding line numbers)
        apply_syntax_highlighting(lines[i], buffer, i, start_col);

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

    // Draw the cursor with red underscore
    int cursorX = current_col + max_line_number_length + 3;
    int cursorY = current_line - top_line;
    if (cursorX < screen_width && cursorY < num_lines) {
        int index = cursorY * screen_width + cursorX;
        buffer[index].Char.AsciiChar = '_'; // Draw a red underscore for cursor
        buffer[index].Attributes = 12; // Red color
        WriteConsoleOutput(hConsole, buffer, bufferSize, bufferCoord, &writeRegion);
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
    int number_length = snprintf(NULL, 0, "%d", current_line + 1); // Длина номера строки
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
    if (strlen(lines[current_line]) < MAX_LINE_LENGTH - 1) {
        memmove(&lines[current_line][current_col + 1], &lines[current_line][current_col], MAX_LINE_LENGTH - current_col - 1);
        lines[current_line][current_col] = c;
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
        memmove(&lines[current_line][current_col - 1], &lines[current_line][current_col], strlen(lines[current_line]) - current_col + 1);
        current_col--;
        fill_console_buffer(); // Redraw entire buffer
        update_cursor_position();
    } else if (current_line > 0) {
        int prev_len = strlen(lines[current_line - 1]);
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
    int line_length = strlen(lines[current_line]);

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
    if (file) {
        for (int i = 0; i < num_lines; i++) {
            fprintf(file, "%s\n", lines[i]);
        }
        fclose(file);
        set_cursor_position(0, screen_height);
        printf("File saved successfully.");
    } else {
        set_cursor_position(0, screen_height);
        printf("Error saving file.");
    }
}

void clear_console() {
    COORD coordScreen = {0, 0};
    DWORD cCharsWritten;
    DWORD dwConSize;

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(hConsole, (TCHAR) ' ', dwConSize, coordScreen, &cCharsWritten);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
    SetConsoleCursorPosition(hConsole, coordScreen);
}

void add_syntax_rule(const char* keyword, int color) {
    if (syntax_rules_count < MAX_SYNTAX_RULES) {
        syntax_rules[syntax_rules_count].keyword = strdup(keyword);
        syntax_rules[syntax_rules_count].color = color;
        syntax_rules_count++;
    }
}

void add_syntax_rules() {
    // Цвета для подсветки
    int color_keyword = 10;  // Зеленый
    int color_function = 14; // Светло-синий
    int color_string = 12;   // Красный
    int color_number = 11;   // Светло-голубой
    int color_comment = 8;   // Темно-серый
    int color_header = 9;    // Светло-зеленый для заголовочных файлов
    int color_include = 13;  // Цвет для #include

    // Ключевые слова и их цвет для подсветки
    const char* keywords[] = {
        "int", "return", "for", "while", "if", "else", "class",
        "public", "private", "protected", "struct", "namespace",
        "template", "void", "static", "auto", "const", "sizeof",
        "true", "false", "NULL", "break", "continue", "switch",
        "case", "default", "typedef", "volatile", "extern",
        "register", "union", "goto", "long", "short", "char",
        "float", "double", "unsigned", "signed"
    };

    // Функции и их цвет для подсветки
    const char* functions[] = {
        // Функции из <stdio.h>
        "printf", "scanf", "fprintf", "fscanf", "sprintf", "sscanf",
        "fopen", "fclose", "fgets", "fputs", "fread", "fwrite",
        "fflush", "fseek", "ftell", "rewind",

        // Функции из <stdlib.h>
        "malloc", "calloc", "realloc", "free", "exit", "atexit", "abs",
        "rand", "srand",

        // Функции из <string.h>
        "strlen", "strcpy", "strncpy", "strcat", "strncat", "strcmp",
        "strncmp", "strchr", "strrchr", "strstr", "memcpy", "memmove",
        "memcmp", "memset",

        // Функции из <ctype.h>
        "isdigit", "isalpha", "islower", "isupper", "tolower", "toupper",

        // Функции из <math.h>
        "sin", "cos", "tan", "sqrt", "pow", "log", "exp",

        // Функции из <iostream> (C++)
        "std::cout", "cout", "std::cin", "cin", "std::cerr", "cerr", "std::clog", "clog",

        // Функции из <fstream> (C++)
        "std::ifstream", "ifstream", "std::ofstream", "ofstream", "std::fstream", "fstream",

        // Функции из <sstream> (C++)
        "std::stringstream", "stringstream",

        // Функции из <vector> (C++)
        "std::vector", "vector",

        // Функции из <map> (C++)
        "std::map", "map",

        // Функции из <set> (C++)
        "std::set", "set",

        // Функции из <list> (C++)
        "std::list", "list",

        // Функции из <algorithm> (C++)
        "std::sort", "sort", "std::find", "find",

        // Функции из <thread> (C++)
        "std::thread", "thread",

        // Функции из <mutex> (C++)
        "std::mutex", "mutex",

        // Дополнительные функции
        // "new_function1", "new_function2" // Добавьте свои функции сюда
    };

    // Заголовочные файлы и их цвет для подсветки
    const char* headers[] = {
        "<stdio.h>", "<stdlib.h>", "<string.h>", "<conio.h>", "<windows.h>",
        "<iostream>", "<fstream>", "<sstream>", "<vector>", "<map>",
        "<set>", "<list>", "<algorithm>", "<cassert>", "<cctype>",
        "<cfloat>", "<climits>", "<cstdarg>", "<cstddef>", "<cstdio>",
        "<cstdlib>", "<cstring>", "<cwchar>", "<cwctype>", "<locale>",
        "<new>", "<typeinfo>", "<utility>", "<bitset>", "<complex>",
        "<exception>", "<functional>", "<future>", "<initializer_list>",
        "<iterator>", "<mutex>", "<random>", "<ratio>", "<regex>",
        "<system_error>", "<thread>", "<tuple>", "<typeindex>", "<utility>"
    };

    // Добавление ключевых слов
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i) {
        add_syntax_rule(keywords[i], color_keyword);
    }

    // Добавление функций
    for (int i = 0; i < sizeof(functions) / sizeof(functions[0]); ++i) {
        add_syntax_rule(functions[i], color_function);
    }

    // Добавление заголовочных файлов
    for (int i = 0; i < sizeof(headers) / sizeof(headers[0]); ++i) {
        add_syntax_rule(headers[i], color_header);
    }

    // Добавление строки включения заголовочных файлов
    add_syntax_rule("#include", color_include);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    init_console();
    get_console_size();
    clear_console(); // Clear console on startup

    // Define syntax rules
    add_syntax_rules(); // Добавить синтаксические правила

    lines = malloc(MAX_LINES * sizeof(char*));
    for (int i = 0; i < MAX_LINES; i++) {
        lines[i] = malloc(MAX_LINE_LENGTH);
        lines[i][0] = '\0';
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        // File does not exist, create a new file with a single empty line
        file = fopen(argv[1], "w");
        if (file) {
            fprintf(file, "\n"); // Add an empty line to the file
            fclose(file);
        } else {
            perror("Error creating file");
            return 1;
        }

        // Re-open the file for reading
        file = fopen(argv[1], "r");
        if (!file) {
            perror("Error reopening file");
            return 1;
        }
    }

    // Read the file contents
    while (fgets(lines[num_lines], MAX_LINE_LENGTH, file) && num_lines < MAX_LINES) {
        lines[num_lines][strcspn(lines[num_lines], "\n")] = 0;
        num_lines++;
    }
    fclose(file);

    max_line_number_length = calculate_max_line_number_length();
    fill_console_buffer(); // Initial draw

    while (1) {
		int c = _getch();
		if (c == 0 || c == 224) { // Handle arrow keys and other extended keys
			c = _getch();
			switch (c) {
				case 72: // Up
					if (current_line > top_line) {
						current_line--;
						current_col = MIN(current_col, strlen(lines[current_line]));
						fill_console_buffer(); // Redraw entire buffer
						update_cursor_position();
					}
					break;
				case 80: // Down
					if (current_line < num_lines - 1) {
						current_line++;
						current_col = MIN(current_col, strlen(lines[current_line]));
						fill_console_buffer(); // Redraw entire buffer
						update_cursor_position();
					}
					break;
				case 75: // Left
					if (current_col > 0) {
						current_col--;
						fill_console_buffer(); // Redraw entire buffer
						update_cursor_position();
					} else if (current_line > 0) {
						current_line--;
						current_col = strlen(lines[current_line]);
						fill_console_buffer(); // Redraw entire buffer
						update_cursor_position();
					}
					break;
				case 77: // Right
					if (current_col < strlen(lines[current_line])) {
						current_col++;
						fill_console_buffer(); // Redraw entire buffer
						update_cursor_position();
					} else if (current_line < num_lines - 1) {
						current_line++;
						current_col = 0;
						fill_console_buffer(); // Redraw entire buffer
						update_cursor_position();
					}
					break;
				case 83: // Delete
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
			copy_line_without_number();
			printf("Line copied to clipboard.\n");
		} else if (c >= 32 && c <= 126) { // Regular printable characters
			insert_char(c);
		}
	}

    clear_console(); // Clear console before exit

    // Free resources
    for (int i = 0; i < MAX_LINES; i++) {
        free(lines[i]);
    }
    free(lines);
    for (int i = 0; i < syntax_rules_count; i++) {
        free(syntax_rules[i].keyword);
    }

    return 0;
}
