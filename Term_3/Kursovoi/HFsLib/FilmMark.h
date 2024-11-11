#ifndef FILM_MARK_H
#define FILM_MARK_H

#include "Film.h"

// класс "заметок" о фильме
class FilmMark
{
protected:
	int film_id;                // ID фильма
public:
	FilmMark();                 // конструкторы
	FilmMark(int f_id);
	FilmMark(Film* _fp);

	FilmMark& operator = (FilmMark& oth);                        // перегрузка оператора присваивания
	bool operator == (const FilmMark& oth);                      // перегрузка оператора "равно"

	friend fstream& operator << (fstream& out, FilmMark& oth);   // перегрузка оператора записи в бинарный файл
	friend fstream& operator >> (fstream& in, FilmMark& oth);    // перегрузка оператора считывания из бинарного файла

	void set_fields(Film* _fp);           // установить значение ID по указателю на фильм
	void set_id(int _id);                 // установить значение ID
	int get_film_id();                    // получить значение ID
};

#endif




