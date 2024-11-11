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

class FilmMarkNInfoWind : public QDialog  // ����� FilmMarkNInfoWind, �������������� �� QT ������  QDialog -
{                                         // ���� ��� ����������� � ����������� ������������
	Q_OBJECT                              // ������, ����������� �������� � ��������� QT

public:
	FilmMarkNInfoWind(int _mode, int* _mark, QWidget *parent = nullptr);  // �����������
	~FilmMarkNInfoWind();                                                 // ����������

	void set_second_mark(int* _mark);     // ������ ���. ���������� �������
	void set_movieStopPointMode();        // ���������� ����� ������ � �������� � �������������� ������
private slots:
	void add_film_rat_mark();             // �������� ����������� �������
	void add_stopSerial_ep();             // �������� ������� � ����� ��������� � ��������� � �������
	void add_stopMovie_time();            // �������� ������� � ����� ��������� � ��������� � ���������. ������

	void fill_fields(int _mode);          // ��������� ����
private:
	Ui::FilmMarkNInfoWindClass *ui;                 // ��������� �� ������ "User Interface"

	int* markInfo;                                  // ���������� � �������
	int* markInfo2;                                 // �������������� ���������� �������
};

#endif