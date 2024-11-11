#ifndef MOVIE_H
#define MOVIE_H
#include "Film.h"

// класс с информацией о полнометражном фильме 
class Movie : public Film
{
protected:
	// длительность фильма:
	int hour;                     // количество часов
	int minute;                   // минут

	static int movie_id_counter;  // статический ID счетчик полнометражных фильмов
public:
	Movie();                      // конструкторы
	Movie(const char* _name, int _prod_year, const char* _country, genres _genre,
		int _hour, int _minute);
	Movie(const Movie& oth);
	virtual ~Movie() {}

	Movie& operator = (Movie& oth);                        // перегрузка оператора присваивания
	bool operator == (const Movie& oth);                   // перегрузка оператора "равно"
	bool operator <= (const Movie& oth);                   // перегрузка оператора  "меньше равно"
	bool operator >= (const Movie& oth);                   // перегрузка оператора  "больше равно"

	friend fstream& operator << (fstream& out, Movie& oth);     // перегрузка оператора записи в бинарный файл
	friend fstream& operator >> (fstream& in, Movie& oth);      // перегрузка оператора считывания из бинарного файла

	virtual void copy_data(Movie& oth);    // скопировать все данные другого фильма, кроме рейтинга и ID

	void set_hour(int);                    // методы для изменения значений в полях
	void set_minute(int);

	int get_hour();                        // методы для получения значений из полей
	int get_minute();

	static void set_movie_id_c(int);       // статические методы, изменяющие значение счетчика ID
	static int get_movie_id_c();
};

#endif

