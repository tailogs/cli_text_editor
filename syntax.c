#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syntax.h"

SyntaxRule syntax_rules[MAX_SYNTAX_RULES];
int syntax_rules_count = 0;

void add_syntax_rule(const char* keyword, int color) {
    if (syntax_rules_count < MAX_SYNTAX_RULES) {
        syntax_rules[syntax_rules_count].keyword = (char*)malloc(strlen(keyword) + 1);
        if (syntax_rules[syntax_rules_count].keyword) {
            strcpy(syntax_rules[syntax_rules_count].keyword, keyword);
        }
        syntax_rules[syntax_rules_count].color = color;
        syntax_rules_count++;
    }
}

// Универсальная функция для добавления правил подсветки
void add_syntax_rules(const char* rules[], int color, int num_rules) {
    for (int i = 0; i < num_rules; ++i) {
        add_syntax_rule(rules[i], color);
    }
}

void add_syntax_rules_c_and_cpp() {
    const char* keywords[] = {
        "do", "int", "return", "for", "while", "if", "else", "class", "public", "private", 
        "protected", "struct", "namespace", "template", "void", "static", "auto", "const", 
        "sizeof", "true", "false", "NULL", "break", "continue", "switch", "case", "default", 
        "typedef", "volatile", "extern", "register", "union", "goto", "long", "short", 
        "char", "float", "double", "unsigned", "signed", "size_t", "this", "virtual", 
        "override", "final", "noexcept", "decltype", "constexpr", "thread_local", 
        "alignas", "alignof", "static_assert", "dynamic_cast", "static_cast", "const_cast", 
        "reinterpret_cast", "requires", "concept", "co_return", "co_yield", "co_await", 
        "import", "module", "export", "char8_t", "char16_t", "char32_t", "constexpr", 
        "consteval", "constinit", "decltype", "explicit", "inline", "module"
    };
    
    const char* functions[] = {
        // C стандартные функции (без пространств имен)
        "printf", "scanf", "fprintf", "fscanf", "sprintf", "sscanf", "fopen", "fclose", 
        "fgets", "fputs", "fread", "fwrite", "fflush", "fseek", "ftell", "rewind", "malloc", 
        "calloc", "realloc", "free", "exit", "atexit", "abs", "rand", "srand", "strlen", 
        "strcpy", "strncpy", "strcat", "strncat", "strcmp", "strncmp", "strchr", "strrchr", 
        "strstr", "memcpy", "memmove", "memcmp", "memset", "isdigit", "isalpha", "islower", 
        "isupper", "tolower", "toupper", "sin", "cos", "tan", "sqrt", "pow", "log", 
        "exp",

        // C++ стандартные функции с пространствами имен
        // std::cout, std::cin и т.д.
        "std::cout", "std::cin", "std::cerr", "std::clog", 
        "std::ifstream", "std::ofstream", "std::fstream", "std::stringstream",
        "std::vector", "std::map", "std::set", "std::list", "std::sort", "std::find",
        "std::thread", "std::mutex", "std::lock_guard", "std::unique_lock", "std::async", 
        "std::promise", "std::future", "std::atomic", "std::shared_ptr", "std::weak_ptr", 
        "std::make_shared", "std::make_unique", 

        // Упрощенные варианты
        "cout", "cin", "cerr", "clog", 
        "ifstream", "ofstream", "fstream", "stringstream",
        "vector", "map", "set", "list", "sort", "find",
        "thread", "mutex", "lock_guard", "unique_lock", "async", 
        "promise", "future", "atomic", "shared_ptr", "weak_ptr", 
        "make_shared", "make_unique",

        // std::chrono и другие
        "std::chrono::steady_clock", "chrono::steady_clock", "steady_clock",
        "std::chrono::high_resolution_clock", "chrono::high_resolution_clock", "high_resolution_clock",
        "std::chrono::system_clock", "chrono::system_clock", "system_clock",
        "std::chrono::duration", "chrono::duration", "duration",
        "std::chrono::seconds", "chrono::seconds", "seconds",
        "std::chrono::milliseconds", "chrono::milliseconds", "milliseconds",
        "std::chrono::microseconds", "chrono::microseconds", "microseconds",
        "std::chrono::nanoseconds", "chrono::nanoseconds", "nanoseconds",
        "std::tuple", "tuple", "std::get", "get", 
        "std::apply", "apply"
    };

    
    const char* headers[] = {
        // C стандартные заголовочные файлы
        "<stdio.h>", "<stdlib.h>", "<string.h>", "<conio.h>", "<windows.h>", 

        // C++ стандартные заголовочные файлы
        "<iostream>", "<fstream>", "<sstream>", "<vector>", "<map>", "<set>", "<list>", 
        "<algorithm>", "<cassert>", "<cctype>", "<cfloat>", "<climits>", "<cstdarg>", 
        "<cstddef>", "<cstdio>", "<cstdlib>", "<cstring>", "<cwchar>", "<cwctype>", 
        "<locale>", "<new>", "<typeinfo>", "<utility>", "<bitset>", "<complex>", 
        "<exception>", "<functional>", "<future>", "<initializer_list>", "<iterator>", 
        "<mutex>", "<random>", "<ratio>", "<regex>", "<system_error>", "<thread>", 
        "<tuple>", "<typeindex>", "<utility>", "<chrono>", "<atomic>", "<shared_mutex>", 
        "<shared_lock>", "<lock_guard>", "<unique_lock>", "<memory>", "<filesystem>", 
        "<numbers>", "<format>", "<ranges>", "<concepts>"
    };

    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rules(headers, COLOR_HEADER, sizeof(headers) / sizeof(headers[0]));
    add_syntax_rule("#include", COLOR_INCLUDE);
}

