# cpp_otus
[![GitHub tag](https://img.shields.io/github/v/tag/Qandra-Si/cpp_otus)](https://github.com/Qandra-Si/cpp_otus/releases)
[![GitHub license](https://img.shields.io/badge/license-beer--ware-44897a)](https://github.com/Qandra-Si/cpp_otus/blob/main/LICENSE)
[![GitHub issues](https://img.shields.io/github/issues/Qandra-Si/cpp_otus)](https://github.com/Qandra-Si/cpp_otus/issues)
[![GitHub forks](https://img.shields.io/github/forks/Qandra-Si/cpp_otus)](https://github.com/Qandra-Si/cpp_otus/network)
[![GitHub stars](https://img.shields.io/github/stars/Qandra-Si/cpp_otus)](https://github.com/Qandra-Si/cpp_otus/stargazers)

Данный репозиторий хранит выполненные домашние задания по курсу C++ Developer Professional (Otus' course), а также [конспект лекций здесь](https://github.com/Qandra-Si/cpp_otus/blob/homework07/NOTES.md) (краткий на память).

Отчёты о выполненных домашних заданиях:
* [Домашнее задание №1](#домашнее-задание-1-система-сборки-build-test-and-deploy)
* [Домашнее задание №2](#домашнее-задание-2-аллокаторы-в-c)
* [Домашнее задание №3](#домашнее-задание-3-принципы-проектирования-по-soc-dry-yagni-kiss-tda-lod-solid-язык-uml)
* [Домашнее задание №4](#домашнее-задание-4-шаблоны-gof-порождающие)
* [Домашнее задание №5](#домашнее-задание-5-шаблоны-gof-поведенческие-command-interpreter-iterator-mediator-memento-chain-of-responsibilily)

К курсу я подключился 2021-10-19, в то время как старт занятий состоялся 2021-09-22 и у группы к этому моменту уже была фора в 1 месяц, приходится догонять группу и форсировать сдачу домашних заданий.

Ради интереса и повышения индивидуальной сложности было принято решение выполнять работу в одном едином проекте *cpp_otus*, в одном едином репозитории, где практические занятия и домашние задания располагаются в отдельных директориях. А для того, чтобы заложить фундамент к тем занятиям, которые ещё предстоят и привнесут дополнительные зависимости, - в проект добавлена гибкость в части касающейся выбора собираемых проектов. В частности, запуск `cmake -DSOLUTION=lesson01 ..` соберёт все проекты 1го урока; запуск `cmake -DTARGETS="homework01 lesson01_cmake_basic" ..` соберёт только указанные проекты, а `cmake ..` будет собирать все проекты, всех уроков (не следует забывать о сбросе заданных настроек с помощью `cmake -USOLUTION -UTARGETS ..` в случае необходимости).

Для подготовки рабочего окружения следует установить программы и выполнить действия (раздел будет дополняться):

```bash
# основной набор программ
sudo apt install git cmake build-essential clang doxygen
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

# Документы стандартов C++
* [Обновляемый черновик стандарта](http://eel.is/c++draft/) в html формате
* [Обновляемый список черновиков стандарта](https://stackoverflow.com/a/4653479), ссылки на черновики в .pdf формате разных версий




# Домашнее задание №1. Система сборки: build, test and deploy

Практические занятия, использующие систему сборки cmake находятся в директориях [lesson01_cmake_basic](/src/lesson01_cmake_basic/), [lesson01_cmake_generator](/src/lesson01_cmake_generator/), [lesson01_cmake_lib](/src/lesson01_cmake_lib/). При этом корневой проект *cpp_otus* является общим для всех включённых в него target-ов, выступающих в роли отдельных программ/подпроектов. Выполненная 1я часть домашнего задания находится в директории [homework01](/src/homework01/), к которому прилагается **CI/CD** скрипт для github [run-tests.yml](/.github/workflows/run-tests.yml) для прогона тестов и скрипт сборки релиза [release-homework01.yml](/.github/workflows/release-homework01.yml). Выполненная 2я часть домашнего задания в директории [homework01a](src/homework01a), которая собирается с помощью `SOLUTION=ip_filter` по инструкции приведённой ниже. Сборка релиза по 2й части домашнего задания выполняется с помощью скрипта [release-homework01a.yml](/.github/workflows/release-homework01a.yml).

Для сборки проектов 1го занятия следует выполнить:

```bash
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release -DSOLUTION=lesson01 ..
cmake --build . --config Release
ctest -C Release
cmake --build . --config Release --target package
```

Внимание! Кроме всего прочего, в скрипте [release-homework01.yml](/.github/workflows/release-homework01.yml) задан параметр SOLUTION с тем, чтобы когда я поломаю проект в ходе работы над следующими домашними заданиями (если и вдруг), у учителя осталась бы возможность проверить мою работу в любое удобное ему время. Да,... я понимаю, что можно данный вопрос решать с помощью git branches, и изолироваться от последующих изменений, однако такая "изоляция" менее жизненна и малоинтересна - пусть проект живёт и развёртывается в общих условиях.

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

Сборка инсталляционного deb-пакета выполняется автоматически по правилам, описанным в [release-homework01.yml](/.github/workflows/release-homework01.yml) с помощью GitHub Actions. Для того, чтобы в сборку релиза попало всё нужное и исчезло всё ненужное (например мои упражнения по теме занятия), пользуемся параметром TARGETS который ссылается только на `TARGETS=homework01`. Настройки cpack выполнены в корневом [CMakeLists.txt](CMakeLists.txt), с тем чтобы впредь не возвращаться к этой теме, когда они понадобятся снова.

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
bin\Release\cpp_otus_homework01a < bin\Release\ip_filter.tsv > crlf.txt
sed "s/\r$//" crlf.txt > lf.txt
md5sum.exe lf.txt
@rem 24e7a7b2270daee89c64d3ca5fb3da1a *lf.txt
```

## Сборка gtest в Linux вручную

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

## Сборка gtest в Windows вручную

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

## Сборка gtest в GitHub Actions

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

## Сборка инсталляционного пакета в Windows

Для того, чтобы собрать инсталляционный пакет для Windows, можно воспользоваться `CPack Generator` `NSIS`, инсталлятор которого необходимо заранее скачать и установить [по этой ссылке](https://nsis.sourceforge.io/Main_Page).

Сборка инсталлятора осуществляется:

```batch
chcp 1251
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"
mkdir build && cd build
cmake -G "Visual Studio 16 2019" -A Win32 ..
cmake --build . --config Release --target package
@rem в результате будет получен инсталляционный файл cpp_otus-0.10.21-win32.exe
```

## Получение названия инсталляционного пакета в GinHub Actions

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

## Ремарка к выполненному заданию

Практические занятия по 2му занятию находятся в директориях [lesson03_deducing_types](/src/lesson03_deducing_types/), [lesson03_move](/src/lesson03_move/), [lesson03_nrvo](/src/lesson03_nrvo/),... При этом корневой проект **cpp_otus** является общим для всех включённых в него target-ов, выступающих в роли отдельных программ/подпроектов. Выполненная 1я часть домашнего задания находится в директории [homework03_deducing_types](/src/homework03_deducing_types/). Выполненная 2я часть домашнего задания в директориях [homework03_custom_tie](/src/homework03_custom_tie/) и [homework03_custom_tuple](/src/homework03_custom_tuple/). Сборка упражнений выполняется с помощью cmake-параметра TARGETS по инструкции приведённой ниже.

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




# Домашнее задание №2. Аллокаторы в C++

## Ремарка к выполненному заданию

Для выполнения задания по разработке собственного аллокатора, было принято решение повысить индивидуальную сложность и реализовать fake_heap алгоритм с методами `malloc`, `calloc`, `free` и `realloc`. Первые три упомянутых метода были реализованы в классе `core::fake_heap_t` в библиотеке core здесь [fake_heap.cpp](/src/core/src/fake_heap.cpp). Алгоритм работы фейковой кучи основан на единовременном выделении памяти для использования и выделения блоков памяти линейно, приращением курсора без сборщика мусора, пока память не кончится, таким образом метод `free` не выполнял никакую работу. По заданию требовалось обеспечить "...попытку выделить большее число элементов считать ошибкой" - в результате чего генерировался `std::bad_alloc`. Для выполнения пункта "...опционально реализовать расширяемость аллокатора..." в фейковую кучу был добавлен метод [enable_safe_mode](/src/core/include/core/fake_heap.h#L90), который переключал алгоритм на использование кучи, соответственно [disable_fake_mode](/src/core/include/core/fake_heap.h#L92) отключает безопасный режим и снова приводит к выбрасыванию исключений, если выделенного блока памяти недостаточно. Для того, чтобы "фейковая куча" имела возможность работать с кучей как методами `malloc` и `calloc`, так и `free` - в класс [core::fake_heap_t](/src/core/src/fake_heap.cpp#L103) была добавлена логика проверки достаточного количества памяти и определения в какой куче выполнялось выделение, с тем чтобы корректно выполнить освобождение.

Релизация метода `realloc` была также выполнена, из "спортивного интереса", но в данном случае пришлось в фейковой куче дополнительно размещать информацию о кол-ве октет для которых выполнялось выделение памяти. С тем, чтобы учесть потребность алгоритмов часто выполняющих realloc (например protoс или asn1c и т.п.) и выполнить "быструю" реаллокацию в пределах ранее выделенного блока памяти, было введено понятие `chunk-ов` в пределах которых блоки памяти могут без лишних накладных расходов выполнять `realloc`, - данные не перемещаются, а остаются в пределах ранее выделенного куска памяти. В том случае, если `chunk` размещён в конце используемой памяти фейковой кучи, - выполняется его расширение, также без каких-либо дополнительных накладных расходов. Если chunk невозможно расширить, то он переносится либо на новое место в фейковой куче, либо в случае `safe_mode` в кучу, разумеется с копированием исходных данных. Алгоритм представлен в методе [fake_realloc_heap_t::realloc](/src/core/src/fake_heap.cpp#L181). Для того, чтобы алгоритмы `malloc`, `calloc` и `free` не обременялись лишними накладными расходами в случаях, когда `realloc` использовать не планируется (как, например, в домашнем задании №2), алгоритмы были реализованы в одном классе, а realloc в классе-потомке.

Аллокатор `fake_heap_allocator_t<T>` также реализован в билиотеке corе в [fake_heap_allocator.h](/src/core/include/core/fake_heap_allocator.h). Для повышения индивидуальной сложности была выполнена реализация statefull allocator (в противовес stateless allocator, как zero size class, который работает с каким-либо memory pool singleton объектом), таковая реализация в рамках домашнего задания представлялась "скучной" :-) Однако пришлось повозиться и подробно разобраться с использованием аллокатора в различных режимах, в особенности когда выполняется rebind. Пришлось вычитывать стандарт C++ в части касающейся требований, предъявляемых к аллокаторам : [требования стандарта C++ к аллокаторам ](https://eel.is/c++draft/allocator.requirements) и [определение аллокатора в C++](http://eel.is/c++draft/default.allocator).

Работа с фейковой кучей и работа с фейковым аллокатором протестирована с помощью gtest и находится в файле [test_fake_heap.cpp](/src/core/test/test_fake_heap.cpp). Особую сложность вызвала модель [test_fake_heap.statefull_allocator](/src/core/test/test_fake_heap.cpp#L396) которая работает с аллокатором, как если бы он был stateless. А также особенность того, что контейнеры размещают в памяти заранее недетермированные блоки памяти (и стандарт C++ их в этом не обязывает никак), в этой связи работа аллокатора в msvc и/или gcc, или clang различаются, также различается объёмы требуемой для работы контейнеров памяти в x86/x64 режимах, модель в тесте [test_fake_heap.map_allocator](/src/core/test/test_fake_heap.cpp#L497) тестирует эту особенность и проверяет работу алгоритма в условиях, например msvc-x86 и gcc-x64.

*Пункт задания "опционально реализовать поэлементное освобождение" для фейковой кучи недостижим, либо должен был бы быть выполнен через хранение информации о выделенных блоках, что повлекло бы лишние накладные расходы (и не соответсвовало бы идеологии быстрой фейковой кучи), и этот пункт был бы полумерой, так как потребовал бы реализовать страничную организацию кучи и дефрагментацию на уровне страниц... это прям как-то слишком круто для домашнего задания :-) Поэтому пункт не выполнен и фейковая куча размещает данные, пока не кончится выделенный блок.*

В качестве контейнера для второй части домашнего задания был выбран и реализован кольцевой буфер `ringbuffer_t<T,Size,Alloc>`, см. файл [ringbuffer.h](/src/core/include/core/ringbuffer.h). Для повышения индивидуальной сложности - буфер сделан потокобезопасным на базе оптимистического механизма блокирования с `std::atomic` индексами головы/хвоста. Кольцевой буфер имеет методы push(T) и pop(T), которые возвращают признаки успешного выполнения операции. Количество элементов в кольцевом буфере лимитировано настройкой шаблона, для корректной инициализации кольцевого буфера, в буфер влезает не больше Size-1 элементов. Шаблон кольцевого буфера позволяет переопределить используемый аллокатор (по умолчанию `std::allocator`) и вот тут пришлось повозиться с rebind-механизмом в аллокаторах (хорошо, что я разобрался с ним на предыдущем этапе работы), так что подключение аллокатора прошло без лишних проблем и непонимания.

С итератором для контейнера "кольцевой буфер" `ringbuffer_iterator_t<T,Size>` пришлось повозиться. Во первых потому, что [const_iterator](/src/core/include/core/ringbuffer.h#L192) для кольцевого буфера невозможен, элемент буфера либо считывается из буфера, что выполняет передвижение хвоста, либо (неменяя контейнер) операцию считывания выполнить нельзя. Также сложностью оказалось и то, что двигается не только хвост при чтении, но и голова с помощью методов push(), в том числе и из "соседних" потоков. Так что пришлось повозиться над алгоритмом [operator==](/src/core/include/core/ringbuffer.h#L120) и учесть ту особенность, что пока у нас в цикле двигается итератор, в это же время голова кольцевого буфера, т.е. `ringbuffer.end()` также двигается.

Тесты с проверками использования ringbuffer и его итератора находятся в файле [test_rindbuffer.cpp](/src/core/test/test_rindbuffer.cpp).

## Сборка домашнего задания

Приложение, выполняющее требования 2го домашнего задания находится в файле [homework06_alloc/main.cpp](/src/homework06_alloc/main.cpp), оно маленькое и простое... более информативны, в части касающейся исследования работы алгоритмов, могут оказаться реализованные тесты.

Для сборки домашнего задания по аллокаторам следует выполнить:

```bash
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release -DGTEST_ROOT=%PWD/../import/gtest/googletest -DSOLUTION=allocators ..
cmake --build . --config Release
ctest -C Release
cmake --build . --config Release --target package
bin/Release/cpp_otus_homework6_alloc
```

Инсталляционный пакет с выполненным домашним заданием №2 не публикуется (не было такого требования), но может быть собран по инструкции выше.

## Doxygen документация

Документацию с описаниями классов/шаблонов, сгенерированную с помощью doxygen-генератора, см. по ссылке: https://qandra-si.github.io/cpp_otus/




# Домашнее задание №3. Принципы проектирования ПО. SoC, DRY, YAGNI, KISS, TDA, LoD, SOLID. Язык UML

## Олимпиадные задачи на acm.timus.ru

Для выполнения домашних заданий было предложено подключиться к роботу на [acm.timus.ru](https://acm.timus.ru/). Была создана учётная запись, и выполнена часть упражнений, результаты автоматически опуликованы по этой ссылке [Карта решенных задач](https://acm.timus.ru/author.aspx?id=323296).

Решения задач находятся в директории [src/timus/num????](/src/timus/). Для добавления новых программ в директорию `timus` изменять корневые cmake-файлы не требуется, достаточно добавить новую директорию `num????`, все target-ы автоматически обнаруживаются в своих директориях и подключаются к проекту. Для локальной сборки решений задач следует выполнить:

```bash
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release -DSOLUTION=timus_acm ..
cmake --build . --config Release
time (cat ../src/timus/num1001/sample.txt | bin/Release/cpp_otus_acm_num1001)
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

## Сборка домашнего задания

Практическая часть домашнего задания с применением метапрограммирования в C++ находится в файле [homework11_sfinae/main.cpp](/src/homework11_sfinae/main.cpp). Выполнена основная и дополнительная части задания (включая tuple). Использованы различные варианты использования перегрузки и приведения типов. Обработка простых типов выполнена с помощью `if constexpr (std::is_same<T,U>)`. Обработка контейнеров вынесена в отдельную обобщённую реализацию (можно было бы продолжать увеличивать кол-во if-ов, но отдельная реализация для контейнеров выглядит понятнее). Наиболее понравилось решать задачу с `std::tuple`, вывод информации на экран выполняется на рекурсивном подъёме, а ограничение рекурсии выполнено в том-же шаблонном методе.

```bash
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release -DCPP_OTUS_SKIP_TEST=TRUE -DSOLUTION=sfinae ..
cmake --build . --config Release
cmake --build . --config Release --target package
bin/Release/cpp_otus_homework11_sfinae
# на экран будут выведены следующие строки:
# 255
# 0.0
# 127.0.0.1
# 123.45.67.89.101.112.131.41
# localhost
# 192.168.0.0/16
# 2001.0db8.85a3.0000.0000.8a2e.0370.7334
# 8.8.8.8
# 33.3.14.Hello.World.!
```

## Doxygen

Более подробные сведения по сборке doxygen-документации приведены в README.md файле библиотеки [libcore](https://github.com/Qandra-Si/cpp_otus/tree/main/src/core).

Документацию с описаниями классов/шаблонов, сгенерированную с помощью doxygen-генератора, см. по ссылке: https://qandra-si.github.io/cpp_otus/




# Домашнее задание №5. Шаблоны GoF. Поведенческие. Command, Interpreter, Iterator, Mediator, Memento, Chain of responsibilily

## Олимпиадные задачи на acm.timus.ru

Учётная запись на [acm.timus.ru](https://acm.timus.ru/) была создана в ходе предыдущих домашних работ, результаты автоматически опуликованы по этой ссылке [Карта решенных задач](https://acm.timus.ru/author.aspx?id=323296).

## Ремарка к выполненному заданию

Практическая часть домашнего задания с реализацией и использованием "разреженной бесконечной матрицы" находится в файле [homework15_operators/main.cpp](/src/homework15_operators/main.cpp), а также в библиотеке libcore в файле [matrix.h](/src/core/include/core/matrix.h).

Основной функционал домашней работы заключён в реализации шаблона [matrix_t](/src/core/include/core/matrix.h#L173). Шаблон имеет две реализации, одну для N-мерной матрицы, вторую для 1-мерной матрицы. Шаблон для N-мерной матрицы предоставляет и реализует интерфейсы для работы с итераторами, с обращением к вложенными матрицам. Шаблон для 1-мерной матрицы реализует [operator[]](/src/core/include/core/matrix.h#L273) для доступа к данным разреженной матрицы. Данные матрицы хранятся в [matrix_container_t](/src/core/include/core/matrix.h#L56). Элементами являются экземпляры структур [matrix_item_t](/src/core/include/core/matrix.h#L46), которые добавляются по мере необходимости в список контейнера матрицы и представляют собой сведения об адресе ячейки и непосредственно экземпляр данных ячейки. Поиск данных в контейнере матрицы осуществляется с помощью `std::find_if` с реализованным предикатом сравнения адреса ячейки.

Вывод данных из матрицы выполнен с помощью proxy-класса [matrix_stub_t](/src/core/include/core/matrix.h#L30), который реализуя copy/direct initialization, а также перегружая оператор value_type() по умолчанию, позволяет читать данные хранимые в матрице, а также при обращении к assignment оператору выполнять редактирование данных в матрице. Экземпляр proxy-класса хранит локальную копию данных (на стеке), т.к. "ничего не знает" о местоположении данных в матрице, при обращении к данным матрицы подставляет свою копию, а при редактировании обращается с помощью полученного при инициализации callback-метода.

Каноническая форма assingment oprator-а и вывод данных из заглушки реализуются с помощью конструкций:
```cpp
// где stub - локальная копия данных
// а on_assign - метод с помощью которых "заглушка" редактирует данные
matrix_stub_t<T> operator=(const T& other) { stub = other; return on_assign(other); }
// перегруженный оператор типа данных
operator T() const { return stub; }
```

Отдельное внимание следует уделить использованию аллокаторов. Работа с ними реализована с учётом знаний, полученных в ходе 3й домашней работы. Реализация алгоритмов матрицы подразумевают два типа динамических выделений памяти: для экземпляра контейнера и для экземпляров элементов контейнера. С тем, чтобы аллокатор, переданный матрице имел возможность выделять память и инициализировать элементы соответствующего размера, - выполняется rebind аллокатора с помощью `std::allocator_traits<A>::rebind_alloc<T>` (см. строки кода ниже). Матрица [хранит аллокаторы](/src/core/include/core/matrix.h#L222), ребинд которых выполнила, с тем чтобы не терялась возможность работать со stateful-аллокаторами.

```cpp
using _Container_allocator = typename std::allocator_traits<_Alloc>::template rebind_alloc<matrix_item_t<T>>;
using _Container = matrix_container_t<T, _Container_allocator>;
using _rebind_alloc = typename std::allocator_traits<_Alloc>::template rebind_alloc<_Container>;
```

Выполнение требования "...матрица должна хранить только занятые элементы..." исключая хранение элементов со значениями по умолчанию продемонстрировано с помощью теста, использующего `fake_heap_allocator_t` аллокатор, см. тест [test_matrix.matrix_allocator](/src/core/test/test_matrix.cpp#L121). Тест инициализиует **крошечное** кол-во памяти в фейковой куче и выполняет множество размещений элементов со значениями по умолчанию, проверяя факт того, что память на размещение элементов **не была потрачена**.

## Сборка домашнего задания

Для сборки домашнего задания по операторам следует выполнить:

```bash
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release -DCPP_OTUS_SKIP_TEST=TRUE -DSOLUTION=operators ..
cmake --build . --config Release
ctest -C Release
cmake --build . --config Release --target package
bin/Release/cpp_otus_homework15_operators
# на экран будут выведены следующие строки:
# 1 0 0 0 0 0 0 8
# 0 2 0 0 0 0 7 0
# 0 0 3 0 0 6 0 0
# 0 0 0 4 5 0 0 0
# 0 0 0 4 5 0 0 0
# 0 0 3 0 0 6 0 0
# 0 2 0 0 0 0 7 0
# 1 0 0 0 0 0 0 8
# 18
# matrix[1][1] = 1
# matrix[2][2] = 2
# matrix[3][3] = 3
# matrix[4][4] = 4
# matrix[5][5] = 5
# matrix[6][6] = 6
# matrix[7][7] = 7
# matrix[8][8] = 8
# matrix[9][9] = 9
# matrix[0][9] = 9
# matrix[1][8] = 8
# matrix[2][7] = 7
# matrix[3][6] = 6
# matrix[4][5] = 5
# matrix[5][4] = 4
# matrix[6][3] = 3
# matrix[7][2] = 2
# matrix[8][1] = 1
# matrix[100][100] = 0, size = 0
# matrix[100][100] = 314, size = 1
# matrix[100][100] = 0, size = 0
# matrix[100][100] = 217, size = 1
```

Тесты и примеры использования разработанного `matrix_t`-шаблона, см. в файле [test_matrix.cpp](/src/core/test/test_matrix.cpp).

## Doxygen документация

Документацию с описаниями классов/шаблонов, сгенерированную с помощью doxygen-генератора, см. по ссылке: https://qandra-si.github.io/cpp_otus/
