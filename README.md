# Проект "Симулирование поведение мыши в лабиринте с помощью дискретных операторов на графе".
Руководитель проекта: Всеволод Леонидович Чернышев.
НИУ ВШЭ, ФКН ПМИ, 2021
***
### О проекте ###
Цель данной работы заключается в том, чтобы предоставить ученым-биологам удобный инструмент для исследований в области когнитивных нарушений. Предполагается, что пищедобывающее поведение мыши в лабиринте может быть моделировано в компьютерной программе с помощью дискретных операторов. С другой стороны, отключение тех или иных дискретных операторов поможет выявить патологические изменения в поведении мыши. На данный момент составление модели, достаточно близкой к действительному положению дел и гибкой для проведения экспериментов, поможет ученым в их исследованиях. В перспективе программа может применяться как в исследовательских лабораториях, так и в области обнаружения и диагностики когнитивных заболеваний (у грызунов).
Мы представляем лабиринт как граф с вершинами с биологическими весами. Каждый дискретный оператор определяется вероятность и методом его работы и необходим для построения или обработки последовательности вершин графа (в буквенном виде), маршрута, по которому двигается мышь в лабиринте. Вероятность дискретного оператора определяет частоту его применения в последовательности.
На данный момент реализованы все дискретные операторы и необходимые для их работы алгоритмы, разработаны алгоритмы построения последовательностей с помощью дискретных операторов, а также функция получения их наиболее точных весов (вероятностей применения) на основе получаемого с помощью метрики Левенштейна параметра сходства двух последовательностей. Кроме того, были проведены программные тесты, позволяющие оценить точность построенной модели.

### Интерфейс (v0.1) ###
Для управления программы разработан простой bash-подобный консольный интерфейс.
* `exit`, `ex` — выход из лабиринта
* `build`, `bd` — построить новый лабиринт
* `dirs` — вывод матрицы направлений
* `info`, `mazeinfo` — вывод полной информации о лабиринте
* `verts` — вывод информации о вершинах
* `least` — вывод кратчайших путей
* `alist` — вывод списка смежности
В командах `dirs`, `info`, `verts`, `least`,  `alist` может использоваться параметр `id` - номер лабиринта. Если параметр передан не был, на консоль выведется информация о последнем созданном лабиринте. Если пользователь не создал ни одного лабиринта, команды не будут выполнены.
* `gen first` — генерирует первый проход лабиринта в файл\
    Параметры:
    * `id` — номер лабиринта
	* `start` — вершина, с которой следует начать
	* `filename` —  имя файла вывода\
    Пример: `gen first 0 Ы firstline.txt`
* `gen discrete`— генерирует дискретную последовательность из текущей\
Параметры:
    * `id` — номер лабиринта
	* `n` — число генерируемых последовательностей
	* `p1, p2, p3, p4` — вероятности в формате `0.xxx` или `0,xxx` в последовательности __*ISGR*__ (инверия-симметрия-сжатие-кольцо)
	* `filename` — имя файла вывода\
	Пример: `gen discrete 0 10 0.25 0,35 0,1 0.3 test.txt`
	
Если пользователь введет несуществующую команду, программа выведет сообщение и продолжит выполнение, предложив ввести новую команду.

В дальнейшем планируется переход на полноценный графический интерфейс.
***
### Ссылки и источники ###
1. Никольская К.А., Дидык Л.А., Серебрякова Е.Р. Структура внешней среды как системообразующий фактор условнорефлекторного процесса. // Сравнительная физиология высш. нервн. деят. человека и животных. 1990. М. Изд-во Наука. С. 69-90.
2. Edward C. Tolman. Cognitive maps in rants and men. 1948. The Psychological Review. 55(4), 189-208.
3. Russell A Epstein, Eva Zita Patai, Joshua B Julian & Hugo J Spiers. The cognitive map in humans: spatial navigation and beyond. 2017. Nature Neuroscience. 20, pages 1504-1513 (2017).
4. А. В. Ахтеров, Т. Ф. Барбашова, А. И. Белоусов, А. А. Бобик, А. Ю. Воронин, Ф. Р. Джегутанов, Е. Ю. Зуева, А. А. Кирильченко, М. Л. Кукушкин, А. С. Молоствов, В. Е. Пряничников, В. С. Смирнова, С. М. Соколов, Н. Ш. Шакарова. К вопросу о моделировании двигательного поведения крыс в многоальтернативном Ж-образном лабиринте. 2008. Препринты ИПМ им. М. В. Келдыша. 2008. 068, 28 с.
