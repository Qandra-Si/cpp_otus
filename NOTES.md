# Заметки (конспект) лекций

## Документы стандартов C++
* [Обновляемый черновик стандарта](http://eel.is/c++draft/) в html формате
* [Обновляемый список черновиков стандарта](https://stackoverflow.com/a/4653479), ссылки на черновики в .pdf формате разных версий

## Занятие №1. Система сборки: build, test and deploy

### Дополнительные материалы по теме

* [Git Book](https://git-scm.com/book/ru/v2)
* [CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)
* [Тестирование в CMake](https://habr.com/ru/post/433822/)
* [CMake: CPack Generators](https://cmake.org/cmake/help/latest/module/CPack.html)
* [CMake: пример настройки сборки инсталлятора](https://github.com/colinbourassa/librosco/blob/master/CMakeLists.txt)
* [Как собрать ProjectConfig.cmake файл](https://gitlab.kitware.com/cmake/community/-/wikis/doc/tutorials/How-to-create-a-ProjectConfig.cmake-file)
* [GitHub Actions](https://docs.github.com/en/actions)
* [GitHub Actions: integrating GTest](https://github.com/bastianhjaeger/github_actions_gtest_example)
* [GitHub highlighting code blocks](https://docs.github.com/en/github/writing-on-github/working-with-advanced-formatting/creating-and-highlighting-code-blocks)
* [GitHub Actions: environment variables](https://stackoverflow.com/a/61428342) и ещё [environment variables](https://docs.github.com/en/actions/learn-github-actions/workflow-commands-for-github-actions#setting-an-environment-variable)
* [GitHub Actions: матрица сборки](https://temofeev.ru/info/articles/ispolzovanie-github-actions-s-c-i-cmake/)
* [Bash Shell Parameter Expansion](https://stackoverflow.com/a/9533099)

## Занятие №2. Особенности C++11. auto, lambda, tuple

Практические занятия по 2му занятию находятся в директориях `lesson03_deducing_types`, `lesson03_move`, `lesson03_nrvo`,... При этом корневой проект *cpp_otus* является общим для всех включённых в него target-ов, выступающих в роли отдельных программ/подпроектов. Выполненная 1я часть домашнего задания находится в директории `homework03_deducing_types`. Выполненная 2я часть домашнего задания в директориях `homework03_custom_tie` и `homework03_custom_tuple`. Сборка упражнений выполняется с помощью cmake-параметра TARGETS по инструкции приведённой ниже.

Для сборки проектов 3го занятия следует выполнить:

```bash
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release -DTARGETS="homework03_deducing_types homework03_custom_tie homework03_custom_tuple" ..
cmake --build . --config Release
```

Пояснения по первому выполненному упражнению находятся в построчных комментариях коде программы.

Что касается реализации второго упражнения, то сделаю несколько ремарок:
* `auto getPerson() {}` в коде на c++11 мы писать не можем, эта конструкция появилась в c++14;
* реализация custom_tuple и custom_tie без использования variadic templates (которые по плану должны быть пройдены на следующем занятии) свелась к простому копированию элементов кортежа;
* полностью расписывать инициализацию объектов, конструкторы копирования и операторы присваивания во всех шаблонах я не стал, т.к. по заданию требовалось достичь работоспособности только предложенного кода, - доработка возможна, но смысла без использования variadic templates не имеет.

### Дополнительные материалы по теме

* [CPP Reference: C++11](https://en.cppreference.com/w/cpp/11)
* [Расширения в C++11](https://isocpp.org/wiki/faq/cpp11-language-misc) рассматриваются на примерах расширения C++11
* [Online compiler: выбор версий C++98, C++11, C++14](http://cpp.sh)
* [Online compiler: online отладка](https://www.onlinegdb.com) поддерживает C, C++, Python, Java, PHP, Ruby, Perl, C#, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
* [Online compiler: выбор версий до C++20](https://cppinsights.io) удобно смотреть промежуточный код после транслятора
* [Online compiler: транслятор в asm](https://godbolt.org/z/5WYvs6Yaq) удобно смотреть сгенерированный asm код, поддерживается множестко компиляторов и множество языков
* [Online compiler: выбор версий до экспериментальных](https://wandbox.org/) присутствует поддержка boost
* [Инициализация в C++](https://habr.com/ru/post/438492/)
* [CPP Reference: list initialization](https://en.cppreference.com/w/cpp/language/list_initialization) список инициализации
* [auto &&](https://stackoverflow.com/a/13242177)

## Занятие №3. Особенности C++11. rvalue и lvalue ссылки, move semantic, variadic templates, POD

### RVO и NRVO

RVO и NRVO - важная тема, для понимания move semantic, см. пример в `lesson03_nrvo`.

```bash
mkdir ./build && cd ./build

# выбираем gcc
sudo update-alternatives --config c++
# update-alternatives: используется /usr/bin/g++-10 для предоставления /usr/bin/c++ (c++) в ручном режиме

# собираем проект
cmake -DCMAKE_BUILD_TYPE=Release -DCPP_OTUS_SKIP_TEST=TRUE -DTARGETS=lesson03_nrvo ..
cmake --build . --config Release
bin/Release/cpp_otus_lesson03_nrvo
# смотрим на результат выполнения программы, - обращаем внимание, что complex NRVO не работает

# выбираем clang
sudo update-alternatives --config c++
# update-alternatives: используется /usr/bin/clang++-10 для предоставления /usr/bin/c++ (c++) в автоматическом режиме

# собираем проект
rm -R ./*
cmake -DCMAKE_BUILD_TYPE=Release -DCPP_OTUS_SKIP_TEST=TRUE -DTARGETS=lesson03_nrvo ..
cmake --build . --config Release
bin/Release/cpp_otus_lesson03_nrvo
# смотрим на результат выполнения программы, - обращаем внимание, что complex NRVO в clang работает
```

Особое внимание обращаем на примеры class.copy.elision, подробности см. [здесь](https://eel.is/c++draft/class.copy.elision).

### rvalue и lvalue

`lvalue` (*locator value*) - представляет собой объект, который занимает идентифицируемое место в памяти (например, имеет адрес).
`rvalue` - определяется исключением от lvalue, т.е. это выражение, которое не представляет собой объект, который занимает идентифицируемое место в памяти.

Изначально, в C определение lvalue означало буквально "выражение, применимое с левой стороны оператора присваивания", однако позже это перестало быть возможным:
```cpp
const int a = 10;
a = 33; // error: assignment of read-only variable ‘a’
```
Позже, начиная с C99 было введено определение `modifiable lvalue`, которое было задано сложным определением с перечислением вариантов.

Возможны преобразования lvalue в rvalue, например `lvalue = a + b;`, где a и b изначально было lvalue. Однако же rvalue в lvalue не преобразовываются, но могут lvalue может быть получено из rvalue явням образом, например `*(ptr + 1) = 33;`, где ptr+1 это rvalue, но *(rvalue) уже lvalue. Унарный оператор `&` принимает lvalue и производит rvalue, например `int b, *a = &b;`, где b это lvalue, но &b это уже rvalue, т.е. например `&b = 40;` приведёт к ошибке.

В C++ константным ссылкам на lvalue можно присваивать rvalue, т.к. они константы, то значение не м.б. изменено по ссылке и проблема модификации rvalue отсутствует, например `const std::string & ref = "This's a const";`.

C++11 разрешает иметь ссылки на rvalue и тем самым изменять их. В частности && - это ссылка на rvalue, даёт ссылку на rvalue, который будет уничтожен после вызова. В этом случае `T& operator=(T&&)` следует считать переносящим оператором присванивания.

```bash
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release -DCPP_OTUS_SKIP_TEST=TRUE -DTARGETS=lesson03_move ..
cmake --build . --config Release
bin/Release/cpp_otus_lesson03_move
```

Оператор && позволяет делать с объектом всё что угодно (почти), но не следует забывать, что у объекта будет рано или поздно вызван деструктор и он не должен генерировать никакого UB.

### move semantic

См. примеры выше.

Использоваться может для передачи владения, или для оптимизации работы с памятью.

### variadic templates

```bash
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release -DCPP_OTUS_SKIP_TEST=TRUE -DTARGETS=lesson03_vtempl ..
cmake --build . --config Release
bin/Release/cpp_otus_lesson03_vtempl
```

### Дополнительные материалы по теме

* [RVO и NRVO](http://alenacpp.blogspot.com/2008/02/rvo-nrvo.html) return value optimization и named return value optimization
* [NRVO](http://msdn2.microsoft.com/en-us/library/ms364057(vs.80).aspx) статья на MSDN о том, как работает NRVO
* [C++ Standard: class.copy.elision](https://eel.is/c++draft/class.copy.elision) стандарт C++ [class.copy.ellision]
* [Понимание lvalue и rvalue в C и С++](https://habr.com/ru/post/348198/)
* [CPP Reference: parameter pack](http://gnuwin32.sourceforge.net/packages/coreutils.htm)
* [CPP Reference: move constructors](https://en.cppreference.com/w/cpp/language/move_constructor)
* [CPP Reerence: std::forward](https://en.cppreference.com/w/cpp/utility/forward)
* [CPP Reerence: std::move](https://en.cppreference.com/w/cpp/utility/move)
* [Perfect forwarding и универсальные ссылки](https://habr.com/ru/post/242639/) статья о том, как работает emplace и forward

## Занятие №4. Сравнение C++11/C++14 с С++ 03

```bash
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Debug -DCPP_OTUS_SKIP_TEST=TRUE -DTARGETS=lesson04 ..
cmake --build . --config Debug
bin/Debug/cpp_otus_lesson04_cpp1x_innovations
# требуется debug-сборка, т.к. в лекции рассматривались assert-ы (в release сбоя не будет)
```

### Дополнительные материалы по теме

* [C++ Guidelines](https://isocpp.github.io/CppCoreGuidelines/) полезные рекомендации о том, как писать на C++
* [CPP Reference: user defined literals](https://en.cppreference.com/w/cpp/language/user_literal)
* [CPP Reference: constexpr](https://en.cppreference.com/w/cpp/language/constexpr)
* [CPP Reference: initializer list](https://en.cppreference.com/w/cpp/utility/initializer_list)
* [CPP Reference: enum declaration](https://en.cppreference.com/w/cpp/language/enum) про enum class
* [CPP Reference: static assert](https://en.cppreference.com/w/cpp/language/static_assert)
* [Нововведения в C++11](https://github.com/AnthonyCalandra/modern-cpp-features/blob/master/CPP11.md)
* [Нововведения в C++14](https://github.com/AnthonyCalandra/modern-cpp-features/blob/master/CPP14.md)

## Занятие №5. Умные указатели

Умные указатели: `unique_ptr`, `shared_ptr`, `weak_ptr`, `enable_shared_from_this`.

```bash
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release -DCPP_OTUS_SKIP_TEST=TRUE -DSOLUTION=lesson05 ..
cmake --build . --config Release
bin/Debug/cpp_otus_lesson05_smart_ptr
bin/Debug/cpp_otus_lesson05_mleak_ctor
```

### unique_ptr

Позволяет безраздельно владеть объектом. Не разрешает копировать (допускает только перемещение). Размер зависит от пользовательского deleter-а. Без особой логики удаления издержки чаще всего отсутствуют. Конструкция `std::make_unique` делает код менее громоздким, плюс предохраняет от exceptions.

### shared_ptr

Позволяет копировать указатели с разделением владения. Копирование небесплатное. Внутри shared_ptr всегда два указателя. Конструкция `std::make_shared` предпочтительнее вызова конструктора. Непотокобезопасный (потокобезопасен только относительно контрольного блока, и то не всегда). Можно создать из unique_ptr. Приводит к фрагментации памяти и вследствии "разбрасывания" дескрипторов данных по памяти размещается "далеко" и не может не попадать в один процессорный кеш, - потому рекомендуется использовать make_shared, для минимизации фрагментации.

### weak_ptr

Следует аккуратно пользоваться `std::make_shared` вместе с `weak_ptr`, т.к. make_shared создаёт единый кусок в памяти с указателями и на блок данных, и на контрольный блок, и эта память отдельно друг от друга удалиться не может. Возможна утечка памяти, если weak_ptr будет удерживать контрольные блоки при перекрёстных ссылках (см. пример в `lesson05_smart_ptr`).

### enable_shared_from_this

Нельзя вызывать в конструкторе - пока объект не создан, нет this, нечего вернуть. Нельзя возвращать shared_from_this для объекта класса, для которого не было создано shared_ptr - произойдёт исключение.

### Исключения к конструкторах

Внимание! на 30 минуте 5го занятия прозвучала спорная фраза касающаяся вызова деструкторов в случае, если в конструкторе произошло исключение. В стандарте на эту тему есть чёткая формулировка, как именно должна себя вести программа, см. подробности тут [[except.ctor]](http://eel.is/c++draft/except.ctor#3). Пример, демонстрирующий пояснение к вызову деструкторов находится в директории `lesson05_mleak_ctor`, который можон собрать с помощью параметра `-DTARGETS=lesson05_mleak_ctor`.

### Дополнительные материалы по теме

* [S++ Standard: exception handling, constructors and destructors](http://eel.is/c++draft/except.ctor) раздел [except.ctor]
* [CPP Reference: unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr)
* [CPP Reference: shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr)
* [CPP Reference: weak_ptr](https://en.cppreference.com/w/cpp/memory/weak_ptr)
* [Предпосылки к умным указателям в C++](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2003/n1431.htm)

## Занятие №6. Аллокаторы в C++

Проблемы: дополнительные издержки при частом создании/удалении объектов, фрагментация, необходимость отладки утечек памяти, циклических ссылок, повреждения памяти, проблемы с кешем при непоследовательном размещении.

Исправить ситуацию можно: переопределить malloc, переопределить operator new, - но это глобально повлияет на работу программы; либо же переопределить аллокатор для контейнера. 

```bash
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release -DCPP_OTUS_SKIP_TEST=TRUE -DSOLUTION=lesson06 ..
cmake --build . --config Release
bin/Release/cpp_otus_lesson06_alloc
bin/Release/cpp_otus_lesson06_pmr
```

### Дополнительные материалы по теме

* [Требования стандарта C++ к аллокаторам ](https://eel.is/c++draft/allocator.requirements) и [определение аллокатора в C++](http://eel.is/c++draft/default.allocator)
* [Декларация allocator traits в стандарте C++](http://eel.is/c++draft/allocator.traits)
* [CPP Reference: allocator_traits](https://en.cppreference.com/w/cpp/memory/allocator_traits)
* [CPP Reference: polymorphic_allocator](https://en.cppreference.com/w/cpp/memory/polymorphic_allocator)
* [Исходный код std::allocator в libstdc++ gcc](https://code.woboq.org/gcc/libstdc++-v3/include/bits/allocator.h.html)
* [Реализация custom allocator-а](https://docs.ros.org/en/foxy/Tutorials/Allocator-Template-Tutorial.html) пример с пояснениями
* [Сложный сценарий использования allocator](http://cpp.sh/8k4vt) и [простое базовое поведение stateless allocator](http://cpp.sh/9hvrm)
* [Allocator requirement...](https://stackoverflow.com/a/22349253) say that copies of an allocator must be able to free each others' memory, so it is not generally possible to store the memory inside the allocator object.
* [Пример разработки собственного iterator](https://internalpointers.com/post/writing-custom-iterators-modern-cpp) по шагам

## Занятие №7. Шаблонная магия. Метафункции, рекурсия и условия, SFINAE

TODO: предложено выполнить самостоятельную реализацию шаблонов из библиотеки std::type_traits.

Этапы работы компилятора: препроцессинг, инстанциирование шаблонов, компиляция.

```cpp
struct foo {
  static const int value = 10;
};
template <int V>
struct foo {
  static const int value = 10;
};
foo::value; // есть всегда
foo<10>::value; // появляется при использовании

template <int V>
struct foo {
  static const int value = V;
};
foo<10>::value;
constexpr int foo(int V) { return V; }
foo(10);
```

Добавляем логику в использование шаблонов:

```cpp
template <int V>
struct abs {
  static const int value = (V < 0) ? -V : V;
};
abs<-10>::value;
abs<10>::value;

template <int P>
struct fact {
  static_assert?
  static const int value = P * fact<P-1>::value;
};
template<>
struct fact<0> {
  static const int value = 1;
};
fact<11>::value;
fact<-1>::value; // -ftemplate-depth
```

Проверка типов, как параметров

```cpp
template<typename T>
struct is_int {
  static const bool value = false;
};
template<>
struct is_int<int> {
  static const bool value = true;
};
is_int<int>::value;
```

Тип как результат

```cpp
template <typename T>
struct remove_const {
  using type = T;
};
template <typename T>
struct remove_const<const T> {
  using type = T;
};
remove_const<int>::type a1;
remove_const<const int>::type a2;

// ---

template <typename T>
struct type_is {
  using type = T;
};
template <typename T>
struct remove_const : type_is<T> { };
template <typename T>
struct remove_const<const T> : type_is<T> { };
template <typename T>
using remove_const_t = typename remove_const<T>::type; // реализация alias-а
```

Ветвление в метапрограммировании:

```cpp
template <bool C, class T, class F>
struct conditional : type_id<T> { };

template<class T, class F>
struct conditional<false, T, F> : type_is<F> { };
```

SFINAE

Sibstitution Failure is not an Error - ошибка подстановки, не ошибка.

```cpp
template <bool B, class T>
struct enable_if : type_is<T> { };

template <class T>
struct enable_if<false, T> { };

enable_if<false, int>::type;
```

Для сборки примеров 7го урока следует выполнить команды:

```bash
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release -DSOLUTION=lesson07 ..
cmake --build . --config Release
bin/Release/
```

### Дополнительные материалы по теме

* [Пример использования метапрограммирования](https://blog.mattbierner.com/stupid-template-tricks-snake/) на задаче с примерами
* [CPP Reference: type_traits]()https://en.cppreference.com/w/cpp/header/type_traits
* [Почему следует избегать использовать std::enable_if](https://newbedev.com/why-should-i-avoid-std-enable-if-in-function-signatures)

## Занятие №8. Обзор C++17. Constexpr lambda. Fold expression. Attributes. Type deduction

TODO: на следующий день после поступления на курс прослушал онлайн-урок, требуется повторить упражнения разобранные в уроке.

### Дополнительные материалы по теме

* [Standard C++: видео-курсы](https://www.youtube.com/playlist?list=PL3BR09unfgcjJ2YUCgh62vgv_1maXcKuS)

## Занятие №9. Обзор C++17. Constexpr lambda. Fold expression. Attributes. Type deduction

TODO: требуется повторить упражнения разобранные в уроке.
TODO: вывести в консоль время последнего изменения файла, std:chrono?

### Дополнительные материалы по теме

* [CPP Reference: inline specifier](https://en.cppreference.com/w/cpp/language/inline)
* [CPP Reference: storage duration](https://en.cppreference.com/w/cpp/language/storage_duration)
* [CPP Reference: one definition rule](https://en.cppreference.com/w/cpp/language/definition)

## Занятие №10. C++20 - что это и зачем?

TODO: каким-то образом пропустил это занятие? прослушать лекцию в записи.

## Занятие №11. Принципы проектирования ПО. SoC, DRY, YAGNI, KISS, TDA, LoD, SOLID. Язык UML

Доклад на тему что такое архитектура ПО, зачем она нужна, что включают себя критерии оценки успешности проектирования архитектуры ПО. В этой связи были изложены основные принципы проектирования ПО.

В лекции не понравилось то, что примеры, статьи и дополнительные материалы на изучение даются на языке Java. В целом, конечно понятно, в том числе учитывая и мой опыт работы на Java. Однако создаётся впечатление, что мир C++ можно легко описать миром Java. Однако это не так. Это разные миры, со своими подходами, разными парадигмами, обношениями и реализациями одних и тех же шаблонов проектирования и т.д. и т.п. "Страхи" программистов C++ учтены и заложены в основу языка Java, поэтому на мой взгляд рассматривать подходы и проектирование C++ программ по примерам на языке Java по меньшей мере ущербно (если такое проектирование распространяется на крупные проекты, а не только на теологические упражнения в стиле `class Lion extends Animal`... упс `class lion_t : public animal_t` конечно же).

### DRY

Расшифровывается как **Don’t Repeat Youself** – не повторяйся, также известен как **DIE** - **Duplication Is Evil** - дублирование это зло. Этот принцип заключается в том, что нужно избегать повторений одного и того же кода. Лучше использовать универсальные свойства и функции.

### KISS

**Keep It Simple, Stupid** – не усложняй! Смысл этого принципа программирования заключается в том, что стоит делать максимально простую и понятную архитектуру, применять шаблоны проектирования и не изобретать велосипед.

### SOLID

Принцип **SOLID** в упрощенном варианте означает, что когда при написании кода используется несколько принципов вместе, то это значительно облегчает дальнейшую поддержку и развитие программы. Полностью акроним расшифровывается так:

* **S**ingle responsibility principle – принцип единственной обязанности (на каждый класс должна быть возложена одна-единственная обязанность);
* **O**pen/closed principle – принцип открытости/закрытости (программные сущности должны быть закрыты для изменения но открыты для расширения);
* **L**iskov substitution principle – принцип подстановки Барбары Лисков (функции, которые используют базовый тип, должны иметь возможность использовать подтипы базового типа, не зная об этом. Подклассы не могут замещать поведения базовых классов. Подтипы должны дополнять базовые типы);
* **I**nterface segregation principle – принцип разделения интерфейса (много специализированных интерфейсов лучше, чем один универсальный);
* **D**ependency inversion principle – принцип инверсии зависимостей (зависимости внутри системы строятся на основе абстракций. Модули верхнего уровня не зависят от модулей нижнего уровня. Абстракции не должны зависеть от деталей. Детали должны зависеть от абстракций);

### YAGNI

Термин YAGNI значит **You Ain’t Gonna Need It** - вам это не понадобится! Его суть в том, чтобы реализовать только поставленные задачи и отказаться от избыточного функционала.

### Дополнительные материалы по теме

* [Doxygen](https://www.doxygen.nl/)
* [Рисовалка UML](https://plantuml.com/ru/)
* [Конвертер документации](https://pandoc.org/)
* [Разъяснения по UML диаграммам](http://book.uml3.ru/sec_1_4)
* [Пояснения по принципу SOLID](https://medium.com/webbdev/solid-4ffc018077da)
* [Подходы к проектированию: как два программиста хел пекли](https://habr.com/ru/post/153225/) шуточная статья, но поучительная
* [Техники codereview](https://habr.com/ru/post/340550/) шуточная статья, но поучительная
* [Google Style Guides](https://google.github.io/styleguide/) и оттуда же [Google Style Guildes: C++](https://google.github.io/styleguide/cppguide.html)
* [KISS, DRY, YAGNI, BDUF, SOLID, APO и бритва Оккама](https://habr.com/ru/company/itelma/blog/546372/) статья на Хабре

## Занятие №12. Принципы проектирования GRASP

Урок не понравился. Изложены были очевидные вещи.

## Занятие №13. Шаблоны GoF. Порождающие

TODO: требуется решить проблему с map-ом в примере в уроке на 30й минуте

## Занятие №14. Шаблоны GoF. Структурные

### Адаптер (Adapter)

Предназначен для организации использования функций объекта, недоступного для модификации, через специально созданный интерфейс.
Другими словами — это структурный паттерн проектирования, который позволяет объектам с несовместимыми интерфейсами работать вместе.

### Мост (Bridge)

Предназначен для разделения абстракции и реализации так, чтобы они могли изменяться независимо.
Использует инкапсуляцию, агрегирование и может использовать наследование для того, чтобы разделить ответственность между классами.

Хороший [пример](http://cpp.sh/6bhzw):
```cpp
# в .h файле

class master_t
{
public:
    master_t();
private:
  class impl_t * p_impl;
};

# в .cpp файле

class master_t::impl_t { };

master_t::master_t() : p_impl(new impl_t()) { }
```

### Компоновщик (Composite)

Предназначен для объединения объектов в древовидную структуру для представления иерархии от частного к целому.
Позволяет клиентам обращаться к отдельным объектам и к группам объектов используя одинаковый интерфейс.

### Декоратор (Decorator)

Предназначен для динамического подключения дополнительного поведения к объекту.
Предоставляет гибкую альтернативу практике создания подклассов с целью расширения функциональности.

Хороший пример: class `raw_file_t` -> `class zip_file_t` -> `class encrypted_file_t`, реализовывая общий метод write() можно модернизировать поведение.

### Фасад (Facade)

Предназначен для скрытия сложности системы путём сведения всех возможных внешних вызовов к одному объекту, делегирующему их соответствующим объектам системы.

### Приспособленец (Flyweight)

Предназначен для переопределения поведения объекта, представляющего из себя уникальный экземпляр, который по факту не является таковым и ведёт себя по разному в разных местах программы.

Хороший пример: можно не хранить копию вырезанной фотографии, что увеличит требования на кол-во накопителя, а хранить ссылку на оригинальную картинку + некоторый контекст для её изменения.

### Заместитель (Proxy)

Предназначен для предоставления объекта, который контролирует доступ к другому объекту, перехватывая все вызовы.

### Дополнительные материалы по теме

* [Рефакторинг.Гуру: рефакторинг и паттерны](https://refactoring.guru/ru)

## Занятие №15. Шаблоны GoF. Поведенческие

### Command

Паттерн отделяет код, которые выполняет действия и бизнес-логику от кода, который запросил команду.

О чём следует подумать:
* насколько умная команда должна быть?
* поддержка отмены и повтора операций?

### Interpreter

Книга: Компиляторы. Принципы, технологии и инструментарий. Альфред В. Ахо, Моника С. Лам, Рави Сети, Джеффри Д. Ульман.

### Iterator

Паттерн позволяет работать с любыми сколь угодно сложными структурами данных, не раскрывая их внутреннего представления.

### Mediator

Паттерн заменяет взаимодействие "все со всеми" взаимодействием "один со всеми".

### Memento

Паттерн позволяет сохранять и восстанавливать прошлые состояния объектов, не раскрывая подробностей их реализации.

### Chain of responsibilily

Связывает в цепочку объекты-получатели, а затем передаёт запрос-сообщение от одного объекта к другому до тех пор, пока не достигнет получателя, способного обработать запрос.

О чём следует подумать:
* как обрабатывать запросы, которые дошли до конца цепочки и не были обработаны
* надо ли передавать запрос дальше, если в середине цепочки данные не отвергнуты

## Занятие №16. Шаблоны GoF. Поведенческие

### Observer

Связь один (publisher) ко многим (subscriber) и возможность самостоятельной подписки/отписки.

Используется push модель (система таймеров) и pull модель (уведомления о приходе множества данных, синхронизация останова потоков).
* push - система таймеров, или передача управления ожидающим сущностям
* pull - уведомление о поступлении данных, которых м.б много, синхронизация останова потоков

Подумать:
* update д.б. лёгкими
* update не должны совершать тяжёлых действий
* какая часть кода должна вызывать notify(), setter-ы или какие-то своеобразные транзакции?
* внимательно следить за тем удаляют ли себя подписчики из списка во время работы update?

### State

Изменение внешнего поведения объекта в зависимости от внутреннего состояния.

Паттерн finite_state_machine в libosmocom.

### Strategy

Изменение взаимозаменяемых алгоритмов на лету.

Подумать:
* про передачу контекста (может быть сложно переключать стратегии в рантайме)
* стратегия как параметр шаблона
* стратегия по умолчанию?

См. `std::search` и разные алгоритмы поиска в строках.

### Template method

Выполнение алгоритма со специализацией отдельных частей.

Подумать:
* шаблонный метод не надо делать виртуальным (его нельзя менять)
* операции можно замещать, но нельзя использовать (virtual private)
* не надо делать слишком много отдельных частей

### Visitor

Добавление новых действий над объектами, не изменяя объекты.

Приём двойной диспетчеризации позволяет компилятору указать на ошибки.

Подумать:
* использовать ли перегрузку для метдов visit? или называть их по разному
* кто должен обходить контейнер с объектами?
* посетитеть может хранить информацию

См. `std::variant` и `std::visitor`.

## Занятие №19. Бонус. Красно-чёрные деревья

Сложность операций над структурами данных: https://habr.com/ru/post/188010/

Считаем уникальные элементы в массиве:
* сортировка массива и проход: O(NlogN)+N
* добавить элементы в set и посмотреть размер: O(NlogN)
* использовать unordered_set: O(N)

### B-деревья

Ориентированы на хранение данных во внешних запоминающих устройствах, у которых доступ к данным очень дорог. 

### Красно-чёрные деревья

Выполняются следующие свойства:
* Каждый узел промаркирован красным или чёрным цветом
* Корень и листья дерева — чёрные
* У красного узла родительский узел — чёрный
* Все простые пути из любого узла x до листьев содержат одинаковое количество чёрных узлов
* Чёрный узел может иметь чёрного родителя

Доказательство балансирования КЧ-деревьев: https://neerc.ifmo.ru/wiki/index.php?title=Красно-черное_дерево

Показательные задачи:
* Задача 1496 (timus)
* Задача 1196 (timus)
* ещё можно поискать что-нибудь здесь: https://codeforces.com/problemset?tags=trees

Есть библиотека Google Benchmark (типа Google Test), но на проверку сложности выполнения.

## Занятие №20. Boost. Разное

Множество библиотек. Множество авторов. Множество code-style. Различные цели и назначения. Быстро расширяется.

Можно собрать только нужное. Много вариантов настроек. Значительная часть библиотек header-only.

https://www.boost.org/

* boost::bimap
* boost::multi_index
* boost::circular_buffer
* boost::fusion
* boost::mpl
* boost::hana
* boost::program_options
* boost Test
* boost::format

## Занятие №21. Boost. Паттерны проектирования

* boost::fusion - std::tuple является гетерогенной структурой, т.е. хранит данные разных типов, в отличие от гомогенных stl-контейнеров. boost::fusion позволяет итерироваться по std::tuple, работать с данными с помощью boost::fusion:for_each
* boost:factory
* boost::signals - без поддержки многопоточности
* boost::signals2 - с поддержкой многопоточности
* boost::flyweight
* boost::interprocess

