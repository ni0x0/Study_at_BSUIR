#include "FilmQItem.h"

// �����������
FilmQItem::FilmQItem(QString txt, int _id) : QListWidgetItem(txt){
	curr_id = _id;
}

// �������� �������� ID
int FilmQItem::get_filmQItem_id() {
	return curr_id;
}

// ������� � ��������� ������ ���������� �� ������
void FilmQItem::set_ratingMarkStr(int _rMark) {
	QString txt = (this->text()).left(58);
	txt.insert(58, QString::number(_rMark));
	this->setText(txt);
}
// ������� � ��������� ������ ���������� � ����� ��������� � ��������� ��� �������
void FilmQItem::set_serialStopPointStr(int _sEp) {
	QString txt = (this->text()).left(52);
	txt.insert(52, QString::number(_sEp));
	txt.insert(52 + (QString::number(_sEp)).size(), " episode");
	this->setText(txt);
}
// ������� � ��������� ������ ���������� � ����� ��������� � ��������� ��� ������
void FilmQItem::set_movieStopPointStr(int _sHour, int _sMin) {
	QString txt = (this->text()).left(52);
	txt.insert(52, QString::number(_sHour));
	txt.insert(52 + (QString::number(_sHour)).size(), "h ");
	txt.insert(54 + (QString::number(_sHour)).size(), QString::number(_sMin));
	txt.insert(54 + (QString::number(_sHour)).size() + (QString::number(_sMin)).size(), "min");
	this->setText(txt);
}

// �������� ��������� �� ������ ������ �� ID 
FilmQItem* FilmQItem::get_FilmQItem_in_listWidget(QListWidget* _listWidg, int _id) {
	FilmQItem* item_pointer_to_return = NULL;
	FilmQItem* item_pointer_check_list = NULL;

	for (int i = 0; i < _listWidg->count(); i++) {
		item_pointer_check_list = dynamic_cast<FilmQItem*>(_listWidg->item(i));
		if (item_pointer_check_list == NULL)
			break;
		if (item_pointer_check_list->get_filmQItem_id() == _id) {
			item_pointer_to_return = item_pointer_check_list;
			break;
		}
	}

	return item_pointer_to_return;
}