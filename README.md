# cpp_otus

Данный репозиторий хранит выполненные домашние задания по курсу C++ Developer Professional (Otus' course).

К курсу я подключился 2021-10-19, в то время как старт занятий состоялся 2021-09-22 и у группы к этому моменту уже была фора в 1 месяц, приходится догонять группу и форсировать сдачу домашних заданий.

Ради интереса и повышения индивидуальной сложности было принято решение выполнять работу в одном едином проекте *cpp_otus*, в одном едином репозитории, где практические занятия и домашние задания располагаются в отдельных директориях. А для того, чтобы заложить фундамент к тем занятиям, которые ещё предстоят и привнесут дополнительные зависимости, - в проект добавлена гибкость в части касающейся выбора собираемых проектов. В частности, запуск `cmake -DSOLUTION=lesson01 ..` соберёт все проекты 1го урока; запуск `cmake -DTARGETS="homework01 lesson01a" ..` соберёт только указанные проекты, а `cmake ..` будет собирать все проекты, всех уроков (не следует забывать о сбросе заданных настроек с помощью `cmake -USOLUTION -UTARGETS ..` в случае необходимости).

Для подготовки рабочего окружения следует установить программы и выполнить действия (раздел будет дополняться):

```bash
# основной набор программ
sudo apt install git cmake build-essential clang
# компиляторы, поддерживающие C++20
sudo apt install g++-10 gcc-10
sudo update-alternatives --install /usr/bin/g++ c++ /usr/bin/g++-10 30
sudo update-alternatives --install /usr/bin/gcc cc /usr/bin/gcc-10 30
# дополнительный набор программ
sudo apt install meld emacs-nox
# выбираем компилятор c/c++ который нравится больше
sudo update-alternatives --config c++
sudo update-alternatives --config cc

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

Практические занятия, использующие систему сборки cmake находятся в директориях `lesson01`, `lesson01a`, `lesson01b`. При этом корневой проект *cpp_otus* является общим для всех включённых в него target-ов, выступающих в роли отдельных программ/подпроектов. Выполненное домашнее задание в директории `homework01`, к которому прилагается *CI/CD* скрипт для github расположенный в `.github/workflows/cmake-homework01.yml`. Выполненное домашнее задание в директории `homework01a`, которое собирается с помощью SOLUTION=ip_filter по инструкции приведённой ниже.

Для сборки проектов 1го занятия следует выполнить:

```bash
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release -DSOLUTION=lesson01 ..
cmake --build . --config Release
ctest -C Release
```

Внимание! Кроме всего прочего, в скрипте `cmake-homework01.yml` задан параметр SOLUTION с тем, чтобы когда я поломаю проект в ходе работы над следующими домашними заданиями (если и вдруг), у учителя осталась бы возможность проверить мою работу в любое удобное ему время. Да,... я понимаю, что можно данный вопрос решать с помощью git branches, и изолироваться от последующих изменений, однако такая "изоляция" менее жизненна и малоинтересна - пусть проект живёт и развёртывается в общих условиях.

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
cat bin/Release/ip_filter.tsv | bin/Release/cpp_otus_homework01a | md5sum
# 24e7a7b2270daee89c64d3ca5fb3da1a  -
```

Ремарка к выполненному заданию: От работы со строковым представлением IPv4 адреса было решено отказаться сразу, в противовес с работой с числами. Строковое представление адреса парсится только на входе и выводится как результат. Во первых: это удобнее, во вторых: быстрее. Более того, октеты адреса располагаются в той же памяти, что и uint32_t число (велосипед не изобретаем, ядро линукса устроено так же). Но при использовании операций сравнения над этими uint32_t значениями, мы можем столкнуться с проблемой когда порядок октет в числе либо big-endian, либо little-endian, так что надо уметь детектировать порядок октет в числе на целевой платформе, и иметь две реализации сортировок. Далее тривиально - все условия вывода записываются as is и несложным образом проверяем ячейки памяти, где располагаются октеты IPv4 адресов.

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
cmake --build . --config Release
cmake --build . --config Release --target package
@rem в результате будет получен инсталляционный файл cpp_otus-0.10.21-win32.exe
```

### Дополнительные материалы по	теме

* [Git Book](https://git-scm.com/book/ru/v2)
* [CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)
* [Тестирование в CMake](https://habr.com/ru/post/433822/)
* [CMake: CPack Generators](https://cmake.org/cmake/help/latest/module/CPack.html)
* [CMake: пример настройки сборки инсталлятора](https://github.com/colinbourassa/librosco/blob/master/CMakeLists.txt)
* [Как собрать ProjectConfig.cmake файл](https://gitlab.kitware.com/cmake/community/-/wikis/doc/tutorials/How-to-create-a-ProjectConfig.cmake-file)
* [GitHub Actions](https://docs.github.com/en/actions)
* [GitHub Actions: integrating GTest](https://github.com/bastianhjaeger/github_actions_gtest_example)
* [GitHub highlighting code blocks](https://docs.github.com/en/github/writing-on-github/working-with-advanced-formatting/creating-and-highlighting-code-blocks)
* [GitHub Actions: parameter expansion](https://stackoverflow.com/a/9533099)

## Занятие №8. Обзор C++17. Constexpr lambda. Fold expression. Attributes. Type deduction

TODO: на следующий день после поступления на курс прослушал онлайн-урок, требуется повторить упражнения разобранные в уроке.

### Дополнительные материалы по теме

* [Standard C++: видео-курсы](https://www.youtube.com/playlist?list=PL3BR09unfgcjJ2YUCgh62vgv_1maXcKuS)
