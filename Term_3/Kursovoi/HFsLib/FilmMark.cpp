#include "FilmMark.h"

// конструктор по умолчанию
FilmMark::FilmMark() {
	film_id = -1;
}
// конструктор с параметром - указателем на фильм
FilmMark::FilmMark(Film* _fp) {
	if (_fp == NULL)
		film_id = -1;
	else film_id = _fp->get_id();
}
// конструктор с параметром - ID фильма
FilmMark::FilmMark(int f_id) {
	film_id = f_id;
}

// перегрузка оператора присваивания
FilmMark& FilmMark::operator = (FilmMark& oth) {
	if (this == &oth)
		return *this;

	this->film_id = oth.film_id;
	return *this;
}

// перегрузка оператора присваивания
bool FilmMark::operator == (const FilmMark& oth) {
	if (this == &oth)
		return true;

	if (this->film_id != oth.film_id && oth.film_id != -1) return false;
	return true;
}

// перегрузка оператора записи в бинарный файл
fstream& operator << (fstream& out, FilmMark& oth) {
	out.write(reinterpret_cast<char*>(&oth.film_id), sizeof(oth.film_id));

	return out;
}

// перегрузка оператора считывания из бинарного файла
fstream& operator >> (fstream& in, FilmMark& oth) {
	in.read(reinterpret_cast<char*>(&oth.film_id), sizeof(oth.film_id));

	return in;
}

// установить значение ID по указателю на фильм
void FilmMark::set_fields(Film* _fp) {
	if (_fp == NULL)
		film_id = -1;
	else film_id = _fp->get_id();
}
// установить значение ID
void FilmMark::set_id(int _id) {
	film_id = _id;
}
// получить значение ID
int FilmMark::get_film_id() {
	return film_id;
}

