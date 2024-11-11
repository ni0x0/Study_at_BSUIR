#ifndef FILM_SPOINT_MARK_H
#define FILM_SPOINT_MARK_H

#include "FilmMark.h"

// класс "заметок" о фильме с информацией о моменте остановки в просмотре
class FilmStopPointMark : public FilmMark
{ 
	union StopPointInfo {                    // информация о моменте остановки в просмотре
		struct MovieStopWatchingPoint {      // для фильмов:
			int hour;                        //            - час              
			int minute;                      //            - минута
		} in_movie;
		struct SerialStopWatchingPoint {     // для сериалов:
			int episode;                     //             - серия
		} in_serial;
	} stop_point;
public:
	FilmStopPointMark();                     // конструкторв
	FilmStopPointMark(Film* _fp);
	FilmStopPointMark(int  _f_id);
	FilmStopPointMark(Film* _fp, int _sEp);
	FilmStopPointMark(Film* _fp, int _sHour, int _sMin);
	FilmStopPointMark(int _f_id, int _sEp);
	FilmStopPointMark(int _f_id, int _sHour, int _sMin);

	FilmStopPointMark& operator = (FilmStopPointMark& oth);              // перегрузка оператора присваивания
	bool operator == (const FilmStopPointMark& oth);                     // перегрузка оператора "равно"

	friend fstream& operator << (fstream& out, FilmStopPointMark& oth);  // перегрузка оператора записи в бинарный файл
	friend fstream& operator >> (fstream& in, FilmStopPointMark& oth);   // перегрузка оператора считывания из бинарного файла

	void set_episodeM(int _sEp);           // методы для изменения значений в полях
	int get_episodeM();
	void set_hourM(int _sHour);

	int get_hourM();                       // методы для ролучения значений в полях
	void set_minuteM(int _sMin);
	int get_minuteM();
};

#endif






