#ifndef SYNTAX_H
#define SYNTAX_H

// Перечисление со всеми доступными цветами
enum AvailableColors {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    BROWN = 6,
    LIGHT_GRAY = 7,
    DARK_GRAY = 8,
    LIGHT_GREEN = 9,
    LIGHT_CYAN = 10,
    LIGHT_RED = 11,
    LIGHT_MAGENTA = 12,
    LIGHT_BLUE = 13,
    LIGHT_YELLOW = 14,
    WHITE = 15
};

// Определение цветов
enum Color {
    COLOR_KEYWORD = LIGHT_GREEN,
    COLOR_FUNCTION = LIGHT_YELLOW,
    COLOR_STRING = MAGENTA,
    COLOR_NUMBER = LIGHT_CYAN,
    COLOR_COMMENT = DARK_GRAY,
    COLOR_HEADER = LIGHT_BLUE,
    COLOR_INCLUDE = LIGHT_BLUE
};

#define NUM_KEYWORDS 20
#define NUM_FUNCTIONS 15
#define NUM_HEADERS 15
#define MAX_SYNTAX_RULES 100

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

void add_syntax_rules_all();
#endif // SYNTAX_H
