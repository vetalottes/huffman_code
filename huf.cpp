#include <iostream> 
#include <Windows.h>
#include <vector> // библиотека для работы с векторами (некий аналог динамического массива, но с куда большим функционалом)
#include <map> // библиотека для работы с map - ассоциативным контейнером, который работает по принципу [ключ — значение] 
// (некий массив, но вместо индексов могут быть, например, строки, буквы. в нашем случае - ассоциативный словарь)
#include <list> // библиотека для работы со списками list
#include <string> // библиотека для работы со строками
using namespace std;

class Node // создаем класс узлов
{
public: 
	int a; // частота появления символа в тексте
	char c; // сам символ
	Node* left, * right; // указатели на левые и правые ветви дерева
	Node() { left = right = NULL; }
	Node(Node* L, Node* R) // принимает две ссылки: на левого и правого сына(ветки дерева)
	{
		left = L; // левый становится L
		right = R; // правый становится R
		a = L->a + R->a; // переменная а становится равной их сумме
	}
};

bool Sorti(const Node* l, const Node* r) // сортировка
{ 
	return l->a < r->a; // возвращаем тру, если левый параметр меньше, чем правый
}

void print(Node* root, unsigned k = 0) // функция вывода бинарного дерева на консоль
{
	if (root != 0) { // если вершина дерева не равна 0, то рекурсивно выводим сначала правые ветки, потом левые
		print(root->right, k + 7);
		// с помощью цикла for выводим пробелы, чтобы всё не сливалось в кучу
		for (unsigned i = 0; i < k; i++)
			cout << " ";
		if (root->c) // если мы наткнулись на букву, то выводим ее и ее частоту появления в скобочках
			cout << root->a << "(" << root->c << ")" << '\n';
		else // если нет, то выводим просто число
			cout << root->a << '\n';
		print(root->left, k + 7); // k+7 - отступ, чтобы, опять же, не сливалось в кучу
	}
}

void BuildTable(Node* root, vector<bool>& code, map<char, vector<bool>>& table)
// рекурсивная функция, которая сопоставляет полученные коды Хаффмана с самими символами
{
	// идем от вершины дерева вниз, слева всегда нули, справа единицы
	// если узел слева не равен нулю, то слева в принципе есть ветка\узел, тогда проходим по всей левой стороне, пока не найдем символ; 
	// когда его нашли, убираем с конца символ и проверяем правую ветку, вскоре снова возвращаясь в левую.
	if (root->left != NULL)
	{
		code.push_back(0); // добавляем в вектор 0 (так как мы идем по левой ветке, а там одни нули)
		BuildTable(root->left, code, table); // снова запускаем рекурсивную функцию для левой ветки
	}
	// аналогично для правой стороны, но в вектор кода добавляем 1 (так как это правая сторона дерева, а так только единицы)
	if (root->right != NULL)
	{
		code.push_back(1); // добавляем в вектор 1
		BuildTable(root->right, code, table); // снова запускаем рекурсивную функцию для правой ветки
	}
	// если мы наткнулись на букву и если это лист дерева (то есть ни снизу слева\справа нет узлов), то ассоциируем ее с соответствующим кодом 
	if (root->left == NULL && root->right == NULL)
		table[root->c] = code;
	// удаляем один символ кода с конца, чтобы продолжить там, где остановились и начать по новой сопоставлять коды 
	if (code.size())
		code.pop_back();
	// пример: допустим, мы дошли до символа и его код равен 010. тогда на этом шаге мы выкидываем один последний символ и начинаем следующий поиск код-символа с 01
}

