#ifndef FILM_INFO_H
#define FILM_INFO_H

#include <QDialog>
#include "ui_FilmInfo.h"

#include <QMessageBox>

#include "InformationDepot.h"
#include "InputExceptions.h"
#include "CheckInputFunctions.h"
                
QT_BEGIN_NAMESPACE
namespace Ui { class FilmInfoClass; };
QT_END_NAMESPACE

class FilmInfo : public QDialog          // класс FilmInfo, унаследованный от QT класса  QDialog -
{                                        // окно для работы с информацией о фильме
	Q_OBJECT                             // макрос, позволяющий работать с макросами QT

public:
	FilmInfo(InformationDepot* dep, int _film_p, int _searched_id, QWidget *parent = nullptr); // конструктор
	~FilmInfo();                                                                               // деструктор

private slots:
	void add_chosen_film_info();             // добавить информацию о выбранном типе фильма
	void add_single_movie_info();            // добавить информацию об одиночном полнометражном фильме
	void add_serial_info();                  // добавить информацию о сериале
	void add_seria_movie_info();             // добавить информацию о серийном полнометражном фильме

	Film& get_film_info(bool _is_first);                   // получить информацию о фильме
	Movie& get_movie_info(bool _is_first);                 // получить информацию об одиночном полнометражном фильме
	Serial& get_serial_info(bool _is_first);               // получить информацию о сериале
	SeriesMovie& get_seria_movie_info(bool _is_first);     // получить информацию о cерийном полнометражном фильме 

	void redact_chosen_film_info();                      // редактировать информацию в выбранном фильме 
	void redact_single_movie_info(Movie* _mov);          // редактировать информацию об одиночном полнометражном фильме
	void redact_serial_info(Serial* _serial);            // редактировать информацию о сериале
	void redact_seria_movie_info(SeriesMovie* _serMov);  // редактировать информацию о серийном полнометражном фильме

	void search_params_info();                // получить информацию для поиска фильма
	void search_movie_params_info();          // получить информацию для поиска одиночного полнометражного фильма
	void search_serial_params_info();         // получить информацию для поиска сериала
	void search_serMovie_params_info();       // получить информацию для поиска серийного полнометражноого фильма
	int get_rating(bool _is_first);           // считать информацию о рейтинге

private:
	Ui::FilmInfoClass *ui;                                // указатель на объект "User Interface"

	InformationDepot* dep_p;                              // указатель на хранилище информаци, обработка которой будет производиться
	int film_page;                                        // номер страницы окна
	int f_id;                                             // ID фильма/режима (обычный id - редактирование, -1 - добавление, -2 - параметры поиска)
	bool enter_is_correct;                                // булева переменная для сохранения информации о правильности ввода 

	void fill_with_search_info();                                // заполнить поля данными для поиска
	Film* get_search_info_from_depot(bool _is_first);            // поличить информацию о границах поиска

	void fill_with_chosen_info(bool _is_first);                                // заполнить поля данными    
	void fill_sinMovie_info(Movie* _movie, bool _is_first);                    // заполнить данными одиночного полнометражного фильма
	void fill_serial_info(Serial* _serial, bool _is_first);                    // заполнить данными сериала
	void fill_serMovie_info(SeriesMovie* _serMovie, bool _is_first);           // заполнить данными серийного полнометражного фильма
	


};

#endif
