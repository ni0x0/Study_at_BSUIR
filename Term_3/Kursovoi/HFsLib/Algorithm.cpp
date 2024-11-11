#include "Algorithm.h"

// найти совпадающий с переданным объект
template <class Iter, class T>
Iter Algorithm::find_match(Iter itBeg, Iter itEnd, const T& obj) {
	Iter iter_to_ret;
	while (1) {
		if (*itBeg == obj) {
			iter_to_ret = itBeg;
			break;
		}
		if (itBeg == itEnd)
			break;
		itBeg++;
	}
	return iter_to_ret;
}

// найти попадающий в диапазон между переданными объект
template <class Iter, class T>
Iter Algorithm::find_in_diap(Iter itBeg, Iter itEnd, const T& l_obj, const T& h_obj) {
	Iter iter_to_ret;
	while (1) {
		if (*itBeg >= l_obj && *itBeg <= h_obj) {
			iter_to_ret = itBeg;
			break;
		}
		if (itBeg == itEnd)
			break;
		itBeg++;
	}
	return iter_to_ret;
}

// получить объект, подходящий по заданному условию
template <class Iter, class T>
Iter Algorithm::get_by_condition(Iter itBeg, Iter itEnd, bool (*func)(T& obj1, T& obj2)) {
	Iter iter_to_ret = itBeg;
	while (1) {
		if (func(*iter_to_ret, *itBeg) == true)
			iter_to_ret = itBeg;
		if (itBeg == itEnd)
			break;
		itBeg++;
	}
	return iter_to_ret;
}

// отсортировать контейнер по заданному условию
template <class Iter, class T>
void Algorithm::sort_by_condition(Iter itBeg, Iter itEnd, bool (*func)(T& obj1, T& obj2)) {
	Iter responded_to_cond_pos; T temp;	Iter sec_it;

	while (1) {
		if (itBeg == itEnd)
			break;
		responded_to_cond_pos = itBeg;
		sec_it = itBeg;
		while (1) {
			sec_it++;
			if (func(*sec_it, *responded_to_cond_pos) == true)
				responded_to_cond_pos = sec_it;
			if (sec_it == itEnd)
				break;
		}

		temp = *itBeg;
		*itBeg = *responded_to_cond_pos;
		*responded_to_cond_pos = temp;

		itBeg++;
	}
}
