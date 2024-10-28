![image](https://github.com/user-attachments/assets/8d3a2710-b417-45fe-b949-87e6797fcd64))]

CLITE (CLI TEXT EDITOR) is a simple text editor for Windows with a graphical interface. It supports editing text files with syntax highlighting and various editing operations, including copying text to the clipboard.

### Why?

My primary system is Windows, but I also occasionally use and read about Linux. I enjoy editors like `Vim` and `Nano` (a [Windows fork](https://github.com/okibcn/nano-for-windows)). Besides these, I've used many other editors, but I wanted to create my own, which I'm implementing here. I often use the previously mentioned editors as well as Sublime Text, VSCode, Notepad++, and other full-fledged IDEs.

### Features

- **Syntax Highlighting**: Support for highlighting keywords, functions, strings, and numbers.
- **File Management**: Opening, saving, and creating files.
- **Command-Line Interface**: Working with text through the console.

### Building

To build the project, you need to have [GCC](https://gcc.gnu.org/) and [MinGW](https://www.mingw-w64.org/downloads/) installed (if you are on Windows).

1. **Install necessary tools** (GCC and MinGW).
2. **Create an executable file**:

   Open the command prompt and navigate to the directory containing your source code and `Makefile`. Run the command:

   ```bash
   make
   ```

   This will build the project and create the executable file `text_editor.exe`.

### Usage

1. **Run the application by opening a new or existing file**, specifying its name in the command line:

   ```bash
   clite.exe <filename>
   ```

2. **Use keys to edit text**:
   - Type text into the file.
   - Use the up and down arrows to navigate through lines.
   - Press `Enter` to add a new line.
   - Press `Backspace` to delete a character.
   - Press `Tab` to insert a tab.

3. **Copying text to the clipboard**:
   - When on the desired line, press `Ctrl+C` to copy the line's text (line numbers are also copied for now).

4. **Save changes to a file**:

   Press `Ctrl+S` to save the file.
   Press `Ctrl+Q` to exit from the file or explorer.
   Press `Ctrl+E` to open the explorer.
   Press `Ctrl+F` while in the explorer to create a file.
   Press `Ctrl+C` while in the explorer to create a directory.
   Press `Ctrl+D` while in the explorer to delete a file or directory.

**Display version**:
  - `clite -v`, `clite --v`, `clite -version`, or `clite --version`

**Replacing spaces with tabs. Suitable for Makefile**
  - `clite <filename> --repair`

### Cleaning Up

To clean up built files, run:

```bash
make clean
```

This will remove object files and the executable.

### Notes

- Ensure all dependencies are installed correctly to avoid compilation or runtime issues.
- For correct operation and especially for displaying Russian characters, set the encoding of `main.c` file to `Windows 1251`.

### Errors and Warnings

Warnings are present and need fixing. There are no critical errors.

### Future Plans

- **Text Editing**: Ability to input, delete, and insert characters and lines.
- **Text Copying**: Copying text to clipboard without line numbers.
- **New File**: If a file is created but no code is written in it, we should cancel its creation by deleting it.
- **Mouse Support**: Implement cursor movement using mouse across lines.
- **Header Files**: Fix syntax highlighting for header files and `#include`, which stopped working after adding all C language functions.
- **Flickering**: Remove flickering in console when moving cursor across lines.
- **Code Optimization and Readability Improvements**: Simplify code, make it more consistent and readable, speed up the program, minimize errors, consider edge cases and unexpected errors.
- **Settings**: Implement settings similar to `vim` and `nano`, making them easy to open (`vim ~/.vimrc`, `nano ~/.nanorc`).
- **Integrations**: Allow integration of plugins, mods, syntax highlighting, etc., into the program.
- **Macro Language**: Implement a simplified programming language specifically for this program to ease configuration and integration work.
- **Bracket Highlighting**: Simplify highlighting of brackets of different nesting levels; this will be particularly useful for Lisp programmers. Also add comments like `;`, `;;`, `;;;`, and `;;;;`.
- **Syntax Highlighting**: Expand syntax highlighting support for more programming languages.
- **Language Support**: Add support for other languages such as Russian, Japanese, Chinese, etc., but I won't be working on this myself. It will motivate me to improve my English. However, if someone else implements language support, I won't oppose it and will definitely include it in my program.

### How to Add Support for Your Language Syntax?

It's very simple! There are some issues with comments, but I've already added basic comments; if you need specific ones added, feel free to contact me or do it yourself but remember to share afterward—this way you’ll make others happy too! :)

To add your language support you need to open files `syntax.c`, `syntax.h`, and `main.c`. Don’t worry; you’ll only need to add a little code. Here’s how:

1. Open the file `syntax.c` and before the function `add_syntax_rules_all`, which is at the very end, add a function called `add_syntax_rules_<your_language_name>`.
2. Copy the body of any of the above functions that outline their syntax and modify it according to your needs. For instance, here’s how it looks for Java:

	```java
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
	```

3. Edit this content by changing arrays like `keywords`, `functions`, and `headers` according to your functions, headers, and keywords. You can specify comment styles at the end as well; it’s not crucial but better for proper functioning.
4. Take your function name and write it at the end of the body of function `add_syntax_rules_all` so others can see which language implementations are present in the code at a glance.
5. Add function prototype in `syntax.h`. For example for Java above it would be like this: `void add_syntax_rules_java();`
6. Open file `main.c` and in main function add an if condition like this:

	```c
	else if (strcmp(extension, ".java") == 0) {
		add_syntax_rules_java();
	}
	```

	This simply adds an extension check in function strcmp as its second argument while calling your previously created function inside that if block.

8. Compile the program and test syntax highlighting by creating a file with your language's extension and entering any code into it.

### Version Changes
- **v1.0**: Initial version with all basic functionality ready.
- **v1.1**: Added more supported programming languages; separated code into files; improved color highlighting functionality.
- **v1.1.1**: Fixed cursor not appearing upon exiting program version.
- **v1.1.2**: Fixed tab display issue in version output.
- **v1.1.3**: Corrected all warnings displayed by compiler in version output.
- **v1.1.4**: Changed file encoding to Windows 1251; fixed display of Russian characters.
- **v1.1.5**: Fixed compiler warning issue.
- **v1.1.6**: Expanded list of keywords, functions, and headers for C and C++ languages.
- **v1.2**: Improved character coloring in console along with cursor visibility.
- **v1.2.3**: Now spaces in Makefile (and other files) can be replaced with tabs using command: clite <filename> --repair
- **v1.3.4**: Added an explorer that makes navigating project directories easier.

### License

This project is open-source and freely distributed under the MIT license. You can use and modify it according to your needs.

---

Developed by [Tailogs](https://github.com/tailogs).

Citations:
[1] https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/15576043/0db77755-882f-4c84-90be-743a20035123/paste.txt
