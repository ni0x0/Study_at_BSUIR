#ifndef FILM_N_MARK_INFO_WIND_H
#define FILM_N_MARK_INFO_WIND_H

#include <QDialog>
#include "ui_FilmMarkNInfoWind.h"

#include <QMessageBox>
#include "InputExceptions.h"
#include "CheckInputFunctions.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FilmMarkNInfoWindClass; };
QT_END_NAMESPACE

class FilmMarkNInfoWind : public QDialog  // класс FilmMarkNInfoWind, унаследованный от QT класса  QDialog -
{                                         // окно для регистрации и авторизации пользователя
	Q_OBJECT                              // макрос, позволяющий работать с макросами QT

public:
	FilmMarkNInfoWind(int _mode, int* _mark, QWidget *parent = nullptr);  // конструктор
	~FilmMarkNInfoWind();                                                 // деструктор

	void set_second_mark(int* _mark);     // задать доп. информацию заметки
	void set_movieStopPointMode();        // установить режим работы с заметкой о полнометражном фильме
private slots:
	void add_film_rat_mark();             // добавить рейтинговую заметку
	void add_stopSerial_ep();             // добавить заметку о точке остановки в просмотре в сериале
	void add_stopMovie_time();            // добавить заметку о точке остановки в просмотре в полнометр. фильме

	void fill_fields(int _mode);          // заполнить поля
private:
	Ui::FilmMarkNInfoWindClass *ui;                 // указатель на объект "User Interface"

	int* markInfo;                                  // информация о заметке
	int* markInfo2;                                 // дополнительная информация заметки
};

#endif