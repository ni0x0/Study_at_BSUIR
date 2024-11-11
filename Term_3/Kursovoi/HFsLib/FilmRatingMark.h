#ifndef FILM_RAT_MARK_H
#define FILM_RAT_MARK_H

#include "FilmMark.h"

// клас "заметок" с информацией о пользовательской оценке
class FilmRatingMark: public FilmMark
{
protected:
	int ratingMark;                                // оценка пользователя
public:
	FilmRatingMark();                              // конструкторы
	FilmRatingMark(Film* _fp);
	FilmRatingMark(int _f_id);
	FilmRatingMark(Film* _fp, int _rt);
	FilmRatingMark(int _f_id, int _rt);

	FilmRatingMark& operator = (FilmRatingMark& oth);                    // перегрузка оператора присваивания
	bool operator == (const FilmRatingMark& oth);                        // перегрузка оператора "равно"

	friend fstream& operator << (fstream& out, FilmRatingMark& oth);     // перегрузка оператора записи в бинарный файл
	friend fstream& operator >> (fstream& in, FilmRatingMark& oth);      // перегрузка оператора считывания из бинарного файла

	void set_ratingM(int _rm);         // установить значение оценки
	int get_ratingM();                 // получить значение оценки
};

#endif

