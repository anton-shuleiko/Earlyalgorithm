
Локальное тестирование:

 g++ main.cpp -o run
 ./run

Полное тестирование:
 Сначала нужно комментировать функцию main в main.cpp, затем:

 mkdir build
 cd build
 cmake ..
 make
 ./earleycpp


