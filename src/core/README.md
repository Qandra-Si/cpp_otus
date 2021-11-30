# libcore of cpp_otus

Библиотека libcore, в составе проекта [cpp_otus](https://github.com/Qandra-Si/cpp_otus).

## Домашняя работа №1

Библиотека libcore используется в качестве демонстрации подключения внешней библиотеки к приложениям, см. пример в директории [../homework01](../homework01). Кроме того, в ходе 1го домашнего задания требовалось разработать приложение ip_filter, где также пригодилась библиотека libcore, в которой были реализованы методы для работы с пулом ipv4-адресов, см. файл [include/core/ipv4.h](include/core/ipv4.h).

Использование пула ipv4-адресов было протестировано с помощью gtest, см. [test/test_ipv4.cpp](test/test_ipv4.cpp).

## Домашняя работа №2

Библиотека libcore используется в качестве реализации классов "фейковая куча", "аллокатор фейковой кучи", "кольцевой буфер" и "итератор кольцевого буфера", см. файлы [include/core/fake_heap.h](include/core/fake_heap.h), [include/core/fake_heap_allocator.h](include/core/fake_heap_allocator.h) и [include/core/ringbuffer.h](include/core/ringbuffer.h). Более подробные пояснения см. в корневом [README.md](https://github.com/Qandra-Si/cpp_otus) файле проекта.

Реализация алгоритмов было протестировано с помощью gtest, см. [test/test_fake_heap.cpp](test/test_fake_heap.cpp) и [test/test_rindbuffer.cpp](test/test_rindbuffer.cpp).

## Домашняя работа №3

В домашней работе №3 предлагалось использовать внешний сервис [travis](travis-ci.com) для генерации doxygen-документации. При этом сервис требует предоставление ему GitHub token, что как минимум несекьюрно и совершенно неприемлемо в моём случае. Тем более, учитывая тот факт что cmake и GitHub actions являются весьма мощным инструментом, которые способны легко решить поставленную задачу без привлечения сторонних сервисов.

Вот последовательность шагов, которые "от начала и до конца" выполняют поставленную задачу как для получения документации в локальной рабочей копии, так и для публикации её с помощью GitHub actions.

### Doxygen

За генерацию документации этой библиотеки отвечает блок строк в корневом [CMakeLists.txt](/CMakeLists.txt#L97), начинающийся со строки `find_package(Doxygen QUIET)`. Блок cmake-команд отвечает за формирование конфигурационного doxygen файла, а также за добавление target-а в состав проекта, таким образом, что с помощью команды `make doxygen` разработчик в локальной рабочей копии может сгенерировать необходимую документацию и проверить предупреждения, которые выдаёт doxygen во время работы.

В случае, если на целевой платформе будет обнаружен doxygen, то файл [doxygen.conf.in](../doxygen.conf.in) в помощью cmake-генератора будет сконвертирован в формат, пригодный для работы doxygen. Исходный файл Doxygen всё так же может быть получен с помощью команды `doxygen -g` и отредактирован самостоятельно. В типовом сгенерированном файле были изменены следующие строки, которые влияют на формирование этой документации:
* PROJECT_NAME = "libcore of cpp_otus"
* OUTPUT_DIRECTORY = @DOXYGEN_OUTPUT@
* BUILTIN_STL_SUPPORT = YES
* TYPEDEF_HIDES_STRUCT = YES
* EXTRACT_STATIC = YES
* EXTRACT_ANON_NSPACES = YES
* CASE_SENSE_NAMES = YES
* INPUT = @DOXYGEN_INPUT@/../README.md @DOXYGEN_INPUT@
* FILE_PATTERNS = README.md *.h
* RECURSIVE = YES
* USE_MDFILE_AS_MAINPAGE = README.md
* ALPHABETICAL_INDEX = NO
* IGNORE_PREFIX = 5
* HTML_TIMESTAMP = YES
* DOCSET_BUNDLE_ID = qandra-si.github.io/cpp_otus
* DOCSET_PUBLISHER_ID = github.com/Qandra-Si
* DOCSET_PUBLISHER_NAME = Qandra Si
* QHP_NAMESPACE = qandra-si.github.io/cpp_otus
* ECLIPSE_DOC_ID = qandra-si.github.io/cpp_otus
* GENERATE_LATEX = NO

Сгенерированная документация появится в каталоге docs в корне проекта.

Наличие doxygen на целевой платформе не обязательно, но в этом случае документация генерироваться не будет.

Для того, чтобы документация была автоматически опубликована как GinHub Pages, надо перейти в настройки проекта, включить модуль "GitHub Pages", указать источник файлов Branch:gh-pages/(root). Документация станет доступна по ссылке: https://qandra-si.github.io/cpp_otus/

## Домашняя работа №5

Библиотека libcore используется в качестве реализации шаблона "2х-мерная разреженная бесконечная матрица", см. файл [include/core/matrix.h](include/core/matrix.h). Более подробные пояснения см. в корневом [README.md](https://github.com/Qandra-Si/cpp_otus) файле проекта.

Реализация алгоритмов было протестировано с помощью gtest, см. [test/test_matrix.cpp](test/test_matrix.cpp).
