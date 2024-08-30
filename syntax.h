#ifndef SYNTAX_H
#define SYNTAX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

// Цвет текста
#define TXT_BLACK "30"
#define TXT_RED "31"
#define TXT_GREEN "32"
#define TXT_YELLOW "33"
#define TXT_BLUE "34"
#define TXT_MAGENTA "35"
#define TXT_CYAN "36"
#define TXT_WHITE "37"

// Цвет фона
#define BACK_BLACK "40"
#define BACK_RED "41"
#define BACK_GREEN "42"
#define BACK_YELLOW "43"
#define BACK_BLUE "44"
#define BACK_MAGENTA "45"
#define BACK_CYAN "46"
#define BACK_WHITE "47"

// Яркий цвет текста
#define TXT_LIGHT_RED "1;31"
#define TXT_LIGHT_GREEN "1;32"
#define TXT_LIGHT_YELLOW "1;33"
#define TXT_LIGHT_BLUE "1;34"
#define TXT_LIGHT_MAGENTA "1;35"
#define TXT_LIGHT_CYAN "1;36"
#define TXT_LIGHT_WHITE "1;37"

// Яркий цвет фона
#define BACK_LIGHT_RED "1;41"
#define BACK_LIGHT_GREEN "1;42"
#define BACK_LIGHT_YELLOW "1;43"
#define BACK_LIGHT_BLUE "1;44"
#define BACK_LIGHT_MAGENTA "1;45"
#define BACK_LIGHT_CYAN "1;46"
#define BACK_LIGHT_WHITE "1;47"

// Комбинированный макрос для текста и фона
#define TXT_ON_BACK(TEXT_COLOR, BACKGROUND_COLOR) "\033[0;" TEXT_COLOR ";" BACKGROUND_COLOR "m"

// Макросы для одиночного применения цвета
#define TXT_ONLY(TEXT_COLOR) "\033[0;" TEXT_COLOR "m"
#define BACK_ONLY(BACKGROUND_COLOR) "\033[0;" TEXT_WHITE ";" BACKGROUND_COLOR "m"

// Сброс цвета
#define CMD_RESET_COLOR "\033[0m"

// Функция для включения поддержки ANSI-кодов в Windows
void enableVirtualTerminalProcessing(HANDLE hConsole);
void print_colored_symbol_and_text(const char* symbol, const char* symbol_text_color, const char* symbol_background_color, 
                                    const char* main_text, const char* main_text_color, const char* main_background_color);

// Перечисление со всеми доступными цветами
enum AvailableColors {
    BLACK = 0,
    RED = 1,
    GREEN = 2,
    YELLOW = 3,
    BLUE = 4,
    MAGENTA = 5,
    CYAN = 6,
    WHITE = 7,
    DARK_GRAY = 8,
    LIGHT_RED = 9,
    LIGHT_GREEN = 10,
    LIGHT_YELLOW = 11,
    LIGHT_BLUE = 12,
    LIGHT_MAGENTA = 13,
    LIGHT_CYAN = 14,
    LIGHT_WHITE = 15
};

// Определение цветов
enum Color {
    COLOR_KEYWORD = LIGHT_GREEN,
    COLOR_FUNCTION = LIGHT_MAGENTA,
    COLOR_STRING = MAGENTA,
    COLOR_NUMBER = LIGHT_CYAN,
    COLOR_COMMENT = DARK_GRAY,
    COLOR_HEADER = LIGHT_BLUE,
    COLOR_INCLUDE = LIGHT_BLUE,
    COLOR_MACROS = LIGHT_YELLOW
};

#define NUM_KEYWORDS 20
#define NUM_FUNCTIONS 15
#define NUM_HEADERS 15
#define MAX_SYNTAX_RULES 100000

// Syntax highlighting rules
typedef struct {
    char* keyword;
    int color;
} SyntaxRule;

extern SyntaxRule syntax_rules[MAX_SYNTAX_RULES];
extern int syntax_rules_count;  // Объявление переменной

// Универсальная функция для добавления правил подсветки
void add_syntax_rule(const char* rule, int color);
void add_syntax_rules(const char* rules[], int color, int num_rules);

// Функции для добавления правил подсветки для разных языков
void add_syntax_rules_c_and_cpp();
void add_syntax_rules_python();
void add_syntax_rules_ruby();
void add_syntax_rules_javascript();
void add_syntax_rules_java();
void add_syntax_rules_php();
void add_syntax_rules_kotlin();
void add_syntax_rules_rust();
void add_syntax_rules_swift();
void add_syntax_rules_perl();
void add_syntax_rules_haskell();
void add_syntax_rules_go();
void add_syntax_rules_typescript();
void add_syntax_rules_scala();
void add_syntax_rules_lua();
void add_syntax_rules_dart();
void add_syntax_rules_elixir();
void add_syntax_rules_r();
void add_syntax_rules_objective_c();
void add_syntax_rules_matlab();
void add_syntax_rules_shell();
void add_syntax_rules_groovy();
void add_syntax_rules_batch();
void add_syntax_rules_powershell();
void add_syntax_rules_fsharp();
void add_syntax_rules_csharp();
void add_syntax_rules_html();
void add_syntax_rules_css();
void add_syntax_rules_lisp();
void add_syntax_rules_makefile();
void add_syntax_rules_resource();

void add_syntax_rules_all();
#endif // SYNTAX_H
