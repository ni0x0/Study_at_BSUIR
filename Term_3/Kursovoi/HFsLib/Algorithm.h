#ifndef ALGORITHM_H
#define ALGORITHM_H

// класс шаблонных статических методов алгоритмов для работы с контейнером
class Algorithm
{
public:
	// найти совпадающий с переданным объект
	template <class Iter, class T>
	static Iter find_match(Iter itBeg, Iter itEnd, const T& obj);

	// найти попадающий в диапазон между переданными объект
	template <class Iter, class T>
	static Iter find_in_diap(Iter itBeg, Iter itEnd, const T& l_obj, const T& h_obj);

	// получить объект, подходящий по заданному условию
	template <class Iter, class T>
	static Iter get_by_condition(Iter itBeg, Iter itEnd, bool (*func)(T& obj1, T& obj2));

	// отсортировать контейнер по заданному условию
	template <class Iter, class T>
	static void sort_by_condition(Iter itBeg, Iter itEnd, bool (*func)(T& obj1, T& obj2));
};

#endif