# Выполнение Фаззинг-тестирования приложения
## Выполнил - Сучков Василий Вячеславович ББМО-01-22
### 1. Подготовка дистрибутива Ubuntu для выполнения практической работы
<p align="center">
  <img src="/images/ubuntu.png">
</p>

Выполняем базовые команды обновления репозиториев:
```bash
sudo apt-get update
sudo apt-get upgrade
```
### 2. Создание собственного C проекта
Программа будет получать на вход строку из ```stdin``` и выполнять различные действия, конечно в программе будут предусмотрены ошибки, например, не правильное выделение памяти:
```c
char * tmp;
tmp = (char *) malloc(sizeof(char) * strlen(buf) - 1);
```
* Мы не оставляем место под нуль символ в строке, данную ошибку должен выявить санитайзер.
* Т.к. программа почти не имеет условных ветвлений - покрытие кода должно быть высокое, проверим при помощи инструмента нашей практической работы.

Функционал программы следующий, после получения строки, длинной не больше 2048, будет запущенно несколько модулей программы, выполняющие различные действия.
```c
void run(char *buf)
{
    if (buf[strlen(buf) - 1] == '\n')
    {
        char * tmp;
        tmp = (char *) malloc(sizeof(char) * strlen(buf) - 1);
        strncpy(tmp, buf, strlen(buf) - 1);
        tmp[strlen(buf) - 1] = 0;
        to_upper(tmp);
        to_lower(tmp);
        ft_atoi(tmp);
        counter(tmp);
        trim(tmp);
        free (tmp);
    }
    else
    {
        to_upper(buf);
        to_lower(buf);
        ft_atoi(buf);
        counter(buf);
        trim(buf);
    }
}
```
Пример использования:
<p align="center">
  <img src="/images/usage.png">
</p>

Программа выводит:
* Строку в верхнем регистре
* Строку в нижнем регистре
* Atoi строки, т.е. строку, преобразованную в int
* Количество чисел
* Количество символов алфавита
* Количество остальных символов
* Обрезанную строку, из начала строки удалены отступы

Для запуска тестирования я создал набор входных данных и поместил в папку inputs, которую также загружу в репозиторий.

Для сборки проекта используется простейший ```CMake``` файл:
```cpp
project(project)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_executable(main main.c)
```
Проект загружен в репозиторий.
### 3.  Проверка установки Docker, получение AFL++
Выполним команду
```bash
sudo systemctl status docker
```
```bash
ouput
● docker.service - Docker Application Container Engine
     Loaded: loaded (/lib/systemd/system/docker.service; enabled; vendor preset>
     Active: active (running) since Fri 2023-05-26 17:37:49 MSK; 9s ago
TriggeredBy: ● docker.socket
       Docs: https://docs.docker.com
   Main PID: 43427 (dockerd)
      Tasks: 9
     Memory: 29.6M
        CPU: 230ms
     CGroup: /system.slice/docker.service
             └─43427 /usr/bin/dockerd -H fd:// --containerd=/run/containerd/con>

```

Docker успешно установлен!
Загрузим образ контейнера и запустим его:
```bash
docker pull aflplusplus/aflplusplus
docker run -ti aflplusplus/aflplusplus
```
В интерфейсе нашего контейнера клонируем репозиторий проект командой:
```bash
git clone https://github.com/Marvoloo/prz6.git
```
### 4. Сборка с санитайзерами
Для сборки  проекта с санитайзерами будет использоваться наш ```CMake```, нам необходимо ввести следующие команды:
```bash
cd prz6
cmake . -D CMAKE_C_COMPILER=afl-cc -D CMAKE_CXX_COMPILER=afl-c++ 
AFL_USE_ASAN=1 AFL_USE_UBSAN=1 make -j20
./main < input/a -f 1 -l 1 -noImage
```
Для проверки санитайзером я подал простую входную строку:
```
          12112~jklad~kHHHHSHDAKJDHkjlksasas____~_~_~+asas
```
Вывод санитайзера показал ошибку в распределении памяти в функции run, и действительно, именно там я оставил ошибку.
<p align="center">
  <img src="/images/sanitaizer.png">
</p>

Уберем ошибку и проверим вывод санитайзера.
```c
char * tmp;
tmp = (char *) malloc(sizeof(char) * strlen(buf));
```
Ошибки больше нет, санитайзер помог нам найти ошибку с выделением памяти, что очень критично в "опасных" языках по типу СИ.
<p align="center">
  <img src="/images/sanitaizer_pass.png">
</p>

Но т.к. размер буфера у нас фиксированный (2048), то при подаче большой входной последовательности санитайзер должен выдать ошибку, проверим, подадим на вход другой файл из набора с большой строкой.
```bash
./main < input/d -f 1 -l 1 -noImage
```
<p align="center">
  <img src="/images/sanitaizer_bad.png">
</p>

Ошибка действительно есть, значит санитайзер успешно отрабатывает для нашего приложения.
### 5. Анализ покрытия кода
**Сборка**
Для проверки покрытия кода будет использоваться наш ```CMake```, нам необходимо ввести следующие команды:
```bash
make clean
cmake . -DCMAKE_C_COMPILER=cc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_FLAGS="-g -O2 --coverage" -DCMAKE_CXX_FLAGS="-g -O2 --coverage" -DCMAKE_EXE_LINKER_FLAGS="-lgcov"
make -j20
```
**Проверка покрытия**
```bash
./main < input/d -f 1 -l 1 -noImage
lcov -t "main" -o main.info -c -d .
genhtml -o report main.info
```
**Результаты покрытия кода**
```
Overall coverage rate:
  lines......: 91.3% (95 of 104 lines)
  functions..: 100.0% (8 of 8 functions)
```
Как и ожидалось, из за прямолинейности программы покрытие почти полное.
### 6. Фаззинг-тестирование
Для запуска фаззинг-тестирования будет использоваться наш ```CMake```, нам необходимо ввести следующие команды:
```bash
make clean
cmake . -D CMAKE_C_COMPILER=afl-cc -D CMAKE_CXX_COMPILER=afl-c++
AFL_USE_ASAN=1 AFL_USE_UBSAN=1 make -j20
afl-fuzz -i input -o out ./main
```
Мы получили результат, словили 8 крашей, для такой маленькой программы - это очень много, ну вероятнее всего санитайзер не пропустил входы с длинной больше 2048, при этом некоторые тесты прошли, но самое главное, что программа очень не оптимизирована в плане использования вычислительных ресурсов, если посмотреть на нижнее значение занятости cpu, то это станет очевидно.

<p align="center">
  <img src="/images/fuzzing.png">
</p>
