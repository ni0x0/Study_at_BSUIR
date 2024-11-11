#include "FilmMarkNInfoWind.h"

// �����������
FilmMarkNInfoWind::FilmMarkNInfoWind(int _mode, int* _mark1, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::FilmMarkNInfoWindClass())
{
	ui->setupUi(this);               // �������� � ��������� ����������������� ����������
	                                 // _mode: 0 - ������ � ����������� �������; 1 - c ������ ��������� � ��������� ������
	                                 // 2 - ������ ��������� � ��������� �������
	ui->stackedWidget->setCurrentIndex(_mode);
	if (_mode == 0)
		ui->lb_action->setText("Add Rating Mark");
	if (_mode == 1 || _mode == 2)
		ui->lb_action->setText("Add Stop Point");

	markInfo = _mark1;               // ��������� �������
	markInfo2 = NULL;

	if (_mode == 0)
		connect(ui->pB_ok, SIGNAL(clicked()), this, SLOT(add_film_rat_mark()));
	if (_mode == 1)
		connect(ui->pB_ok, SIGNAL(clicked()), this, SLOT(add_stopMovie_time()));
	if (_mode == 2)
		connect(ui->pB_ok, SIGNAL(clicked()), this, SLOT(add_stopSerial_ep()));

	if ((*_mark1) != -1 )           // ���� ������� - "�� �� ��������",
		fill_fields(_mode);         // ��������� ���� (�������������� �������)
}

FilmMarkNInfoWind::~FilmMarkNInfoWind()
{
	delete ui;
}

// ������ ���. ���������� �������
void FilmMarkNInfoWind::set_second_mark(int* _mark) {
	markInfo2 = _mark;
	if ((*markInfo2) != -1)         
		ui->lnEd_min->setText(QString::number(*markInfo2));
}
// ���������� ����� ������ � �������� � �������������� ������
void FilmMarkNInfoWind::set_movieStopPointMode() {
	ui->stackedWidget->setCurrentIndex(1);
	disconnect(ui->pB_ok, SIGNAL(clicked()), this, SLOT(add_stopSerial_ep()));
	connect(ui->pB_ok, SIGNAL(clicked()), this, SLOT(add_stopMovie_time()));
}

// �������� ����������� �������
void FilmMarkNInfoWind::add_film_rat_mark() {
	int buff_mark = -1;         // ������, ���� ����� �������� ���������� �� ������

	try {
		QString enter_line = ui->lnEd_addMark->text();  // ���������� � �������� ����������� ������
		if (enter_line.size() > 2)
			throw InputException(0, "Mark_field must contain not more than 2 digits");
		if (enter_line == "")
			throw InputException(1, "Mark_field is empty");
		QByteArray ba = enter_line.toLocal8Bit();      // �������������� QString � char*
		char* ch_buff = ba.data();
		if (CheckInputFunctions::check_on_digits(ch_buff) == false)
			throw InputException(2, "Mark_field must contain not more than 2 digits");

		buff_mark = enter_line.toInt();                // �������������� QString � int
		if (buff_mark < 0 || buff_mark > 10)
			throw InputException(2, "Mark_field must contai digit from 0 to 10");
	}
	catch (InputException excp)
	{
		QMessageBox::information(this, "", excp.get_exp_msg());
		return;
	}

	*markInfo = buff_mark;                            // ���������� ���������� ����������
	QMessageBox::information(this, "", "Rating Mark is successfully added!");
	this->close();
}

// �������� ������� � ����� ��������� � ��������� � �������
void FilmMarkNInfoWind::add_stopSerial_ep() {
	int buff_ep = -1;                                    // ������, ���� ����� ������� ����� �������

	try {
		QString enter_line = ui->lnEd_episode->text();   // ���������� � �������� ������ �������
		if (enter_line.size() > 3)
			throw InputException(0, "Episode_field is too long ");
		if (enter_line == "")
			throw InputException(1, "Episode_field is empty");
		QByteArray ba = enter_line.toLocal8Bit();       // �������������� QString � char*
		char* ch_buff = ba.data();
		if (CheckInputFunctions::check_on_digits(ch_buff) == false)
			throw InputException(2, "Episode_field must contain only digits");

		buff_ep = enter_line.toInt();                   // �������������� QString � int
	}
	catch (InputException excp)
	{
		QMessageBox::information(this, "", excp.get_exp_msg());
		return;
	}

	*markInfo = buff_ep;                               // ���������� ���������� ����������
	QMessageBox::information(this, "", "Serial Stop Point is successfully added!");
	this->close();
}

// �������� ������� � ����� ��������� � ��������� � ���������. ������
void FilmMarkNInfoWind::add_stopMovie_time() {
	int buff_hour = -1;                            // ������, ���� ����� ������� ��� ���������
	int buff_min = -1;                             // ������, ���� ����� �������� ������ ���������

	try {
		QString enter_line = ui->lnEd_hour->text(); // ���������� � �������� ���� ���������
		if (enter_line.size() > 2)
			throw InputException(0, "Hour_field is too long ");
		if (enter_line == "")
			throw InputException(1, "Hour_field is empty");
		QByteArray ba = enter_line.toLocal8Bit();
		char* ch_buff = ba.data();                 // �������������� QString � char*
		if (CheckInputFunctions::check_on_digits(ch_buff) == false)
			throw InputException(2, "Hour_field must contain only digits");
		buff_hour = enter_line.toInt();
		if (buff_hour < 0 || buff_hour > 30)        // �������������� QString � int
			throw InputException(2, "HoursAmount_field has minimum value - 0 and maximum - 30");

		enter_line = ui->lnEd_min->text();          // ���������� � �������� ������ ���������
		if (enter_line.size() > 2)
			throw InputException(0, "Minute_field is too long");
		if (enter_line == "")
		throw InputException(1, "Minute_field is empty");
		ba = enter_line.toLocal8Bit();              // �������������� QString � char*
		ch_buff = ba.data();
		if (CheckInputFunctions::check_on_digits(ch_buff) == false)
			throw InputException(2, "Minute_field must contain only digits");
		buff_min = enter_line.toInt();             // �������������� QString � int
		if (buff_min < 0 || buff_min > 60)
			throw InputException(2, "MinutesAmount_field has minimum value - 0 and maximum - 60");

	}
	catch (InputException excp)
	{
		QMessageBox::information(this, "", excp.get_exp_msg());
		return;
	}

	*markInfo = buff_hour;                  // ���������� ���������� ����������
	*markInfo2 = buff_min;
	QMessageBox::information(this, "", "Movie Stop Point is successfully added!");
	this->close();
}

// ��������� ����
void  FilmMarkNInfoWind::fill_fields(int _mode) {
	if (_mode == 0)
		ui->lnEd_addMark->setText(QString::number(*markInfo));
	else
		ui->lnEd_hour->setText(QString::number(*markInfo));
}