void add_syntax_rules_python() {
    const char* keywords[] = {
        "def", "class", "return", "if", "elif", "else", "while", "for", "break", 
        "continue", "pass", "import", "from", "as", "try", "except", "finally", 
        "raise", "with", "assert", "lambda", "yield", "print", "True", "False", "None"
    };
    
    const char* functions[] = {
        "print", "len", "range", "int", "float", "str", "list", "dict", "set", "tuple", 
        "sorted", "sum", "min", "max", "abs"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("#", COLOR_COMMENT);
}

void add_syntax_rules_ruby() {
    const char* keywords[] = {
        "def", "class", "module", "end", "if", "else", "elsif", "unless", "while", 
        "until", "for", "break", "next", "redo", "retry", "return", "begin", "rescue", 
        "ensure", "raise", "yield", "super", "self", "true", "false", "nil"
    };
    
    const char* functions[] = {
        "puts", "print", "gets", "chomp", "length", "each", "map", "select", "reject", 
        "inject", "reduce", "join", "split", "downcase", "upcase"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("#", COLOR_COMMENT);
}

void add_syntax_rules_javascript() {
    const char* keywords[] = {
        "function", "return", "if", "else", "for", "while", "do", "break", "continue", 
        "switch", "case", "default", "try", "catch", "finally", "throw", "async", "await", 
        "class", "extends", "import", "export", "const", "let", "var", "new", "this", 
        "super", "null", "true", "false"
    };
    
    const char* functions[] = {
        "console.log", "alert", "prompt", "parseInt", "parseFloat", "setTimeout", 
        "setInterval", "clearTimeout", "clearInterval", "fetch"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("//", COLOR_COMMENT);
    add_syntax_rule("/*", COLOR_COMMENT);
    add_syntax_rule("*/", COLOR_COMMENT);
}

void add_syntax_rules_java() {
    const char* keywords[] = {
        "public", "private", "protected", "class", "interface", "extends", "implements", 
        "static", "final", "abstract", "synchronized", "volatile", "transient", "native", 
        "return", "if", "else", "switch", "case", "default", "while", "do", "for", 
        "break", "continue", "try", "catch", "finally", "throw", "throws", "import", 
        "package", "new", "this", "super", "null", "true", "false"
    };
    
    const char* functions[] = {
        "System.out.println", "System.out.print", "System.out.printf", "String.valueOf", 
        "Integer.parseInt", "Double.parseDouble", "Math.sqrt", "Math.pow", "Math.max", 
        "Math.min", "Collections.sort", "Arrays.sort"
    };
    
    const char* headers[] = {
        "import", "package"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rules(headers, COLOR_INCLUDE, sizeof(headers) / sizeof(headers[0]));
    add_syntax_rule("//", COLOR_COMMENT);
    add_syntax_rule("/*", COLOR_COMMENT);
    add_syntax_rule("*/", COLOR_COMMENT);
}

void add_syntax_rules_php() {
    const char* keywords[] = {
        "echo", "print", "if", "else", "elseif", "for", "foreach", "while", "do", 
        "switch", "case", "default", "function", "return", "try", "catch", "finally", 
        "throw", "class", "public", "private", "protected", "extends", "implements", 
        "static", "abstract", "final", "new", "instanceof", "isset", "unset", "empty", 
        "isset", "strlen", "str_replace", "array_push", "array_pop", "count", "date"
    };
    
    const char* functions[] = {
        "echo", "print", "strlen", "str_replace", "array_push", "array_pop", "count", 
        "date", "isset", "unset", "empty", "json_encode", "json_decode", "file_get_contents", 
        "file_put_contents", "preg_match", "preg_replace", "fopen", "fclose", "fread", 
        "fwrite"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("//", COLOR_COMMENT);
    add_syntax_rule("/*", COLOR_COMMENT);
    add_syntax_rule("*/", COLOR_COMMENT);
}

void add_syntax_rules_kotlin() {
    const char* keywords[] = {
        "fun", "val", "var", "if", "else", "when", "for", "while", "do", "try", 
        "catch", "finally", "throw", "return", "continue", "break", "class", "object", 
        "interface", "data", "sealed", "enum", "constructor", "init", "companion", 
        "lateinit", "by", "is", "as", "in", "out", "typealias", "abstract", "final", 
        "open", "override"
    };
    
    const char* functions[] = {
        "println", "print", "readLine", "toInt", "toDouble", "toString", "max", "min", 
        "sum", "average", "joinToString", "split", "filter", "map"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("//", COLOR_COMMENT);
    add_syntax_rule("/*", COLOR_COMMENT);
    add_syntax_rule("*/", COLOR_COMMENT);
}

void add_syntax_rules_rust() {
    const char* keywords[] = {
        "fn", "let", "mut", "if", "else", "match", "loop", "while", "for", "break", 
        "continue", "return", "mod", "use", "pub", "crate", "self", "super", "enum", 
        "struct", "trait", "impl", "const", "static", "async", "await", "type", "unsafe", 
        "ref", "move", "Box", "Rc", "Arc", "Cell", "RefCell", "Mutex", "RwLock", "HashMap", 
        "Vec", "Option", "Result", "Some", "None", "Ok", "Err"
    };
    
    const char* functions[] = {
        "println!", "print!", "format!", "vec!", "format!", "panic!", "assert!", "assert_eq!", 
        "assert_ne!", "debug_assert!", "debug_assert_eq!", "debug_assert_ne!"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("//", COLOR_COMMENT);
    add_syntax_rule("/*", COLOR_COMMENT);
    add_syntax_rule("*/", COLOR_COMMENT);
}

void add_syntax_rules_swift() {
    const char* keywords[] = {
        "func", "let", "var", "if", "else", "guard", "switch", "case", "default", 
        "for", "while", "repeat", "break", "continue", "return", "throw", "try", 
        "catch", "defer", "class", "struct", "enum", "protocol", "extension", 
        "import", "super", "self", "nil", "true", "false"
    };
    
    const char* functions[] = {
        "print", "println", "append", "contains", "count", "map", "filter", "reduce", 
        "sorted", "split", "joined", "write", "read"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("//", COLOR_COMMENT);
    add_syntax_rule("/*", COLOR_COMMENT);
    add_syntax_rule("*/", COLOR_COMMENT);
}

void add_syntax_rules_perl() {
    const char* keywords[] = {
        "sub", "my", "local", "our", "if", "elsif", "else", "unless", "while", "for", 
        "foreach", "last", "next", "redo", "return", "given", "when", "default", 
        "use", "require", "package", "strict", "warnings", "unless", "eval", "try", 
        "catch", "die", "unless"
    };
    
    const char* functions[] = {
        "print", "say", "chomp", "split", "join", "length", "push", "pop", "shift", 
        "unshift", "map", "grep", "sort", "reverse", "substr", "index", "rindex"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("#", COLOR_COMMENT);
}

void add_syntax_rules_haskell() {
    const char* keywords[] = {
        "let", "in", "where", "case", "of", "do", "if", "then", "else", "data", 
        "type", "class", "instance", "newtype", "module", "import", "qualified", 
        "as", "hiding", "deriving", "default", "forall", "mdo", "foreign", "inline", 
        "pragma"
    };
    
    const char* functions[] = {
        "map", "filter", "foldr", "foldl", "head", "tail", "length", "concat", 
        "reverse", "zip", "unzip", "elem", "notElem", "and", "or", "not"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("--", COLOR_COMMENT);
    add_syntax_rule("{-", COLOR_COMMENT);
    add_syntax_rule("-}", COLOR_COMMENT);
}

void add_syntax_rules_go() {
    const char* keywords[] = {
        "func", "var", "const", "if", "else", "for", "range", "switch", "case", 
        "default", "return", "break", "continue", "goto", "defer", "select", 
        "interface", "type", "struct", "map", "chan", "import", "package", "error", 
        "nil", "true", "false"
    };
    
    const char* functions[] = {
        "fmt.Println", "fmt.Printf", "fmt.Sprintf", "fmt.Scan", "fmt.Scanln", 
        "strconv.Itoa", "strconv.Atoi", "strings.Split", "strings.Join", 
        "strings.Contains", "strings.Replace", "os.Open", "os.Create", "os.Exit"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("//", COLOR_COMMENT);
    add_syntax_rule("/*", COLOR_COMMENT);
    add_syntax_rule("*/", COLOR_COMMENT);
}

void add_syntax_rules_typescript() {
    const char* keywords[] = {
        "function", "return", "if", "else", "for", "while", "do", "break", "continue",
        "switch", "case", "default", "try", "catch", "finally", "throw", "class", 
        "interface", "extends", "implements", "constructor", "public", "private", 
        "protected", "static", "readonly", "abstract", "enum", "namespace", "type",
        "const", "let", "var", "null", "undefined", "true", "false"
    };
    
    const char* functions[] = {
        "console.log", "console.error", "console.warn", "fetch", "setTimeout", 
        "setInterval", "clearTimeout", "clearInterval", "parseInt", "parseFloat"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("//", COLOR_COMMENT);
    add_syntax_rule("/*", COLOR_COMMENT);
    add_syntax_rule("*/", COLOR_COMMENT);
}

void add_syntax_rules_scala() {
    const char* keywords[] = {
        "def", "val", "var", "if", "else", "while", "for", "yield", "match", 
        "case", "object", "class", "trait", "extends", "with", "import", 
        "package", "new", "return", "throw", "try", "catch", "finally", 
        "abstract", "final", "sealed", "implicit", "lazy", "override", 
        "private", "protected", "public"
    };
    
    const char* functions[] = {
        "println", "print", "readLine", "toInt", "toDouble", "toString", 
        "map", "filter", "reduce", "flatMap", "foreach", "mkString"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("//", COLOR_COMMENT);
    add_syntax_rule("/*", COLOR_COMMENT);
    add_syntax_rule("*/", COLOR_COMMENT);
}

void add_syntax_rules_lua() {
    const char* keywords[] = {
        "function", "local", "end", "if", "then", "elseif", "else", "while", 
        "do", "repeat", "until", "for", "in", "break", "return", "goto", 
        "continue", "and", "or", "not", "true", "false", "nil"
    };
    
    const char* functions[] = {
        "print", "pairs", "ipairs", "next", "type", "tostring", "tonumber", 
        "require", "assert", "error", "collectgarbage", "load", "loadfile", 
        "dofile", "pcall", "xpcall"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("--", COLOR_COMMENT);
    add_syntax_rule("--[[", COLOR_COMMENT);
    add_syntax_rule("--]]", COLOR_COMMENT);
}

void add_syntax_rules_dart() {
    const char* keywords[] = {
        "class", "abstract", "enum", "extends", "implements", "static", "final", 
        "const", "var", "dynamic", "void", "return", "if", "else", "for", 
        "while", "do", "switch", "case", "default", "break", "continue", 
        "try", "catch", "finally", "throw", "async", "await", "await", 
        "import", "library", "part", "with", "set", "get", "factory"
    };
    
    const char* functions[] = {
        "print", "int.parse", "double.parse", "List.of", "Set.of", "Map.of", 
        "DateTime.now", "print", "Future.delayed"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("//", COLOR_COMMENT);
    add_syntax_rule("/*", COLOR_COMMENT);
    add_syntax_rule("*/", COLOR_COMMENT);
}

void add_syntax_rules_elixir() {
    const char* keywords[] = {
        "def", "defp", "defmodule", "defmacro", "do", "end", "if", "unless", 
        "cond", "case", "with", "for", "try", "catch", "raise", "throw", 
        "import", "require", "use", "alias", "module", "super", "self"
    };
    
    const char* functions[] = {
        "IO.puts", "IO.inspect", "Enum.map", "Enum.filter", "Enum.reduce", 
        "String.length", "String.upcase", "String.downcase", "List.first", 
        "List.last", "List.flatten"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("#", COLOR_COMMENT);
}

void add_syntax_rules_r() {
    const char* keywords[] = {
        "function", "return", "if", "else", "for", "while", "repeat", 
        "break", "next", "try", "catch", "finally", "require", "library", 
        "source", "load", "save", "data", "plot", "summary", "lm", "glm", 
        "apply", "sapply", "lapply", "tapply", "aggregate"
    };
    
    const char* functions[] = {
        "print", "cat", "mean", "sd", "sum", "length", "nrow", "ncol", 
        "head", "tail", "str", "dim", "summary", "ggplot", "plot"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("#", COLOR_COMMENT);
}

void add_syntax_rules_objective_c() {
    const char* keywords[] = {
        "class", "interface", "protocol", "implementation", "id", "instancetype", 
        "void", "if", "else", "for", "while", "do", "switch", "case", 
        "default", "break", "continue", "return", "try", "catch", "finally", 
        "synchronized", "property", "nonatomic", "atomic", "readonly", 
        "readwrite", "strong", "weak", "assign", "copy", "retain", "dynamic"
    };
    
    const char* functions[] = {
        "NSLog", "NSString.stringWithFormat", "NSArray.arrayWithObjects", 
        "NSDictionary.dictionaryWithObjectsAndKeys", "NSNumber.numberWithInt", 
        "NSNumber.numberWithDouble", "NSArray.arrayWithCapacity"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("//", COLOR_COMMENT);
    add_syntax_rule("/*", COLOR_COMMENT);
    add_syntax_rule("*/", COLOR_COMMENT);
}

void add_syntax_rules_matlab() {
    const char* keywords[] = {
        "function", "return", "if", "else", "elseif", "for", "while", 
        "switch", "case", "otherwise", "break", "continue", "try", 
        "catch", "end", "global", "persistent", "private", "public", 
        "static", "import", "load", "save"
    };
    
    const char* functions[] = {
        "disp", "fprintf", "sprintf", "size", "length", "mean", "median", 
        "std", "sum", "plot", "xlabel", "ylabel", "title", "legend"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("%", COLOR_COMMENT);
}

void add_syntax_rules_groovy() {
    const char* keywords[] = {
        "def", "class", "interface", "package", "import", "if", "else", 
        "elseif", "while", "for", "break", "continue", "return", "try", 
        "catch", "finally", "throw", "throws", "switch", "case", "default", 
        "with", "as", "static", "synchronized", "public", "private", 
        "protected", "abstract", "final", "native", "transient", "volatile"
    };
    
    const char* functions[] = {
        "println", "print", "size", "collect", "each", "find", "findAll", 
        "grep", "sort", "unique", "join", "split", "replace", "trim", 
        "toString", "equals", "compareTo"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("//", COLOR_COMMENT);
    add_syntax_rule("/*", COLOR_COMMENT);
    add_syntax_rule("*/", COLOR_COMMENT);
}

void add_syntax_rules_shell() {
    const char* keywords[] = {
        "if", "then", "else", "elif", "fi", "for", "in", "do", "done", 
        "while", "until", "break", "continue", "case", "esac", "function", 
        "return", "export", "source", "unset", "declare", "local", "readonly", 
        "trap", "set", "getopts"
    };
    
    const char* functions[] = {
        "echo", "printf", "read", "grep", "awk", "sed", "cut", "sort", 
        "wc", "find", "xargs", "basename", "dirname", "mkdir", "rm", "cp", 
        "mv", "chmod", "chown", "ln", "ps", "kill"
    };
    
    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("#", COLOR_COMMENT);
}

void add_syntax_rules_batch() {
    const char* keywords[] = {
        "echo", "set", "if", "else", "goto", "for", "del", "copy", "move", 
        "mkdir", "rmdir", "pause", "cls", "call", "exit", "rem", "start", 
        "taskkill", "type", "attrib", "ren", "find", "findstr", "dir"
    };

    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rule("rem", COLOR_COMMENT);
    add_syntax_rule("::", COLOR_COMMENT);
}

void add_syntax_rules_powershell() {
    const char* keywords[] = {
        "function", "param", "if", "else", "elseif", "for", "foreach", "while", 
        "do", "try", "catch", "finally", "throw", "return", "switch", "case", 
        "default", "break", "continue", "exit", "import-module", "export", 
        "Get-Command", "Get-Help", "Get-Process", "Get-Service", "Set-Variable", 
        "Get-Variable", "Remove-Variable", "New-Object", "Add-Content", "Clear-Content"
    };

    const char* functions[] = {
        "Write-Output", "Write-Host", "Read-Host", "Start-Process", "Invoke-Command", 
        "Out-File", "Out-Host", "Out-String", "ConvertTo-Json", "ConvertFrom-Json"
    };

    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("#", COLOR_COMMENT);
}

void add_syntax_rules_fsharp() {
    const char* keywords[] = {
        "let", "mutable", "if", "then", "else", "elif", "match", "with", 
        "for", "in", "while", "do", "try", "with", "finally", "catch", 
        "throw", "raise", "module", "open", "namespace", "type", "abstract", 
        "interface", "class", "inherit", "override", "member", "static", 
        "new", "delegate", "fun", "async", "await"
    };

    const char* functions[] = {
        "printf", "printfn", "eprintf", "eprintfn", "printf", "sprintf", 
        "String.concat", "String.split", "String.trim", "List.map", "List.filter", 
        "List.fold", "List.reduce", "List.iter", "Seq.map", "Seq.filter", 
        "Seq.fold", "Seq.iter", "Seq.toList", "Seq.toArray"
    };

    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("//", COLOR_COMMENT);
    add_syntax_rule("(*", COLOR_COMMENT);
    add_syntax_rule("*)", COLOR_COMMENT);
}

void add_syntax_rules_csharp() {
    const char* keywords[] = {
        "public", "private", "protected", "internal", "static", "abstract", 
        "virtual", "override", "sealed", "new", "partial", "readonly", 
        "volatile", "unsafe", "const", "enum", "struct", "class", "interface", 
        "namespace", "using", "if", "else", "switch", "case", "default", 
        "for", "foreach", "while", "do", "break", "continue", "return", 
        "try", "catch", "finally", "throw", "async", "await", "delegate", 
        "event", "operator", "this", "base", "yield", "lock"
    };

    const char* functions[] = {
        "Console.WriteLine", "Console.ReadLine", "Console.Write", "Console.Read", 
        "String.Format", "String.Concat", "String.Join", "String.Split", 
        "String.Substring", "String.Replace", "String.ToUpper", "String.ToLower", 
        "Math.Abs", "Math.Pow", "Math.Sqrt", "Math.Max", "Math.Min", "DateTime.Now", 
        "DateTime.UtcNow", "DateTime.Parse", "DateTime.TryParse", "DateTime.ToString"
    };

    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rules(functions, COLOR_FUNCTION, sizeof(functions) / sizeof(functions[0]));
    add_syntax_rule("//", COLOR_COMMENT);
    add_syntax_rule("/*", COLOR_COMMENT);
    add_syntax_rule("*/", COLOR_COMMENT);
}

void add_syntax_rules_html() {
    const char* keywords[] = {
        "html", "head", "body", "title", "meta", "link", "script", "style", 
        "div", "span", "a", "img", "table", "tr", "td", "th", "form", "input", 
        "button", "select", "option", "textarea", "h1", "h2", "h3", "h4", 
        "h5", "h6", "p", "br", "hr", "ol", "ul", "li", "nav", "header", 
        "footer", "section", "article", "aside", "main", "iframe", "embed", 
        "source", "audio", "video", "canvas", "svg", "path", "polygon", 
        "circle", "rect", "line", "ellipse"
    };

    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rule("<!--", COLOR_COMMENT);
    add_syntax_rule("-->", COLOR_COMMENT);
}

void add_syntax_rules_css() {
    const char* keywords[] = {
        "color", "background", "border", "margin", "padding", "font", "text", 
        "display", "position", "width", "height", "top", "bottom", "left", 
        "right", "float", "clear", "z-index", "overflow", "align", "justify", 
        "flex", "grid", "transition", "transform", "animation", "opacity", 
        "box-shadow", "border-radius", "line-height", "letter-spacing", 
        "text-align", "text-decoration", "text-transform", "white-space", 
        "visibility", "cursor", "pointer", "resize", "user-select", "list-style"
    };

    add_syntax_rules(keywords, COLOR_KEYWORD, sizeof(keywords) / sizeof(keywords[0]));
    add_syntax_rule("/*", COLOR_COMMENT);
    add_syntax_rule("*/", COLOR_COMMENT);
}

// Главная функция для добавления правил подсветки для всех языков
void add_syntax_rules_all() {
    add_syntax_rules_c_and_cpp();
    add_syntax_rules_python();
    add_syntax_rules_ruby();
    add_syntax_rules_javascript();
    add_syntax_rules_java();
    add_syntax_rules_php();
    add_syntax_rules_kotlin();
    add_syntax_rules_rust();
    add_syntax_rules_swift();
    add_syntax_rules_perl();
    add_syntax_rules_haskell();
    add_syntax_rules_go();
    add_syntax_rules_typescript();
    add_syntax_rules_scala();
    add_syntax_rules_lua();
    add_syntax_rules_dart();
    add_syntax_rules_elixir();
    add_syntax_rules_r();
    add_syntax_rules_objective_c();
    add_syntax_rules_matlab();
    add_syntax_rules_shell();
    add_syntax_rules_groovy();
    add_syntax_rules_batch();
    add_syntax_rules_powershell();
    add_syntax_rules_fsharp();
    add_syntax_rules_csharp();
    add_syntax_rules_html();
    add_syntax_rules_css();
}