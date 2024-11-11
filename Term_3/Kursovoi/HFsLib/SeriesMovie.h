#ifndef SERIES_MOVIE_H
#define SERIES_MOVIE_H
#include "Movie.h"

enum SeriesMovietype {          // тип серийного фильма
	sm_type_not_selected,       // не выбран  
	original,                   // оригинал (первый фильм из серии)
	sequel,                     // сиквел   (продолжение)
	prequel,                    // приквел  (предыстория)
	spinoff                     // спин-офф (история второстепенных персонажей из той же вселенной)
};

// класс с информацией о полнометражном фильме из серии фильмов
class SeriesMovie : public Movie
{
protected:
	SeriesMovietype seriaStatus;        // статус в серии
	int seriaNum;                       // номер в серии

	static int seria_movie_id_counter;  // статический ID счетчик серийных полнометражных фильмов
public:
	SeriesMovie();                      // конструкторы
	SeriesMovie(const char* _name, int _prod_year, const char* _country, genres _genre,
		int _hour, int _minute, SeriesMovietype _type, int _seria);
	SeriesMovie(const SeriesMovie& oth);
	virtual ~SeriesMovie() {}

	SeriesMovie& operator = (SeriesMovie& oth);                  // перегрузка оператора присваивания
	bool operator == (const SeriesMovie& oth);                   // перегрузка оператора "равно"
	bool operator <= (const SeriesMovie& oth);                   // перегрузка оператора  "меньше равно"
	bool operator >= (const SeriesMovie& oth);                   // перегрузка оператора  "больше равно"

	friend fstream& operator << (fstream& out, SeriesMovie& oth);    // перегрузка оператора записи в бинарный файл
	friend fstream& operator >> (fstream& in, SeriesMovie& oth);     // перегрузка оператора считывания из бинарного файла

	virtual void copy_data(SeriesMovie& oth);       // скопировать все данные другого фильма, кроме рейтинга и ID

	void set_seria_type(SeriesMovietype);           // методы для изменения значений в полях
	void set_seria_num(int);

	SeriesMovietype get_seria_type();               // методы для получения значений из полей
	int get_seria_num();

	static void set_seria_movie_id_c(int);          // статические методы, изменяющие значение счетчика ID
	static int set_seria_movie_id_c();              
};

#endif
