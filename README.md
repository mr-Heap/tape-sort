# tape-sort

### Tape

Устройство хранения данных типа лента (Tape) предназначено для последовательной записи и
чтения данных. Считывающая/записывающая магнитная головка неподвижна во время чтения и
записи, а лента имеет возможность двигаться в обоих направлениях. Запись и чтение информации
возможны в ячейку ленты, на которой в данный момент находится магнитная головка.
Перемещения ленты – затратная по времени операция – лента не предназначена для
произвольного доступа.

Интерфейс для работы с лентой -- ```ITape<T>```

Его реализует класс ```BinaryTape<T>``` -- если элементы в файле записаны друг за другом в двоичном представлении без
разделителей, то для такого файла создается лента ```BinaryTape<T>```. Чтобы очистить файл, в качестве второго аргумента нужно передать ```std::ios::trunc```.

Если элементы в файле записаны посимвольно с разделителями, то для такого файла создается лента ```Tape<T>```. Интерфейс идентичен ```BinaryTape<T>```.
Чтобы отобразить результат работы ленты в файл, надо вызвать метод ```flush()``` на ленте.

```EmulatedBinaryTape<T>``` -- эмулируемая лента
### Algorithm

Идея такая же как в mergesort, однако мы теперь хотим минимизировать операции left и right на лентах.
В обычной сортировке мы делили исходную ленту на 2 части, и вызывали алгоритм рекурсивно. Выходом из рекурсии является лента
размера меньше M, которую мы можем посортить в памяти. Наша глубина рекурсии `log(n)`, на каждой глубине мы делаем слияние 
за `O(n*L)`, где `L` - время операции `left/right` на ленте. Теперь на каждом уровне будем создавать от `2` до `M` лент (в нашем алгоритме - `M`).
Теперь глубина рекурсии составляет `log_M(N)`, однако по асимптотике мы не выиграли - теперь мы выполняем слияние
каждого слоя за `O(n*L*log(M))`, т.к. всего у нас `M` лент и мы используем кучу, чтобы выбирать минимальный элемент из них.
Однако доставание минимума не требует вызова операций `left\right`, поэтому работает быстро. А значит итоговое число подобных операаций уменьшится.
Общее время работы алгоритма `O(n*L*log_2(M)*log_M(N)) = O(n*L*log_2(N))`, однако число операций `left/right` пропорционально глубине рекурсии,
а значит их число уменьшилось.

### Emulator
Приложение стоит запускать в консоли под windows или в терминале linux. В ide вроде clion очистить консоль для отрисовки
следующего состояния программы -- нетривиальная задача, очистка выполняется стандартными командами clear/cls
Эмулятор отображает состояния всех лент. Можно запустить сортировку с эмуляцией и без, передав соответствующий флаг в template

Также можно создать ленту с поддержкой эмуляции и без неё, также передав соответсвующий флаг.

### Usage
```
    tape::Tape<int, true> in("path/to/file-in"); // Лента с эмуляцией
    tape::Tape<int> out("path/to/file/out"); //Выходная лента без эмуляции 
    size_t M = 3; 
    Sort<int>::sort(in, out, M); // Сортировка для типа ленты Tape<int>
```

### Тесты

Тесты можно запускать через Cmake
