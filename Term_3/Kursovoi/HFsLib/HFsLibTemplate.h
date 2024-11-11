#ifndef HFLIB_TEMPLATE_H
#define  HFLIB_TEMPLATE_H

#include <random>
#include <QtWidgets/QMainWindow>
#include "ui_HFsLib.h"

#include "AuthorWindow.h"
#include "FilmMarkNInfoWind.h"
#include "FilmConstInfo.h"
#include "FilmInfo.h"

#include "FilmQItem.h"

// класс шаблонных статических методов алгоритмов для работы с листами-виджетами и контейнером
class HFsLibTemplate 
{
public:
	// добавить фильмы из кольца в лист-виджет
	template<class T>
	static void add_filmInfo_in_listWidg(QListWidget* _listWidg, Ring<T>* _ring);

	// установить видимость фильмов, подходящих условию поиска
	template<class T>
	static void show_searched_filmInfo_in_listWidg(QListWidget* _listWidg, Ring<T>* _ring, T& l_obj, T& h_obj);

	//                       сортировать фильмы по:
	template<class T>                        // имени  
	static void sort_by_name_template(QListWidget* _listWidg, Ring<T>* _ring);
	template<class T>                        // по ID  
	static void sort_by_id_template(QListWidget* _listWidg, Ring<T>* _ring, bool _is_increase);
	template<class T>                         // по стране производства
	static void sort_by_country_template(QListWidget* _listWidg, Ring<T>* _ring);
	template<class T>                         // году производства
	static void sort_by_year_template(QListWidget* _listWidg, Ring<T>* _ring, bool _is_increase);
	template<class T>                         // по жанру
	static void sort_by_genre_template(QListWidget* _listWidg, Ring<T>* _ring);
	template<class T>                         // рейтингу
	static void sort_by_rating_template(QListWidget* _listWidg, Ring<T>* _ring);

	// получить ID случайного фильма из списка
	template<class T>                         
	static int get_random_film_id(Ring<T>* _ring, InformationDepot* info_d);
};

#endif;
