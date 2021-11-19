# cpp_otus

Данный репозиторий хранит выполненные домашние задания по курсу C++ Developer Professional (Otus' course).

К курсу я подключился 2021-10-19, в то время как старт занятий состоялся 2021-09-22 и у группы к этому моменту уже была фора в 1 месяц, приходится догонять группу и форсировать сдачу домашних заданий.

Ради интереса и повышения индивидуальной сложности было принято решение выполнять работу в одном едином проекте *cpp_otus*, в одном едином репозитории, где практические занятия и домашние задания располагаются в отдельных директориях. А для того, чтобы заложить фундамент к тем занятиям, которые ещё предстоят и привнесут дополнительные зависимости, - в проект добавлена гибкость в части касающейся выбора собираемых проектов. В частности, запуск `cmake -DSOLUTION=lesson01 ..` соберёт все проекты 1го урока; запуск `cmake -DTARGETS="homework01 lesson01a" ..` соберёт только указанные проекты, а `cmake ..` будет собирать все проекты, всех уроков (не следует забывать о сбросе заданных настроек с помощью `cmake -USOLUTION -UTARGETS ..` в случае необходимости).

Для подготовки рабочего окружения следует установить программы и выполнить действия (раздел будет дополняться):

```bash
# основной набор программ
sudo apt install git cmake build-essential clang
# компиляторы, поддерживающие C++20
sudo update-alternatives --remove-all c++
sudo update-alternatives --remove-all cc
sudo apt install g++-10 gcc-10
sudo update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-9 10
sudo update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-10 20
sudo update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++-10 30
sudo update-alternatives --install /usr/bin/cc cc /usr/bin/gcc-9 10
sudo update-alternatives --install /usr/bin/cc cc /usr/bin/gcc-10 20
sudo update-alternatives --install /usr/bin/cc cc /usr/bin/clang-10 30
# выбираем компилятор c/c++ который нравится больше
sudo update-alternatives --config c++
sudo update-alternatives --config cc
c++ --version && cc --version

# дополнительный набор программ
sudo apt install meld emacs-nox

# получение необходимых файлов
mkdir ~/cpp_otus && cd ~/cpp_otus
git clone --origin=github https://github.com/Qandra-Si/cpp_otus.git .
# запускаем сборку проекта
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
ctest
```

## Занятие №1. Система сборки: build, test and deploy

Практические занятия, использующие систему сборки cmake находятся в директориях `lesson01`, `lesson01a`, `lesson01b`. При этом корневой проект *cpp_otus* является общим для всех включённых в него target-ов, выступающих в роли отдельных программ/подпроектов. Выполненная 1я часть домашнего задания находится в директории `homework01`, к которому прилагается *CI/CD* скрипт для github `run-tests.yml` для прогона тестов и скрипт сборки релиза `release-homework01.yml`. Выполненная 2я часть домашнего задания в директории `homework01a`, которая собирается с помощью SOLUTION=ip_filter по инструкции приведённой ниже. Сборка релиза по 2й части домашнего задания выполняется с помощью скрипта `release-homework01a.yml`.

Для сборки проектов 1го занятия следует выполнить:

```bash
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release -DSOLUTION=lesson01 ..
cmake --build . --config Release
ctest -C Release
cmake --build . --config Release --target package
```

Внимание! Кроме всего прочего, в скрипте `release-homework01.yml` задан параметр SOLUTION с тем, чтобы когда я поломаю проект в ходе работы над следующими домашними заданиями (если и вдруг), у учителя осталась бы возможность проверить мою работу в любое удобное ему время. Да,... я понимаю, что можно данный вопрос решать с помощью git branches, и изолироваться от последующих изменений, однако такая "изоляция" менее жизненна и малоинтересна - пусть проект живёт и развёртывается в общих условиях.

Для проверки сборки инсталляционного deb-пакета следует выполнить:

```bash
dpkg --info cpp_otus-0.10.21-Linux.deb | grep 'Version'
# Version: 0.10.21
sudo dpkg -i cpp_otus-0.10.21-Linux.deb
helloworld_cli
# build 0.10.21
# Hello, World!
sudo apt purge cpp_otus
```

Сборка инсталляционного deb-пакета выполняется автоматически по правилам, описанным в `release-homework01.yml` с помощью GitHub Actions. Для того, чтобы в сборку релиза попало всё нужное и исчезло всё ненужное (например мои упражнения по теме занятия), пользуемся параметром TARGETS который ссылается только на `homework01`. Настройки cpack выполнены в корневом CMakeLists.txt, с тем чтобы впредь не возвращаться к этой теме, когда они понадобятся снова.

Для сборки 2й части 1го домашнего задания следует выполнить:

```bash
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release -DSOLUTION=ip_filter ..
cmake --build . --config Release
ctest -C Release
cmake --build . --config Release --target package
cat bin/Release/ip_filter.tsv | bin/Release/cpp_otus_homework01a | md5sum
# 24e7a7b2270daee89c64d3ca5fb3da1a  -
```

Ремарка к выполненному заданию: От хранения строковых представления IPv4 адресов было решено отказаться сразу, в противовес с работой с числами. Строковое представление адреса парсится только на входе и выводится как результат. Во первых: это удобнее, во вторых: быстрее. Более того, октеты адреса располагаются в той же памяти, что и uint32_t число (велосипед не изобретаем, ядро линукса устроено так же). Но при использовании операций сравнения над этими uint32_t значениями, мы можем столкнуться с проблемой когда порядок октет в числе либо big-endian, либо little-endian, так что надо уметь детектировать порядок октет в числе на целевой платформе, и иметь две реализации сортировок. Далее тривиально - все условия вывода записываются as is и несложным образом проверяем ячейки памяти, где располагаются октеты IPv4 адресов.

Проверка результата работы программы в Windows может быть выполнена с помощью утилиты `sed`, которая может заменить Windows new line (CRLF) на Unit new line (LF). Подобная замена программным способом (в коде программы) кажется не вполне преемлема, поскольку изменение способа вывода приведёт к проблемам при работе с файлом в других приложениях. Поэтому объединим запуск утилиты `md5sum` с запуском утилиты `sed`.

```batch
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release -DSOLUTION=ip_filter ..
cmake --build . --config Release
ctest -C Release
cmake --build . --config Release --target package
bin/Release/cpp_otus_homework01a < cat bin/Release/ip_filter.tsv > crlf.txt
sed "s/\r$//" crlf.txt > lf.txt
md5sum.exe lf.txt
@rem 24e7a7b2270daee89c64d3ca5fb3da1a *lf.txt
```

### Сборка gtest в Linux вручную

Зачем собирать gtest вручную? Когда можно воспользоваться официальным мануалом:

```bash
sudo apt install libgtest-dev
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp lib*.a /usr/lib
```

Но, конечно, из спортивного интереса можно собрать и присоединённый extern-ом репозиторий gtest-а. Для этого выполняем следующие действия:

```bash
git submodule update --init --recursive
cd import/gtest
git checkout release-1.10.0
mkdir googletest/build && cd googletest/build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
cp -r lib ..
cd ..
GTEST_ROOT=$(pwd)
```

Собирать библиотеку можно было бы прямо в корень, так как сказано в мануале, и от копирования директории `lib` на уровень ниже можно было-бы отказаться, но в этом случае директория с исходными кодами была бы замусорена продуктами работы CMake.

### Сборка gtest в Windows вручную

Поскольку скрипт FindGTest.cmake из стандартной поставки cmake выполняет поиск .lib файлов в каталоге `googletest/msvc/gtest-md` и `googletest/msvc/gtest`, где эти файлы могут оказаться только при сборке студийного проекта (без использования cmake), расположенного там-же и не обновлявшегося уже давно... из терминала собрать gtest в Windows не допиливая cmake напильником сейчас проблематично. Поэтому собираем последнюю версию gtest из терминала и подсовываем продукты сборки в директории, где они будут найдены cmake.

```batch
git submodule update --init --recursive
cd import\gtest
git checkout release-1.10.0
mkdir googletest\build && cd googletest\build
chcp 1251
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"
cmake -G "Visual Studio 16 2019" -A Win32 -Dgtest_force_shared_crt=ON ..
cmake --build . --config Debug
cmake --build . --config Release
mkdir ..\msvc\gtest-md\Debug && mkdir ..\msvc\gtest-md\Release
xcopy /F /R /Y lib\Debug\*.lib ..\msvc\gtest-md\Debug
xcopy /F /R /Y lib\Release\*.lib ..\msvc\gtest-md\Release
cmake -Dgtest_force_shared_crt=OFF ..
cmake --build . --config Debug
cmake --build . --config Release
mkdir ..\msvc\gtest\Debug && mkdir ..\msvc\gtest\Release
xcopy /F /R /Y lib\Debug\*.lib ..\msvc\gtest\Debug
xcopy /F /R /Y lib\Release\*.lib ..\msvc\gtest\Release
@rem Бинго! теперь можно пользоваться find_package(GTest) указав GTEST_ROOT
cd ..
set GTEST_ROOT=%cd%
echo GTEST_ROOT=%GTEST_ROOT%
```

### Сборка gtest в GitHub Actions

Исследовал и опробовал два варианта сборки gtest-а на сервере сборок GitHub Actions.

Первый способ: установка готового пакета из репозитория, его сборка и инсталляция в Linux.

```yml
- name: Install GTest from Repository
  run: sudo apt-get install libgtest-dev && cd /usr/src/gtest && sudo cmake CMakeLists.txt && sudo make && sudo cp /usr/src/gtest/lib/*.a /usr/lib && sudo ln -s /usr/lib/libgtest.a /usr/local/lib/libgtest.a && sudo ln -s /usr/lib/libgtest_main.a /usr/local/lib/libgtest_main.a
```

Второй способ: загрузка gtest как git-субмодуля текущего проекта, его сборка и настройка GTEST_ROOT-переменной.

```yml
- name: Build GTest as Embedded Git Submodule
  run: cd ${{env.GTEST_ROOT}} && git checkout release-1.10.0 && cmake -DCMAKE_BUILD_TYPE=${{env.BUILD_TYPE}} . && make
```

Оба способа рабочих и find_package(GTest) завершается успешно, но второй способ мне нравится больше в силу своей большей гибкости - в случае необходимости можно переключиться на любой подходящий релиз библиотеки.

### Сборка инсталляционного пакета в Windows

Для того, чтобы собрать инсталляционный пакет для Windows, можно воспользоваться `CPack Generator` `NSIS`, инсталлятор которого необходимо заранее скачать и установить [https://nsis.sourceforge.io/Main_Page](по этой ссылке).

Сборка инсталлятора осуществляется:

```batch
chcp 1251
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"
mkdir build && cd build
cmake -G "Visual Studio 16 2019" -A Win32 ..
cmake --build . --config Release --target package
@rem в результате будет получен инсталляционный файл cpp_otus-0.10.21-win32.exe
```

### Получение названия инсталляционного пакета в GinHub Actions

Если при формировании названия пакета в CMake скрипте используется какое-либо кодунство, например:

```CMake
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
if (DEFINED SOLUTION)
  set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH}-${SOLUTION_LOWERCASE})
else()
  set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
endif()
```

То после выполнения скрипта мы можем получить как пакет с названием `cpp_otus-0.10.22-ip_filter-win32.exe`, так и например `cpp_otus-0.10.20-helloworld_cli-Linux.deb`, который потребуется передать в workflow-скрипт GitHub Actions. Такую передачу можно сделать с помощью файла в файловой workspace-системе runner-а:

```CMake
if (NOT WIN32)
  file(WRITE ${CMAKE_BINARY_DIR}/_generated_/package_file_name.txt "cpp_otus-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-Linux.deb")
endif()
```

Настройки (переменные) GinHub Actions хранятся в текстовом файле в формате `name=value`, и для того, чтобы сохранить и передать значение environment переменной на следующий шаг, - достаточно просто сохранить пару name=value в файл `$env:GITHUB_ENV`.

```yml
- name: Make Package
  run: cmake --build ${{github.workspace}}/build --target package

- name: Get Package FileName
  run: |
       if ("${ { runner.os } }" STREQUAL "Windows")
         $env:PACKAGE_FILE_NAME = Get-Content ${{github.workspace}}\build\CPackConfig.cmake | where { $_.Contains('set(CPACK_PACKAGE_FILE_NAME "') } | ForEach-Object { $_.Trim('")').Trim('set(CPACK_PACKAGE_FILE_NAME "') }
         echo "package_file_name=$env:PACKAGE_FILE_NAME" | Out-File -FilePath $env:GITHUB_ENV -Encoding utf-8 -Append
       elseif ("${ { runner.os } }" STREQUAL "Linux")
         PACKAGE_FILE_NAME=$(cat ${{github.workspace}}/build/_generated_/package_file_name.txt)
         echo "package_file_name=$PACKAGE_FILE_NAME" >> $GITHUB_ENV
       endif()

- name: Upload Release Asset
  uses: actions/upload-release-asset@v1
  with:
    # Получаем название файла с инсталляционным пакетом
    asset_path: ${{github.workspace}}/build/${{ env.package_file_name }}
    # Переименовываем пакет всоответствии созданным tag-ом в git-е
    asset_name: cpp_otus-${{ github.ref }}-Linux.deb
```

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

Практические занятия по 2му занятию находятся в директориях `lesson03`, `lesson03a`,... При этом корневой проект *cpp_otus* является общим для всех включённых в него target-ов, выступающих в роли отдельных программ/подпроектов. Выполненная 1я часть домашнего задания находится в директории `homework03`. Выполненная 2я часть домашнего задания в директориях `homework03a` и `homework03b`. Сборка упражнений выполняется с помощью cmake-параметра TARGETS по инструкции приведённой ниже.

Для сборки проектов 3го занятия следует выполнить:

```bash
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release -DTARGETS="homework03 homework03a homework03b" ..
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
bin/Debug/cpp_otus_lesson04
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
bin/Debug/cpp_otus_lesson05
```

### unique_ptr

Позволяет безраздельно владеть объектом. Не разрешает копировать (допускает только перемещение). Размер зависит от пользовательского deleter-а. Без особой логики удаления издержки чаще всего отсутствуют. Конструкция `std::make_unique` делает код менее громоздким, плюс предохраняет от exceptions.

### shared_ptr

Позволяет копировать указатели с разделением владения. Копирование небесплатное. Внутри shared_ptr всегда два указателя. Конструкция `std::make_shared` предпочтительнее вызова конструктора. Непотокобезопасный (потокобезопасен только относительно контрольного блока, и то не всегда). Можно создать из unique_ptr. Приводит к фрагментации памяти и вследствии "разбрасывания" дескрипторов данных по памяти размещается "далеко" и не может не попадать в один процессорный кеш, - потому рекомендуется использовать make_shared, для минимизации фрагментации.

### weak_ptr

Следует аккуратно пользоваться `std::make_shared` вместе с `weak_ptr`, т.к. make_shared создаёт единый кусок в памяти с указателями и на блок данных, и на контрольный блок, и эта память отдельно друг от друга удалиться не может. Возможна утечка памяти, если weak_ptr будет удерживать контрольные блоки при перекрёстных ссылках (см. пример в `lesson05`).

### enable_shared_from_this

Нельзя вызывать в конструкторе - пока объект не создан, нет this, нечего вернуть. Нельзя возвращать shared_from_this для объекта класса, для которого не было создано shared_ptr - произойдёт исключение.

### Исключения к конструкторах

Внимание! на 30 минуте 5го занятия прозвучала спорная фраза касающаяся вызова деструкторов в случае, если в конструкторе произошло исключение. В стандарте на эту тему есть чёткая формулировка, как именно должна себя вести программа, см. подробности тут [http://eel.is/c++draft/except.ctor#3]([except.ctor]). Пример, демонстрирующий пояснение к вызову деструкторов находится в директории `lesson05_mleak_ctor`, который можон собрать с помощью параметра `-DTARGETS=lesson05_mleak_ctor`.

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

### Домашнее задание

Для сборки домашнего задания по аллокаторам следует выполнить:

```bash
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release -DSOLUTION=allocators ..
cmake --build . --config Release
ctest -C Release
cmake --build . --config Release --target package
cat bin/Release/ip_filter.tsv | bin/Release/cpp_otus_homework6_alloc | md5sum
# 24e7a7b2270daee89c64d3ca5fb3da1a  -
```

homework6_alloc

### Дополнительные материалы по теме

* [CPP Reference: allocator_traits](https://en.cppreference.com/w/cpp/memory/allocator_traits)
* [CPP Reference: polymorphic_allocator](https://en.cppreference.com/w/cpp/memory/polymorphic_allocator)
* [Исходный код std::allocator в libstdc++ gcc](https://code.woboq.org/gcc/libstdc++-v3/include/bits/allocator.h.html)
* [Реализация custom allocator-а](https://docs.ros.org/en/foxy/Tutorials/Allocator-Template-Tutorial.html) пример с пояснениями

## Занятие №7. Шаблонная магия. Метафункции, рекурсия и условия, SFINAE

TODO: в ходе работы над домашним заданием lesson11 пришлось вернуть к пропущенным урокам по SFINAE.

Для сборки примеров 7го урока следует выполнить команды:

```bash
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release -DSOLUTION=lesson07 ..
cmake --build . --config Release
```

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

## Занятие №11. Принципы проектирования ПО. SoC, DRY, YAGNI, KISS, TDA, LoD, SOLID. Язык UML

Доклад на тему что такое архитектура ПО, зачем она нужна, что включают себя критерии оценки успешности проектирования архитектуры ПО. В этой связи были изложены основные принципы проектирования ПО.

В лекции не понравилось то, что примеры, статьи и дополнительные материалы на изучение даются на языке Java. В целом, конечно понятно, в том числе учитывая и мой опыт работы на Java. Однако создаётся впечатление, что мир C++ можно легко описать миром Java. Однако это не так. Это разные миры, со своими подходами, разными парадигмами, обношениями и реализациями одних и тех же шаблонов проектирования и т.д. и т.п. "Страхи" программистов C++ учтены и заложены в основу языка Java, поэтому на мой взгляд рассматривать подходы и проектирование C++ программ по примерам на языке Java по меньшей мере ущербно (если такое проектирование распространяется на крупные проекты, а не только на теологические упражнения в стиле `class Lion extends Animal`... упс `class lion_t : public animal_t` конечно же).

### Олимпиадные задачи на acm.timus.ru

Для выполнения домашних заданий было предложено подключиться к роботу на [acm.timus.ru](https://acm.timus.ru/). Была создана учётная запись, и выполнена часть упражнений, результаты автоматически опуликованы по этой ссылке [Карта решенных задач](https://acm.timus.ru/author.aspx?id=323296).

Решения задач находятся в директории `src/homework11/num????`. Для добавления новых программ в директорию `homework11` изменять корневые cmake-файлы не требуется, достаточно добавить новую директорию `num????`, все target-ы автоматически обнаруживаются в своих директориях и подключаются к проекту. Для локальной сборки решений задач следует выполнить:

```bash
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release -DSOLUTION=timus_acm ..
cmake --build . --config Release
time (cat ../src/homework11/num1001/sample.txt | bin/Release/cpp_otus_acm_num1001)
# 2297.0716
# 936297014.1164
# 0.0000
# 37.7757
#
# real  0m0,006s
# user  0m0,004s
# sys   0m0,005s
```

Пока выполнялась работа над решением задач, сайт acm.timus.ru неодонкратно ложился (судя по форумам, в Уральском федеральном университете в ВЦ периодически выключают электроэнергию). Было найдено зеркало сайта с условиями задач здесь: [timus.online](https://timus.online). Вот только сортировка задач по сложонсти там иная,... но я удерживал открытой закладку на основном сайте, где порядок задач был таким: 1001, 1000, 1293, 1785, 1409, 2021, 1877, 1264, 2001, 1787, 1820, 2066, 1197, 2100, 1880.

### Дополнительные материалы по теме

* [Doxygen](https://www.doxygen.nl/)
* [Рисовалка UML](https://plantuml.com/ru/)
* [Конвертер документации](https://pandoc.org/)
* [Разъяснения по UML диаграммам](http://book.uml3.ru/sec_1_4)
* [Пояснения по принципу SOLID](https://medium.com/webbdev/solid-4ffc018077da)
* [Подходы к проектированию: как два программиста хел пекли](https://habr.com/ru/post/153225/) шуточная статья, но поучительная
* [Техники codereview](https://habr.com/ru/post/340550/) шуточная статья, но поучительная
* [Google Style Guides](https://google.github.io/styleguide/) и оттуда же [Google Style Guildes: C++](https://google.github.io/styleguide/cppguide.html)

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

