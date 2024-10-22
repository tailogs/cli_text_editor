# Text Editor

## Изображение

![image](https://github.com/user-attachments/assets/e895d593-1032-4f74-8592-85a0b85be659) <br> Рисунок 1. Интерфейс программы

## Описание

CLITE (CLI TEXT EDITOR) — это простой текстовый редактор для Windows с графическим интерфейсом. Поддерживает редактирование текстовых файлов с подсветкой синтаксиса, поддерживает различные операции редактирования и копирование текста в буфер обмена.

## Почему?

Моя основная система это Windows, но я так же время от времени использую и читаю что-то про linux. Мне нравиться редактор `Vim` и `Nano` ([форк под виндовс](https://github.com/okibcn/nano-for-windows)). Помимо них я использовал множество иных редакторов, но было желания создать и свой редактор, которую я и реализовываю тут. Сам же я использую зачастую в работу редакторы упомянуты ранее и так же sublime text, vscode, notepad++ и другие уже не совсем редакторы, а полноценные IDE.

## Функции

- **Подсветка синтаксиса**: Поддержка подсветки ключевых слов, функций, строк и чисел.
- **Работа с файлами**: Открытие, сохранение и создание файлов.
- **Интерфейс командной строки**: Работа с текстом через консоль.

## Сборка

Для сборки проекта вам понадобится установленный [GCC](https://gcc.gnu.org/) и [MinGW](https://www.mingw-w64.org/downloads/) (если вы работаете на Windows).

1. **Установите необходимые инструменты** (GCC и MinGW).

2. **Создайте исполняемый файл**:

   Откройте командную строку и перейдите в каталог, содержащий ваш исходный код и `Makefile`. Выполните команду:

   ```bash
   make
   ```

   Это соберет проект и создаст исполняемый файл `text_editor.exe`.

## Использование

1. **Запустите приложение открытием нового или существующего файла**, указав его имя в командной строке:

   ```bash
   clite.exe <filename>
   ```

2. **Используйте клавиши** для редактирования текста:
   - Введите текст в файл.
   - Используйте стрелки вверх и вниз для навигации по строкам.
   - Нажмите `Enter` для добавления новой строки.
   - Нажмите `Backspace` для удаления символа.
   - Нажмите `Tab` для вставки табуляции.

3. **Копирование текста** в буфер обмена:
   - При нахождении на нужной строке, нажмите `Ctrl+C` для копирования текста строки (пока что нумерация строк тоже копируется).

4. **Сохраните изменения** в файл:

   Нажмите `Ctrl+S` для сохранения файла
   Нажмите `Ctrl+Q` для выхода из файла или проводника.
   Нажмите `Ctrl+E` для открытия проводника.

**Отображение версии**:
  - `clite -v` или `clite --v` или `clite -version` или `clite --version`

**Замена пробелов на табуляции. Подходит для Makefile**
  - `clite <filename> --repair`

## Очистка

Для очистки собранных файлов выполните команду:

```bash
make clean
```

Это удалит объектные файлы и исполняемый файл.

## Примечания

- Убедитесь, что все зависимости установлены правильно, чтобы избежать проблем с компиляцией или запуском.
- Если вы хотите корректной работы и тем более отображения русского языка, то ставьте кодировку у `main.c` файла на `Windows 1251`.

## Ошибки и предупреждения

- Присутствуют предупреждения и требуют исправления. Кретических ошибок не имеется.

## Планы на будущее

- **Редактирование текста**: Возможность ввода, удаления и вставки символов и строк.
- **Копирование текста**: Копирование текста в буфер обмена без номера строки.
- **Новый файл**: Если файл не имеется и он создан, но в него не вписан код, то мы должны отменять создание этого файла путем удаления.
- **Мышь**: Осуществить перемешение каретки по средствам движения мыши по строке и по строкам.
- **Загаловочные файлы**: Исправить подсветку загаловочных файлов и `#include`, которые перестали работать когда я внес все функции языка Си.
- **Подергивание**: Убрать поддергивания консоли при движении в нем каретки по строкам.
- **Оптимизация и улучшение читабельности кода**: Нужно сделать код проще, последовательнее и читаемее, а программу быстрее и приятнее в использовании. Минимализировать число ошибок. Учитывать нештатное поведение и возможное появление непредвиденных ошибок.
- **Настройки**: Реализовать настройки подобные `vim` и `nano` и удобство их открытия `vim ~/.vimrc` и `nano ~/.nanorc`.
- **Интеграции**: Сделать возможность интеграции плагинов, модов, синтаксиса и прочего в программе.
- **Макро-язык**: Реализовать упрощенную версию яп специально для программы дабы упростить работу с настройкам и интеграцией.
- **Скобки**: Упростить подсветку скобок разной вложенности. Это особенно будет полезно для lisp программистов. Ну и добавить комментарии вида `;`, `;;`, `;;;` и `;;;;` тоже для них.
- **Подсветка синтаксиса**: Сделать подсветку ещё большему числу языков программирования.
- **Языки**: Сделать поддержку других языков, таких как русский, японский, китайский и прочие, но я этим заниматься не буду. Для меня это будет повод подучить английский. Но если кто-то реализует поддержку языков, то я против не буду и обязательно запихну в свою программу.

## Как добавить поддержку своего синтаксиса языка?

Очень просто! Только с комментариями все плохо, но все базовые комментарии я уже добавил, если вам нужно специфичный добавить, то пишите мне или сделайте это сами, но потом не забудьте поделиться - так вы сделаете и другим приятно! :)

Чтобы добавить свой язык вам нужно открыть файлы `syntax.c`, `syntax.h` и `main.c`. Не бойтесь, добавить там нужно будет совсем немного кода. По порядку:

1. Откройте файл `syntax.c` и перед функцией `add_syntax_rules_all` которая находиться в самом конце добавьте функцию `add_syntax_rules`, но дописав через `_` нижнее подчеркивание название языка, например `add_syntax_rules_java`.
2. Скопируйте тело любой из функций выше, которые расписывают свой синтаксис и измените под себя. Например у примера функции яп (языка программирования) `java` было такая реализация тела:

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

3. Вы редактируете это содержимое поменяв массив `keywords`, `functions`, `headers` на свои функции, загаловки и ключевые слова. Можете указать и стиль комментариев в конце, но это не важно, но лучше сделать для того чтобы точно работало.
4. Возьмите имя функции и впишите его в конец тела функции `add_syntax_rules_all` чтобы другие могли сразу наглядно видеть какие реализации языков присутсвуют в коде.
5. Добавьте прототип функции в `syntax.h`. Например у `java` примера выше этот прототип такой `void add_syntax_rules_java();`
6. Откройте файл `main.c` и в главной функции `main` добавьте в тело условия `if (extension)` условие вида:

	```c
	else if (strcmp(extension, ".java") == 0) {
		add_syntax_rules_java();
	}
	```

	Тут мы просто добавили в функцию `strcmp` вторым аргументом расширение нашего языка, а потом внутрь тела `if` вписали функцию которую создали ранее, в нашем случае `add_syntax_rules_java`.

8. Скомпируйте программу и протестируйте подсветку синтаксиса создав файл с вашим расширением языка и введя в него код любой

## Изменения версий
- **v1.0**: Первая версия. Весь базовый функционал готов.
- **v1.1**: Добавлено больше поддерживаемых языков программирования, разделен на файлы код, улучшен функционал покраски цвета.
- **v1.1.1**: Исправлено отсутствие появления курсора при выходе из программы версии.
- **v1.1.2**: Исправлено отображение табуляции версии
- **v1.1.3**: Исправлены все предупреждения, отображаемые компилятором версии.
- **v1.1.4**: Изменена кодировка файла на `Windows 1251` и исправлен вывод русских символов.
- **v1.1.5**: Исправлено предупреждение компилятора.
- **v1.1.6**: Расширен список ключевых слов, функций и заголовков на языках C и C++.
- **v1.2**: Улучшена покраска символов в консоли и курсор.
- **v1.2.3**: Теперь можно пробелы в Makefile (и других файлах) заменить табуляцией такой командой `clite <filename> --repair`
- **v1.3.4**: Добавлен проводник, который позволяет легче ориентироваться в каталоге проекта.

## Лицензия

Этот проект является открытым и свободно распространяемым по лицензии **MIT**. Вы можете использовать и модифицировать его в соответствии с вашими потребностями.

---

Разработано [Tailogs](https://github.com/tailogs).

