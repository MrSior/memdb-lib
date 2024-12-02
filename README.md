# memdb-lib

## Реализованно
- ```create table <table name> ([{attributes}] <name>: <type> [= <value>], ...)```
- ```insert (<values>) to <table>```
- ```select <columns> from <table> where <condition>```.
- ```update <table> set <assignments> where <condition>```
- ```delete <table> where <condition>```
- ```<table1> join <table2> on <condition>```

- ```select``` и ```join``` поддерживают вложенность запросов. Вместо ```<table>``` могут быть запросы, возвращающие таблицу.
- При ошибке в запросе выдаётся ошибка с описанием.

ВАЖНО!!! <br>
в выражениях все операции кроме унарных должны отделяться от операнда пробелом <br>
```
a+(-b)      ---> error
a + (-b)    ---> OK
```

## Запуск проекта
1. ```mkdir cmake-build```
2. ```cd cmake-build```
3. ```cmake ..```
4. ```cmake --build .```

- ```./db-app``` - запуск программы в ```app.cpp```
- ```./lib-tests``` - запуск всех тестов в ```test.cpp```
- ```./lib-tests "[basic]"``` - запуск тестов с тегом ```[basic]``` в ```test.cpp```
- ```./lib-tests "[benchmark]"``` - запуск тестов с тегом ```[benchmark]``` в ```test.cpp``` (их пока нет)