# 2D-PathPlanning
Программа планирования траектории на  2D карте с одним или двумя критериями оптимизации.  
Первый критерий оптимизации --- длина пути.  
Второй критерий оптимизации --- опасность пути, счтитается как сумма опасностей клеток.
Поддерживаются алгоритмы Дейкстры, A*, Bi-Objective A* и генетический алгоритм. Карта задаётся в виде таблицы, где каждая клетка либо содержит препятствие, либо свободна.

## Постановка задачи
На заданной карте, предстваляющей таблицу, в которой каждай клетка либо проходима, либо является препятствием, необходимо найти путь от заданной стартовой клетки до конечной клетки.  
В случае задачи с одим критерием оптимизации мы хотим найти путь с минимальной длиной пути.  
В случае задачи с двумя критериями, мы кроме пути хотим оптимизировать опасность пути. Для каждой клетки на карте зададим значение w(s) -- расстояние до ближайшего препятствия (в смысле манхэттоновского расстояния, для клеток с препятствиями w(s) = 0). Тогда в программе можно оптимизировать для пути <img src="https://render.githubusercontent.com/render/math?math=\pi"> величину вида  
<img src="https://render.githubusercontent.com/render/math?math=\sum_{u \in \pi} \phi(w(u))">  
где <img src="https://render.githubusercontent.com/render/math?math=\phi"> -- положительная убывающая функция, которую можно выбрать как <img src="https://render.githubusercontent.com/render/math?math=\frac{1}{e^x}">, <img src="https://render.githubusercontent.com/render/math?math=\frac{1}{x}"> или <img src="https://render.githubusercontent.com/render/math?math=max(d_{max} - x, 0)">  
Так как два критерия могут противоречить друг другу программа выдаёт множество Парето-оптимальных решений.

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
В папке /Bin/Release будет находится исполняемый файл, принимает в качестве агрумента xml файл. Результат так же возвращается в виде другого xml файла.
Пример
```bash
Bin/Release/Alexander_Kolodezny_ASearch Example/example_astar.xml
```

## Формат входного файла
Файл с входными данными должен иметь формат XML с корневым тегом root содержащий элементы
* map  
 Описание карты
  * width - ширина карты (количетво столбцов)
  * height - высота карты (количество строк)
  * cellsize - размер одно клетки на карте
  * startx, starty - координаты стартовой точки
  * finishx, finishy - координаты конечной точки
  * grid - описание карты в виде матрицы из 0 и 1, где 0 обозначает свободную клетку, 1 - препятствие. Каждая строка задаётся в теге row  
  Нумерация начинается с верхнего левого угла, имеющего координаты (0, 0). Ось x идёт слева направо, ось y сверху вниз.
* algorithm 
 Описание алгоритма
  * searchtype - Применяемый алгоритм  
  Один из
    * astar - A*
    * dijkstra - Дейкстра
    * boaster - Bi-Objective A*
    * gamopp - Генетический алгоритм
  * metrictype - используемая функция эвристики (только для алгоритмов A* и Bi-Objective A*)
  Одна из
    * diagonal - диагональная метрика  
    <img src="https://render.githubusercontent.com/render/math?math=\rho((x_1, y_1), (x_2, y_2)) = min(|x_1 - x_2|, |y_1 - y_2|) %2B ||x_2 - x_1| - |y_2 - y_1||">

    * manhattan - манхэтоновское расстояние  
    <img src="https://render.githubusercontent.com/render/math?math=\rho((x_1, y_1), (x_2, y_2)) = |x_2 - x_1| %2B |y_2 - y_1|">
    
    * euclidian - евклидово расстояние  
    <img src="https://render.githubusercontent.com/render/math?math=\rho((x_1, y_1), (x_2, y_2)) = \sqrt{(x_1 - x_2)^2 %2B (y_1 - y_2)^2}">

    * chebyshev - расстояние Чебышева  
    <img src="https://render.githubusercontent.com/render/math?math=\rho((x_1, y_1), (x_2, y_2)) = max(|x_1 - x_2|, |y_1 - y_2|)">
  * hweight - вес эвристики (поддерживается только для A*)
  * allowdiagonal - если true, то разрешены переходы по диагонали
  * cutcorners - если true, то разрешено срезать углы по диагонали
  * allowsqueeze - если true, то разрешено проходить по диагонали между рядом стоящими препятствиями
  Для генетического алгоритма теги allowdiagonal, cutcorners и allowsqueeze устанавливаются в false.
* options
 Опции выходного файла
  * loglevel - параметр размера логирования один из множества 0, 0.5, 1, 1.5, 2.

  Пример входного файла в Example/example.xml
## Формат выходного файла
Если параметр loglevel входного файла больше 0, то в качестве ответа программа выдаёт выходной xml файл с элементами 
* map, algorithm, options такие же как в входном файле
* log - содержит выходной ответ
  * mapfilename - имя входного файла
  * summury - суммарная информация о работе алгоритма с атрибутами
    * numberofsteps - количество шагов сделанных алгоритмом (кроме генетического алгоритма)
    * nodescreated - количество Node созданных за время работы алгоритма (кроме генетического алгоритма)
    * time - время работы алгоритма
  * paths - множество найденых путей, каждый путь хранится в теге item, содержащий теги:
    * pathsummury - суммарная информация о пути с атрибутами
      * length - длина полученного пути в клетках сетки
      * length_scaled - длина полученного пути
      * danger - критерий опасности пути (если оптимизируем только длину, то этот тег равен 0)
    * path - представление итогового пути на сетке (если параметри loglevel >= 1.5)
    * lplevel - последовательность клеток, которые образуют путь (если loglevel >= 1)
    * hplevel - последовательность секций, по которым двигаемся только в одном направлении (если loglevel >= 1)  
  * Если loglevel >= 2 и используется генетический алгоритм, то внутри тега log в тегах generation записываются промежуточные поколения внутри тегов item.

Пример выходного файла Example/example_log.xml

## Визуализации выходных данных
В папке Visualization находятся скрипты для визуализации результатов работы алгоритма.

map.py принимает в качестве аргумента командной строки входной или выходной файл и показывет карту поиска, например  
```bash
python3 Visualization/map.py Example/example_boastar.xml
```  
![map.py](/Images/map.png)

graphic.py принимает в качестве аргумента командной строки один или два выходных файл с решениями, полученными для одной карты, и строит график из найденных решений, например  
```bash
python3 Visualization/graphic.py Example/example_genetic_log.xml Example/example_boastar_log.xml
```  
![graphic.py](/Images/graphic.png)

paths.py - принимает в качестве аргумента комндной строки выходной файл с loglevel >= 1 и показывает найденные пути на карте. Выбрать путь можно с помощью слайдера Paths, например  
```bash
python3 Visualization/paths.py Example/example_boastar_log.xml
```  
![paths.py](/Images/paths.png)

generation.py - принимает в качестве аргумента командной строки выходной файл, полученный генетическим алгоритмом с loglevel==2 и показывает все пути которые порождались в процессе работы алгоритма. Вержний слайдер выбирает поколение, нижний путь из этого поколения, например  
```bash
python3 Visualization/generation.py Example/example_genetic_log.xml
```  
![generation.py](/Images/generation.png)