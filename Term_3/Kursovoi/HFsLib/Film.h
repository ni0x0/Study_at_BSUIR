#ifndef FILM_H
#define FILM_H

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

enum genres {             // перечисление жанров
	genre_not_selected,   // не выбран  
	comedy,               // комедия
	militant,             // боевик
	fantastic,            // фантастика
	fantasy,              // фэнтези
	war,                  // военный
	horror,               // ужасы
	drama,                // драма
	romantic              // романтическое
};

#define FILM_NAME_LEN 51
#define COUNTRY_LEN 15

// класс с информацией о фильме
class Film
{
protected:
	char name[FILM_NAME_LEN];                  // название
	int ID;                                    // ID 
	int productionYear;                        // год производства
	char productionCountry[COUNTRY_LEN];       // страна производства
	genres genre;                              // жанр
	float rating;                              // рейтинг

public:
	Film();                                                  // конструкторы класса
	Film(const char* _name, int _prod_year, const char* _country, genres _genre);
	Film(const Film& oth);
	virtual ~Film() {}

	Film& operator = (const Film& oth);                       // перегрузка оператора присваивания
	bool operator == (const Film& oth);                       // перегрузка оператора "равно"
	bool operator <= (const Film& oth);                       // перегрузка оператора  "меньше равно"
	bool operator >= (const Film& oth);                       // перегрузка оператора  "больше равно"

	friend fstream& operator << (fstream& out, Film& oth);          // перегрузка оператора записи в бинарный файл
	friend fstream& operator >> (fstream& in, Film& oth);           // перегрузка оператора считывания из бинарного файла

	void add_rating_mark(int);                                      // добавить оценку к рейтингу фильма
	void delete_rating_mark(int);                                   // удалить оценку из рейтинга 
	void reset_rating_mark();                                       // сбросить рейтинг фильма
	virtual void copy_data(Film& oth);                              // скопировать такие данные другого фильма, как: имя, год, страна, жанр 

	void set_name(const char*);                                     // методы для изменения значений в полях
	void set_id(int _id);                  
	void set_prod_year(int);
	void set_prod_country(const char*);
	void set_genre(genres);
	void set_rating(float);

	char* get_name();                                               // методы для получения значений из полей
	int get_id();
	int get_prod_year();
	char* get_prod_country();
	genres get_genre();
	float get_rating();
};

#endif










































