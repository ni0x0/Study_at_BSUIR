#include "FilmMarkNInfoWind.h"

// конструктор
FilmMarkNInfoWind::FilmMarkNInfoWind(int _mode, int* _mark1, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::FilmMarkNInfoWindClass())
{
	ui->setupUi(this);               // создание и настройка пользовательского интерфейса
	                                 // _mode: 0 - работа с рейтинговой оценкой; 1 - c точкой остановки в просмотре фильма
	                                 // 2 - точкой остановки в просмотре сериала
	ui->stackedWidget->setCurrentIndex(_mode);
	if (_mode == 0)
		ui->lb_action->setText("Add Rating Mark");
	if (_mode == 1 || _mode == 2)
		ui->lb_action->setText("Add Stop Point");

	markInfo = _mark1;               // установка отметки
	markInfo2 = NULL;

	if (_mode == 0)
		connect(ui->pB_ok, SIGNAL(clicked()), this, SLOT(add_film_rat_mark()));
	if (_mode == 1)
		connect(ui->pB_ok, SIGNAL(clicked()), this, SLOT(add_stopMovie_time()));
	if (_mode == 2)
		connect(ui->pB_ok, SIGNAL(clicked()), this, SLOT(add_stopSerial_ep()));

	if ((*_mark1) != -1 )           // если заметка - "не по умочанию",
		fill_fields(_mode);         // заполнить поля (редактирование заметки)
}

FilmMarkNInfoWind::~FilmMarkNInfoWind()
{
	delete ui;
}

// задать доп. информацию заметки
void FilmMarkNInfoWind::set_second_mark(int* _mark) {
	markInfo2 = _mark;
	if ((*markInfo2) != -1)         
		ui->lnEd_min->setText(QString::number(*markInfo2));
}
// установить режим работы с заметкой о полнометражном фильме
void FilmMarkNInfoWind::set_movieStopPointMode() {
	ui->stackedWidget->setCurrentIndex(1);
	disconnect(ui->pB_ok, SIGNAL(clicked()), this, SLOT(add_stopSerial_ep()));
	connect(ui->pB_ok, SIGNAL(clicked()), this, SLOT(add_stopMovie_time()));
}

// добавить рейтинговую заметку
void FilmMarkNInfoWind::add_film_rat_mark() {
	int buff_mark = -1;         // буффер, куда будет занесена информация об оценке

	try {
		QString enter_line = ui->lnEd_addMark->text();  // считывание и проверка рейтинговой оценки
		if (enter_line.size() > 2)
			throw InputException(0, "Mark_field must contain not more than 2 digits");
		if (enter_line == "")
			throw InputException(1, "Mark_field is empty");
		QByteArray ba = enter_line.toLocal8Bit();      // преобразование QString в char*
		char* ch_buff = ba.data();
		if (CheckInputFunctions::check_on_digits(ch_buff) == false)
			throw InputException(2, "Mark_field must contain not more than 2 digits");

		buff_mark = enter_line.toInt();                // преобразование QString в int
		if (buff_mark < 0 || buff_mark > 10)
			throw InputException(2, "Mark_field must contai digit from 0 to 10");
	}
	catch (InputException excp)
	{
		QMessageBox::information(this, "", excp.get_exp_msg());
		return;
	}

	*markInfo = buff_mark;                            // сохранение полученной информации
	QMessageBox::information(this, "", "Rating Mark is successfully added!");
	this->close();
}

// добавить заметку о точке остановки в просмотре в сериале
void FilmMarkNInfoWind::add_stopSerial_ep() {
	int buff_ep = -1;                                    // буффер, куда будет занесен номер эпизода

	try {
		QString enter_line = ui->lnEd_episode->text();   // считывание и проверка номера эпизода
		if (enter_line.size() > 3)
			throw InputException(0, "Episode_field is too long ");
		if (enter_line == "")
			throw InputException(1, "Episode_field is empty");
		QByteArray ba = enter_line.toLocal8Bit();       // преобразование QString в char*
		char* ch_buff = ba.data();
		if (CheckInputFunctions::check_on_digits(ch_buff) == false)
			throw InputException(2, "Episode_field must contain only digits");

		buff_ep = enter_line.toInt();                   // преобразование QString в int
	}
	catch (InputException excp)
	{
		QMessageBox::information(this, "", excp.get_exp_msg());
		return;
	}

	*markInfo = buff_ep;                               // сохранение полученной информации
	QMessageBox::information(this, "", "Serial Stop Point is successfully added!");
	this->close();
}

// добавить заметку о точке остановки в просмотре в полнометр. фильме
void FilmMarkNInfoWind::add_stopMovie_time() {
	int buff_hour = -1;                            // буффер, куда будет занесен час остановки
	int buff_min = -1;                             // буффер, куда будет занесена минута остановки

	try {
		QString enter_line = ui->lnEd_hour->text(); // считывание и проверка часа остановки
		if (enter_line.size() > 2)
			throw InputException(0, "Hour_field is too long ");
		if (enter_line == "")
			throw InputException(1, "Hour_field is empty");
		QByteArray ba = enter_line.toLocal8Bit();
		char* ch_buff = ba.data();                 // преобразование QString в char*
		if (CheckInputFunctions::check_on_digits(ch_buff) == false)
			throw InputException(2, "Hour_field must contain only digits");
		buff_hour = enter_line.toInt();
		if (buff_hour < 0 || buff_hour > 30)        // преобразование QString в int
			throw InputException(2, "HoursAmount_field has minimum value - 0 and maximum - 30");

		enter_line = ui->lnEd_min->text();          // считывание и проверка минуты остановки
		if (enter_line.size() > 2)
			throw InputException(0, "Minute_field is too long");
		if (enter_line == "")
		throw InputException(1, "Minute_field is empty");
		ba = enter_line.toLocal8Bit();              // преобразование QString в char*
		ch_buff = ba.data();
		if (CheckInputFunctions::check_on_digits(ch_buff) == false)
			throw InputException(2, "Minute_field must contain only digits");
		buff_min = enter_line.toInt();             // преобразование QString в int
		if (buff_min < 0 || buff_min > 60)
			throw InputException(2, "MinutesAmount_field has minimum value - 0 and maximum - 60");

	}
	catch (InputException excp)
	{
		QMessageBox::information(this, "", excp.get_exp_msg());
		return;
	}

	*markInfo = buff_hour;                  // сохранение полученной информации
	*markInfo2 = buff_min;
	QMessageBox::information(this, "", "Movie Stop Point is successfully added!");
	this->close();
}

// заполнить поля
void  FilmMarkNInfoWind::fill_fields(int _mode) {
	if (_mode == 0)
		ui->lnEd_addMark->setText(QString::number(*markInfo));
	else
		ui->lnEd_hour->setText(QString::number(*markInfo));
}