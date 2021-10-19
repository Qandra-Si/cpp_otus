# cpp_otus

Данный репозиторий хранит выполненные домашние задания по курсу C++ Developer Professional (Otus' course).

Для подготовки рабочего окружения следует установить программы и выполнить действия (раздел будет дополняться):

```bash
# основной набор программ
sudo apt install git cmake build-essential clang
# дополнительный набор программ
sudo apt install meld emacs-nox
# выбираем компилятор c++ который нравится больше
update-alternatives --config c++

# получение необходимых файлов
mkdir ~/cpp_otus && cd ~/cpp_otus
git clone --origin=github https://github.com/Qandra-Si/cpp_otus.git .
# запускаем сборку проекта
mkdir ./build && cd ./build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

# Занятие №1. Система сборки: build, test and deploy

Практические занятия, использующие систему сборки cmake находятся в директориях `lesson01`, `lesson01a`, `lesson01b`. При этом корневой проект cpp_otus является общий для всех включённых в него target-ов, выступающих в роли отдельных программ/подпроектов.