int main()
{
	setlocale(LC_ALL, "Rus");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	cout << "Введите текст, который Вы хотите закодировать кодом Хаффмана!" << '\n';
	string s;
	getline(cin, s);


	// Шаг 1: считаем частоты появления символов
	// проходим по строке, с помощью map ассоциируем символ и количество его вхождения, таким образом у каждого узла будут 2 параметра: символ char и число (частота) int 
	map <char, int> u;
	for (int i = 0; i < s.length(); i++) { 
		char c = s[i]; // в c записываем переменную
		u[c]++; // узел от этой переменной увеличиваем на 1
	}
	// выводим, какому символу сколько появлений соответствует с помощью итератора, который используется для обращения к определенному элементу в контейнерах
	map<char, int>::iterator j;
	cout << "Шаг 1. Частота появления символов в данном тексте: " << '\n';
	for (j = u.begin(); j != u.end(); j++) // проходимся от первого элемента контейнера до последнего
		cout << j->first << ": " << j->second << '\n'; // ключевое слово first всегда указывает на первый параметр (char - символ), а second - на второй параметр (int - частота)


	// Шаг 2: создаем бинарное дерево
	// создаём list указателей на Node, чтобы в дальнейшем работать с узлами дерева (сортировать и искать коды)
	list<Node*> derevo;
	// аналогично проходим от начала map до конца
	map<char, int>::iterator itr;
	for (itr = u.begin(); itr != u.end(); ++itr) { // загружаем из map в Node
		Node* p = new Node; // создаем новый узел
		p->c = itr->first; // его символ - это первый параметр (также символ)
		p->a = itr->second; // его число становится равным второму параметру (частоте)
		derevo.push_back(p); 
	}
	while (derevo.size() != 1) // пока мы не дошли до последнего единственного узла - главной вершины дерева (пока количество элементов в списке не станет равно 1) 
	{
		derevo.sort(Sorti); // сортируем посредством функции
		Node* SonL = derevo.front(); // левому узлу присваеваем первый наименьший элемент 
		// (тк у нас отсортированные по возрастанию элементы, то наименьшие как раз будут располагаться в начале)
		derevo.pop_front(); // удаляем из списка list элемент в начале, таким образом на его место становится второй элемент, который как раз нужен нам
		Node* SonR = derevo.front(); // правому узлу присваеваем первый элемент (что как раз является вторым наименьшим из-за удаления из списка)
		derevo.pop_front(); // также удаляем из списка list
		Node* parent = new Node(SonL, SonR); // создаем новый узел, равный сумме двух наименьших (SonL и SonR)
		derevo.push_back(parent); // добавляемый созданный узел в конец списка derevo
	}
	Node* root = derevo.front(); // root - указатель на главную вершину (корень) дерева
	cout << '\n';
	cout << "Шаг 2. Бинарное дерево выглядит следующим образом: " << '\n';
	// выведем на экран полученное бинарное дерево с помощью функции
	print(root);


	// Шаг 3: создаем таблицу значений
	// проходим рекурсивно по дереву и выписываем коды
	vector<bool> code; 
	map<char, vector<bool> > table; // сама ассоциация символа с кодом
	BuildTable(root, code, table);
	cout << '\n';
	cout << "Шаг 3. Таблица кодов: " << "\n";
	// выводим на консоль полученную таблицу
	// сопоставляем первый параметр (символ) с параметром полученных кодов символа
	for (itr = u.begin(); itr != u.end(); itr++)
	{
		cout << itr->first << " - ";
		for (int j = 0; j < table[itr->first].size(); j++)
			cout << table[itr->first][j];
		cout << endl;
	}


	// Шаг 4: код Хаффмана
	// выводим полученный код на консоль
	cout << '\n';
	cout << "Полученный код Хаффмана: ";
	string res = ""; // строка для кода
	for (int i = 0; i < s.length(); i++) { // проходим по всей длине изначальной строки
		for (int j = 0; j < table[s[i]].size(); j++) // а также по всей нашей полученной таблицы map
		{
			// на каждом шаге прибавляем новый код к строке res и выводим его
			res += table[s[i]][j];
			cout << table[s[i]][j];
		}
		cout << " ";
	}
	cout << '\n';
}
