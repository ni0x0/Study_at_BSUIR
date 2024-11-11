#ifndef FILM_CONST_INFO_H
#define FILM_CONST_INFO_H

#include <QDialog>
#include "ui_FilmConstInfo.h"
#include <qmessagebox.h>
#include "InformationDepot.h"
  
QT_BEGIN_NAMESPACE
namespace Ui { class FilmConstInfoClass; };
QT_END_NAMESPACE

class FilmConstInfo : public QDialog     // ����� FilmConstInfo, �������������� �� QT ������  QDialog -
{                                        // ���� ��� ��������� ���������� � ������
	Q_OBJECT                             // ������, ����������� �������� � ��������� QT
		 
public:
	FilmConstInfo(InformationDepot* dep, int _id, QWidget *parent = nullptr);   // �����������
	~FilmConstInfo();                                                           // ����������

private:
	Ui::FilmConstInfoClass *ui;                                                 // ��������� �� ������ "User Interface"
};

#endif
