# tape-sort

Интерфейс для работы с лентой -- ```ITape<T>```

Его реализует класс ```BinaryTape<T>``` -- если элементы в файле записаны друг за другом в двоичном представлении без
разделителей, то для такого файла создается лента ```BinaryTape<T>```. Чтобы очистить файл, в качестве второго аргумента нужно передать ```std::ios::trunc```.

Если элементы в файле записаны посимвольно с разделителями, то для такого файла создается лента ```Tape<T>```. Интерфейс идентичен ```BinaryTape<T>```.
Чтобы отобразить результат работы ленты в файл, надо вызвать метод ```flush()``` на ленте.

```EmulatedBinaryTape<T>``` -- эмулируемая лента

### Algorithm

Идея такая же как в mergesort, однако теперь у нас есть возможность сохранить и сортировать
в памяти M элементов. Будем идти по ленте, сохраняя в память числа, затем сориторвать и записывать
на временную ленту. Теперь наша задача попарно слить каждый из n/k блоков в блоки большего размера.
Как слить два больших отсортированных блока имея M свободной памяти?
Заведем два дека размером M/2 на первый и второй блок. Будем удалять минимум из начал двух деков и дописывать в конец
дека следующий элемент из соответсвующего ему блока. Слияние всех пар блоков работает за O(n). При каждом таком слиянии
все отсортированные блоки увеличиваются вдвое, итого O(nlogn)

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
    Sort<int, true>::sort(in, out, M); // Сортировка с эмуляцией
```

### Тесты

Тесты можно запускать через Cmake
