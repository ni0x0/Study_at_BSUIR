#ifndef FILM_QITEM_H
#define FILM_QITEM_H

#include<QListWidget>
#include<QListWidgetItem>

class FilmQItem: public QListWidgetItem   // ����� FilmQItem, �������������� �� QT ������ QListWidgetItem
{                                         // ��� �������� � ������ ����� ���������� �� ID ������
	int curr_id;
public:
	explicit FilmQItem(QString txt, int _id); // �����������
	int get_filmQItem_id();                   // �������� �������� ID

	void set_ratingMarkStr(int _rMark);                 // ������� � ��������� ������ ���������� �� ������
	void set_serialStopPointStr(int _sEp);              // ������� � ��������� ������ ���������� � ����� ��������� � ��������� ��� �������
	void set_movieStopPointStr(int _sHour, int _sMin);  // ������� � ��������� ������ ���������� � ����� ��������� � ��������� ��� ������

	// �������� ��������� �� ������ ������ �� ID 
	static FilmQItem* get_FilmQItem_in_listWidget(QListWidget* _widg, int _id);
};

#endif
