#ifndef FILM_CONST_INFO_H
#define FILM_CONST_INFO_H

#include <QDialog>
#include "ui_FilmConstInfo.h"
#include <qmessagebox.h>
#include "InformationDepot.h"
  
QT_BEGIN_NAMESPACE
namespace Ui { class FilmConstInfoClass; };
QT_END_NAMESPACE

class FilmConstInfo : public QDialog     // класс FilmConstInfo, унаследованный от QT класса  QDialog -
{                                        // окно для просмотра информации о фильме
	Q_OBJECT                             // макрос, позволяющий работать с макросами QT
		 
public:
	FilmConstInfo(InformationDepot* dep, int _id, QWidget *parent = nullptr);   // конструктор
	~FilmConstInfo();                                                           // деструктор

private:
	Ui::FilmConstInfoClass *ui;                                                 // указатель на объект "User Interface"
};

#endif
