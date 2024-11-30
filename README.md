# memdb-lib
## Запуск проекта
1. ```mkdir cmake-build```
2. ```cd cmake-build```
3. ```cmake ..```
4. ```cmake --build .```

- ```./db-app``` - запуск программы в ```app.cpp```
- ```./lib-tests``` - запуск всех тестов в ```test.cpp```
- ```./lib-tests "[basic]"``` - запуск тестов с тегом ```[basic]``` в ```test.cpp```
- ```./lib-tests "[benchmark]"``` - запуск тестов с тегом ```[benchmark]``` в ```test.cpp```