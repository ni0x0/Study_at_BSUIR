#ifndef FILM_QITEM_H
#define FILM_QITEM_H

#include<QListWidget>
#include<QListWidgetItem>

class FilmQItem: public QListWidgetItem   // класс FilmQItem, унаследованный от QT класса QListWidgetItem
{                                         // для хранения в виджет листе информации об ID фильма
	int curr_id;
public:
	explicit FilmQItem(QString txt, int _id); // конструктор
	int get_filmQItem_id();                   // получить значение ID

	void set_ratingMarkStr(int _rMark);                 // занести в выводимую строку информацию об оценке
	void set_serialStopPointStr(int _sEp);              // занести в выводимую строку информацию о точке остановки в просмотре для сериала
	void set_movieStopPointStr(int _sHour, int _sMin);  // занести в выводимую строку информацию о точке остановки в просмотре для фильма

	// получить указатель на нужный объект по ID 
	static FilmQItem* get_FilmQItem_in_listWidget(QListWidget* _widg, int _id);
};

#endif
