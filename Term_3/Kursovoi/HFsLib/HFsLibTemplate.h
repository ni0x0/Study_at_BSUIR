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

// ����� ��������� ����������� ������� ���������� ��� ������ � �������-��������� � �����������
class HFsLibTemplate 
{
public:
	// �������� ������ �� ������ � ����-������
	template<class T>
	static void add_filmInfo_in_listWidg(QListWidget* _listWidg, Ring<T>* _ring);

	// ���������� ��������� �������, ���������� ������� ������
	template<class T>
	static void show_searched_filmInfo_in_listWidg(QListWidget* _listWidg, Ring<T>* _ring, T& l_obj, T& h_obj);

	//                       ����������� ������ ��:
	template<class T>                        // �����  
	static void sort_by_name_template(QListWidget* _listWidg, Ring<T>* _ring);
	template<class T>                        // �� ID  
	static void sort_by_id_template(QListWidget* _listWidg, Ring<T>* _ring, bool _is_increase);
	template<class T>                         // �� ������ ������������
	static void sort_by_country_template(QListWidget* _listWidg, Ring<T>* _ring);
	template<class T>                         // ���� ������������
	static void sort_by_year_template(QListWidget* _listWidg, Ring<T>* _ring, bool _is_increase);
	template<class T>                         // �� �����
	static void sort_by_genre_template(QListWidget* _listWidg, Ring<T>* _ring);
	template<class T>                         // ��������
	static void sort_by_rating_template(QListWidget* _listWidg, Ring<T>* _ring);

	// �������� ID ���������� ������ �� ������
	template<class T>                         
	static int get_random_film_id(Ring<T>* _ring, InformationDepot* info_d);
};

#endif;
