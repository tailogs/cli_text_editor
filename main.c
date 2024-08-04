#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include "syntax.h"

#define MAX_LINES 10000
#define MAX_LINE_LENGTH 1000
#define MIN(a, b) (((size_t)(a) < (size_t)(b)) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MAX_CLIPBOARD_SIZE 10000

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
    int line_len = strlen(line);
    int i;

    // Create a buffer to store the line with tab substitutions
    char expanded_line[2048]; // Assumes line length is less than 2048 characters
    int expanded_len = 0;

    // Expand tabs to spaces
    for (i = 0; i < line_len; i++) {
        if (line[i] == '\t') {
            int spaces = 4 - (expanded_len % 4); // Number of spaces to add
            for (int j = 0; j < spaces; j++) {
                expanded_line[expanded_len++] = ' ';
            }
        } else {
            expanded_line[expanded_len++] = line[i];
        }
    }
    expanded_line[expanded_len] = '\0'; // Null-terminate the expanded line

    // Initialize buffer with default color (white)
    for (i = 0; i < expanded_len; i++) {
        buffer[row * screen_width + start_col + i].Attributes = 15; // White text
    }

    // Apply syntax highlighting rules for comments
    i = 0;
    while (i < expanded_len) {
        // Single-line comments (C, C++, Java, JavaScript, Python, Ruby, Perl, Haskell)
        if (expanded_line[i] == '/' && i + 1 < expanded_len && (expanded_line[i + 1] == '/' || expanded_line[i + 1] == '*')) {
            // C, C++, Java, JavaScript (// and /* ... */)
            if (expanded_line[i + 1] == '/') {
                while (i < expanded_len) {
                    buffer[row * screen_width + start_col + i].Attributes = 8; // Dark gray for single-line comments
                    i++;
                }
            } else if (expanded_line[i + 1] == '*') {
                i += 2; // Skip "/*"
                while (i < expanded_len) {
                    if (expanded_line[i] == '*' && i + 1 < expanded_len && expanded_line[i + 1] == '/') {
                        i += 2; // Skip "*/"
                        break;
                    }
                    buffer[row * screen_width + start_col + i].Attributes = 8; // Dark gray for multi-line comments
                    i++;
                }
                if (i == expanded_len) {
                    // Handle case where comment goes to the end of the line
                    buffer[row * screen_width + start_col + expanded_len - 1].Attributes = 8;
                }
            }
        } else if (expanded_line[i] == '#' && (i == 0 || expanded_line[i - 1] == ' ')) {
            // Single-line comments (Python, Ruby, Perl)
            while (i < expanded_len) {
                buffer[row * screen_width + start_col + i].Attributes = 8; // Dark gray for single-line comments
                i++;
            }
        } else if (expanded_line[i] == '<' && i + 3 < expanded_len && expanded_line[i + 1] == '!' && expanded_line[i + 2] == '-' && expanded_line[i + 3] == '-') {
            // HTML/JavaScript/Java multi-line comment
            i += 4; // Skip "<!--"
            while (i < expanded_len) {
                if (expanded_line[i] == '-' && i + 2 < expanded_len && expanded_line[i + 1] == '-' && expanded_line[i + 2] == '>') {
                    i += 3; // Skip "-->"
                    break;
                }
                buffer[row * screen_width + start_col + i].Attributes = 8; // Dark gray for multi-line comments
                i++;
            }
            if (i == expanded_len) {
                // Handle case where comment goes to the end of the line
                buffer[row * screen_width + start_col + expanded_len - 1].Attributes = 8;
            }
        } else if (expanded_line[i] == '-' && i + 1 < expanded_len && expanded_line[i + 1] == '-') {
            // Haskell single-line comment
            while (i < expanded_len) {
                buffer[row * screen_width + start_col + i].Attributes = 8; // Dark gray for single-line comments
                i++;
            }
        } else {
            i++;
        }
    }

    // Apply syntax highlighting rules for other elements
    for (i = 0; i < syntax_rules_count; i++) {
        SyntaxRule rule = syntax_rules[i];
        char* ptr = strstr(expanded_line, rule.keyword);
        while (ptr != NULL) {
            int index = ptr - expanded_line;
            int keyword_len = strlen(rule.keyword);
            for (int j = 0; j < keyword_len; j++) {
                buffer[row * screen_width + start_col + index + j].Attributes = rule.color;
            }
            ptr = strstr(ptr + keyword_len, rule.keyword);
        }
    }

    // Highlight numbers
    i = 0;
    while (i < expanded_len) {
        if (isdigit(expanded_line[i])) {
            int start = i;
            while (i < expanded_len && isdigit(expanded_line[i])) {
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
                    buffer[row * screen_width + start_col + j].Attributes = 13; // Magenta for single-quoted strings
                }
                i++; // Move past the closing quote
            }
        } else {
            i++;
        }
    }

    // Copy the expanded line content into the buffer
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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    init_console();
    get_console_size();
    clear_console(); // Clear console on startup

	// Определение расширения файла
	char* filename = argv[1];
	char* extension = strrchr(filename, '.');

	if (extension) {
		if (strcmp(extension, ".c") == 0 || strcmp(extension, ".cpp") == 0) {
			add_syntax_rules_c_and_cpp(); // Добавить правила для C/C++
		} else if (strcmp(extension, ".py") == 0) {
			add_syntax_rules_python(); // Добавить правила для Python
		} else if (strcmp(extension, ".rb") == 0) {
			add_syntax_rules_ruby(); // Добавить правила для Ruby
		} else if (strcmp(extension, ".js") == 0) {
			add_syntax_rules_javascript(); // Добавить правила для JavaScript
		} else if (strcmp(extension, ".java") == 0) {
			add_syntax_rules_java(); // Добавить правила для Java
		} else if (strcmp(extension, ".php") == 0) {
			add_syntax_rules_php(); // Добавить правила для PHP
		} else if (strcmp(extension, ".kt") == 0) {
			add_syntax_rules_kotlin(); // Добавить правила для Kotlin
		} else if (strcmp(extension, ".rs") == 0) {
			add_syntax_rules_rust(); // Добавить правила для Rust
		} else if (strcmp(extension, ".swift") == 0) {
			add_syntax_rules_swift(); // Добавить правила для Swift
		} else if (strcmp(extension, ".pl") == 0) {
			add_syntax_rules_perl(); // Добавить правила для Perl
		} else if (strcmp(extension, ".hs") == 0) {
			add_syntax_rules_haskell(); // Добавить правила для Haskell
		} else if (strcmp(extension, ".go") == 0) {
			add_syntax_rules_go(); // Добавить правила для Go
		} else if (strcmp(extension, ".ts") == 0) {
			add_syntax_rules_typescript(); // Добавить правила для TypeScript
		} else if (strcmp(extension, ".scala") == 0) {
			add_syntax_rules_scala(); // Добавить правила для Scala
		} else if (strcmp(extension, ".lua") == 0) {
			add_syntax_rules_lua(); // Добавить правила для Lua
		} else if (strcmp(extension, ".dart") == 0) {
			add_syntax_rules_dart(); // Добавить правила для Dart
		} else if (strcmp(extension, ".ex") == 0) {
			add_syntax_rules_elixir(); // Добавить правила для Elixir
		} else if (strcmp(extension, ".r") == 0) {
			add_syntax_rules_r(); // Добавить правила для R
		} else if (strcmp(extension, ".m") == 0 || strcmp(extension, ".mm") == 0) {
			add_syntax_rules_objective_c(); // Добавить правила для Objective-C
		} else if (strcmp(extension, ".mat") == 0) {
			add_syntax_rules_matlab(); // Добавить правила для MATLAB
		} else if (strcmp(extension, ".sh") == 0) {
			add_syntax_rules_shell(); // Добавить правила для Shell
		} else if (strcmp(extension, ".groovy") == 0) {
			add_syntax_rules_groovy(); // Добавить правила для Groovy
		} else if (strcmp(extension, ".bat") == 0) {
			add_syntax_rules_batch(); // Добавить правила для Batch
		} else if (strcmp(extension, ".ps1") == 0) {
			add_syntax_rules_powershell(); // Добавить правила для PowerShell
		} else if (strcmp(extension, ".fs") == 0) {
			add_syntax_rules_fsharp(); // Добавить правила для F#
		} else if (strcmp(extension, ".cs") == 0) {
			add_syntax_rules_csharp(); // Добавить правила для C#
		} else if (strcmp(extension, ".html") == 0 || strcmp(extension, ".htm") == 0) {
			add_syntax_rules_html(); // Добавить правила для HTML
		} else if (strcmp(extension, ".css") == 0) {
			add_syntax_rules_css(); // Добавить правила для CSS
		}
	}
		
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

    // Restore cursor visibility before exit
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

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