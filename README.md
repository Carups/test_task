# Тестовое Задание Fast Forward
## Техническое задание
Дан набор реальных биржевых данных (формат сообщений описан ниже). В
первой строчке находится снепшот стакана, далее следуют сообщения об
апдейтах.
Вам необходимо реализовать программу на языке C++, которая восстанавливает
снепшоты стаканов на каждый апдейт. В выходной файл необходимо вывести
цены и объемы лучших уровней best bid и best ask с временной меткой. Подумайте
над выбором оптимальной структуры данных для данной задачи, учитывая, что
максимальное количество ценовых уровней на одной стороне снепшота равно 20.
Подумайте, что нужно будет поменять, если размер одной стороны книги будет
больше - 400 уровней. Основное требование к решению - это его
производительность.
Дополнительно необходимо провести benchmark на скорость обновления стакана
и получения из него best bid и best ask. Проведите сравнение различных структур
данных для книги. Бенчмарк должен включать в себя только применения
распарешнного события к ордербуку, не нужно включать в него время парсинга и
время дискового чтения/записи.

## Пояснение к решению
Поскольку уровней всего 20, вероятно, решение на основе простого массива будет наиболее эффективным, поскольку ядро оптимально работает
с последовательными сегментами памяти, несмотря на то, что имеет наименее оптимальную асимптотическую сложность.

### Реализация
* __Map__: std::map
* __Hash__: std::unordered_map 
* __Heap__: Кастомная структура на основе std::vector, с поддержкой операций upsacle downscale для удаления и добавления элементов в HEAP
* __Array__: Использовал std::array с поддержкой удаления за `O(1)` 
### Асимптотика
* __Map__: 
  * Build ` O(NlogN) `
  * Update `O(LogN)`
  * Get bests `O(1)`
* __Array__: 
  * Build `O(N)`
  * Update `O(N)`
  * Get bests `O(1)`
* __Hash__: 
  * Build `O(N)`
  * Update `O(1)`
  * Get bests `O(N)` 
* __Heap__: 
  * Build `O(N)` (так как входные вектора отсортированы)
  * Update `O(NlogN)`
  * Get bests `O(1)`
### Структура репозитория
Для парсинга Json добавил header only библиотеку https://github.com/nlohmann/json.
Реализацию делал согласно идиоме PIMPL языка с++, чтобы вся использованная структура данных была в отдельном файле, за счёт этого 
на этапе линковки возможно подменять файл с реализацией структуры данных и получать разные бинарники.
Все использованные структуры данных можно найти в папке impl
## Бенчмарк
Для запуска бенчмарка можно воспользоваться скриптом ./run_test.sh 
Скрипт запускает программу 10 раз для каждой структуры данных
### Cтруктура вывода
Входные данный для программы берутся из файла /huobi_global_depth.log
Результат работы программы пишется в out_{DataStructure}
Время работы каждого запуска в result_time_{DataStructure}

### Конфигурация
Проводил замеры в следующей конфигурации:
CPU model: 11th Gen Intel(R) Core(TM) i7-11850H @ 2.50GHz
gcc (Ubuntu 11.3.0-1ubuntu1~22.04) 11.3.0

### Результат
Среднее время работы для каждой структуры. 
* __Array__:   6 368  miliseconds
* __Heap__:   11 131  miliseconds
* __Map__:    18 569  miliseconds
* __Hash__:  112 421  miliseconds

В итоге простой массив показывает самый лучший результат. Интересно, что unordered_map работеат сильно хуже. Вероятно структура плохо оптимизирована для double ключа. 
При увеличение уровней с 20 до 400, результаты поменяются местами, Array думаю будет самый не оптимальный, heap и map, вероятно, будут бороться за первое место по эффективности. В целом для heap очень многое зависит от типа данных, если будет не очень много удалений, а больше запросов топа и изменений значений, то он будет наиболее эффективным. 
 
