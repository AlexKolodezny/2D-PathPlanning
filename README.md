# 2D-PathPlanning
Программа нахождения кратчайшей траектории на 2D карте между двумя точками.

## Сборка
Сборка с помощью cmake

### Linux и Mac
Release сборка:
```bash
cd Build
cd Release
cmake ../../ -DCMAKE_BUILD_TYPE="Release"
make
make install
```

### Windows
Release сборка:
```cmd
cd Build
cd Release
set PATH
cmake ../../ -DCMAKE_BUILD_TYPE="Release" -G "MinGW Makefiles"
mingw32-make
mingw32-make install
```

## Запуск
В папке /Bin/Release будет находится исполняемый файл, в качестве аргумента принимается XML файл с входными данными.
Пример
```bash
Bin/Release/Alexander_Kolodezny_ASearch Example/example.xml
```

## Формат входного файла
Файл с входными данными должен иметь формат XML с корневым элементом root содержащий элементы
* map  
 Описание карты
  * width, height - размеры карты
  * cellsize - размер одно клетки на карте
  * startx, starty - координаты стартовой точки
  * finishx, finishy - координаты конечной точки
  * grid - описание карты в виде матрицы из 0 и 1, где 0 обозначает свободную клетку, 1 - препятствие. Каждая строка задаётся в теге row
* algorithm 
 Описание алгоритма
  * searchtype - Применяемый алгоритм  
  Один из
    * astar - A*
    * dijkstra - Дейкстра
  * metrictype - используемая функция эвристики
  Одна из
    * diagonal - диагональная метрика
    * manhatten - манхэтоновское расстояние
    * euclidian - евклидово расстояние
    * chebyshev - расстояние Чебышева
  * hweight - вес эвристики
  * allowdiagonal - если true, то разрешены переходы по диагонали
  * cutcorners - если true, то разрешено срезать углы по диагонали
  * allowsqueeze - если true, то разрешено проходить по диагонали между рядом стоящими препятствиями

  Пример входного файла в Example/example.xml
## Формат выходного файла
В качестве ответа программа выдаёт выходной xml файл с элементами
* map, algorithm, options такие же как в входном файле
* log - содержит выходной ответ
  * mapfilename - имя входного файла
  * summury имеет атрибуты
    * numberofsteps - количество шагов сделанных алгоритмом
    * nodescreated - количество Node созданных за время работы алгоритма
    * length - длина полученного пути в клетках сетки
    * length_scaled - длина полученного пути
    * time - время работы алгоритма
  * path - представление итогового пути на сетке
  * lplevel - последовательность клеток, которые образуют путь
  * hplevel - последовательность секций, по которым двигаемся только в одном направлении

Пример выходного файла Example/example_log.xml