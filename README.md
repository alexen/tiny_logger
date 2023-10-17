[![Configure and build](https://github.com/alexen/tiny_logger/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/alexen/tiny_logger/actions/workflows/cmake-multi-platform.yml)

# tiny_logger
Простой класс журналирования с поддержкой ротации.

Основные хотелки:
- максимально быстрое логгирование в файл (с использованием потоков C++ ``std::ostream``);
- поддержка дублирования выходного потока на консоль в ``std::cerr``;
- поддержка ротации текущего лога "на лету" при достижении максимального размера лога;
- поддержка ротации логов при достижении максимального кол-ва файлов;
- корректное ведение логов в многопоточной среде.
