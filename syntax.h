#ifndef SYNTAX_H
#define SYNTAX_H

// Определение цветов
enum Color {
    COLOR_KEYWORD = 10,  // Зеленый
    COLOR_FUNCTION = 14, // Светло-синий
    COLOR_STRING = 12,   // Красный
    COLOR_NUMBER = 11,   // Светло-голубой
    COLOR_COMMENT = 8,   // Темно-серый
    COLOR_HEADER = 9,    // Светло-зеленый
    COLOR_INCLUDE = 13   // Фиолетовый
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
