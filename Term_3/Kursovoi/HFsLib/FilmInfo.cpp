#include "FilmInfo.h"

FilmInfo::FilmInfo(InformationDepot* dep, int _film_p, int _searched_id, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::FilmInfoClass())
{
	ui->setupUi(this);                                     // создание и настройка пользовательского интерфейса
	ui->radB_comedyG->setChecked(true);                    // установка радио-кнопок по умолчанию
	ui->radB_original->setChecked(true);
	this->setStyleSheet("background-color:lightblue");     // установка цвета окна

	this->dep_p = dep;                                     // привязка хранилища с информацией
	film_page = _film_p;                                   // сохранение нужной страницы (0 - полнометр. ф., 1 - сериалы, 2 - серийн. ф.)  
	f_id = _searched_id;                                   // сохранение ID фильма/режима (обычный id - редактирование, -1 - добавление, -2 - параметры поиска)
	enter_is_correct = true;

	if (_film_p == 0)                                      // установка нужной страницы
		ui->lb_FilmNameInfo->setText("Single Movie Info");
	else if (_film_p == 1) 
		ui->lb_FilmNameInfo->setText("Serial Info");
	else if (_film_p == 2)
		ui->lb_FilmNameInfo->setText("Seria Movie Info");
	ui->stWidg_specInfo->setCurrentIndex(_film_p);

	if (_searched_id != -2) {                             // обычный id - редактирование, -1 - добавление, -2 - параметры поиска
		ui->radB_notSelectedG->hide();                    // если окно не для параметров поиска фильма
		ui->radB_notselectedType->hide();                 // прячем объекты, предназначенные для поиска
		ui->lnEd_prodYear_2->hide();
		ui->lnEd_hoursAmount_2->hide();
		ui->lneEd_minsAmount_2->hide();
		ui->lnEd_hoursAmount_serM_2->hide();
		ui->lnEd_minsAmount_serM_2->hide();
		ui->lnEd_seriaNum_serM_2->hide();
		ui->lnEd_seasonNum_2->hide();
		ui->lnEd_epsAmount_2->hide();
		ui->lnEd_epLen_2->hide();
		ui->lb_ratMark->hide();
		ui->lnEd_ratMark->hide();
		ui->lnEd_ratmark_2->hide();
	}
	                                                      // в зависимости от режима устанавливается разная обработка кноки "ok"
	if (_searched_id == -1)                               // добавить информацию о фильме                        
		connect(ui->pB_ok, SIGNAL(clicked()), this, SLOT(add_chosen_film_info()));
	if (_searched_id == -2)                               // установить параметры для поиска
		connect(ui->pB_ok, SIGNAL(clicked()), this, SLOT(search_params_info()));
	if(_searched_id != -1 && _searched_id != -2)          // редактировать информацию о фильме
		connect(ui->pB_ok, SIGNAL(clicked()), this, SLOT(redact_chosen_film_info()));

	if (_searched_id != -1 && _searched_id != -2)         // если производится не добавление, 
		fill_with_chosen_info(1);                         // происходит заполнение полей нужной информацией
	if (_searched_id == -2) {                             // при установке параметров поиска
		fill_with_search_info();                          // также по умолчанию устанавливаются радио-кнопки "не выбрано"
		ui->radB_notSelectedG->setChecked(true);
		ui->radB_notselectedType->setChecked(true);
	}
}

FilmInfo::~FilmInfo()
{
	delete ui;
}

// слот: добавить информацию о выбранном фильме
void FilmInfo::add_chosen_film_info() {
	if (film_page == 0)
		add_single_movie_info();
	if (film_page == 1)
		add_serial_info();
	if (film_page == 2)
		add_seria_movie_info();
}

// добавить информацию об одиночном полнометражном фильме
void FilmInfo::add_single_movie_info() {
	Movie returned_movie = get_movie_info(1);       // считать информацию об одиночном полнометражном фильме
	if (enter_is_correct == false)
		return;

	QMessageBox::information(this, "", "Information is successfully added!");
	this->close();
	Movie movie_to_add(returned_movie.get_name(), returned_movie.get_prod_year(), returned_movie.get_prod_country(),
		returned_movie.get_genre(), returned_movie.get_hour(), returned_movie.get_minute());
	(*dep_p).moviesInfo_r.add(movie_to_add);
}

// добавить информацию о сериале
void FilmInfo::add_serial_info() { 
	Serial returned_serial = get_serial_info(1);   // считать информацию о сериале
	if (enter_is_correct == false)
		return;

	QMessageBox::information(this, "", "Information is successfully added!");
	this->close();
	Serial serial_to_add(returned_serial.get_name(), returned_serial.get_prod_year(), returned_serial.get_prod_country(),
		returned_serial.get_genre(), returned_serial.get_seasonNum(), returned_serial.get_episodesAmount(), returned_serial.get_episodeLenght());
	(*dep_p).serialsInfo_r.add(serial_to_add);
}

// добавить информацию о серийном полнометражном фильме
void FilmInfo::add_seria_movie_info() {
	SeriesMovie returned_movie = get_seria_movie_info(1);   // получить информацию о серийном полнометражном фильме
	if (enter_is_correct == false)
		return;

	QMessageBox::information(this, "", "Information is successfully added!");
	this->close();
	SeriesMovie seria_movie_to_add(returned_movie.get_name(), returned_movie.get_prod_year(), returned_movie.get_prod_country(),
		returned_movie.get_genre(), returned_movie.get_hour(), returned_movie.get_minute(), 
		returned_movie.get_seria_type(), returned_movie.get_seria_num());
	(*dep_p).seriesMoviesInfo_r.add(seria_movie_to_add);
}

// получить общую информацию о фильме
Film& FilmInfo::get_film_info(bool _is_first) {          // булева переменная указывает на то, из первого или второго поля(если их 2) считать информацию 
	char buff_name[FILM_NAME_LEN];                       // буффер, куда будет занесено и проверено имя фильма
	int buff_prod_year;                                  // буффер, куда будет занесен и проверен год производства
	char buff_production_country[COUNTRY_LEN];           // буффер, куда будет занесено и проверено название страны 
	genres buff_genre;                                   // буффер, куда будет занесен и проверен жанр

	try {
		QString enter_line = ui->lnEd_filmName->text();                  // считывание и проверка имени фильма
		if (enter_line.size() > FILM_NAME_LEN - 1)
			throw InputException(0, "Name_field is too long");
		if (enter_line == "" && f_id != -2)
			throw InputException(1, "Name_field is empty");
		QByteArray ba = enter_line.toLocal8Bit();                        // преобразование QString в char[]
		char* ch_buff = ba.data();
		strcpy_s(buff_name, FILM_NAME_LEN, ch_buff);
		if (CheckInputFunctions::check_on_title(buff_name) == false)
			throw InputException(2, "Name_field can contain only eng letters, digits, spaces, dashes, commas, colons");

		if (_is_first == true) enter_line = ui->lnEd_prodYear->text();  // считывание и проверка года производства
		else enter_line = ui->lnEd_prodYear_2->text();
		if ((enter_line.size() != 4 && f_id != -2) || (f_id == -2 && enter_line.isEmpty() == false && enter_line.size() != 4))
			throw InputException(0, "ProductionYear_field must contain 4 digits");
		if (enter_line == "" && f_id != -2)
			throw InputException(1, "ProductionYear_field is empty");
		ba = enter_line.toLocal8Bit();                                  // преобразование QString в char*
		ch_buff = ba.data();              
		if (CheckInputFunctions::check_on_digits(ch_buff) == false)
			throw InputException(2, "ProductionYear_field must contain only digits");
		buff_prod_year = enter_line.toInt();                            // преобразование QString в int
		if ((buff_prod_year < 1700 || buff_prod_year > 2030) && buff_prod_year != 0)
			throw InputException(2, "ProductionYear_field has minimum value - 1700 and maximum - 2030");

		enter_line = ui->lnEd_prodCountry->text();                      // считывание и проверка страны производства
		if (enter_line.size() > COUNTRY_LEN - 1)
			throw InputException(0, "ProductionCountry_field is too long");
		if (enter_line == "" && f_id != -2)
			throw InputException(1, "ProductionCountry_field is empty");
		ba = enter_line.toLocal8Bit();                                  // преобразование QString в char[]
		ch_buff = ba.data(); 
		strcpy_s(buff_production_country, COUNTRY_LEN, ch_buff);
		if (CheckInputFunctions::check_on_eng_letters_spaces(buff_production_country) == false)
			throw InputException(2, "ProductionCountry_field can contain only english letters and spaces");

		if (ui->radB_comedyG->isChecked() == true)                      // обработка выбранной радиокнопки
			buff_genre = comedy;
		else if (ui->radB_fantasyG->isChecked() == true)
			buff_genre = fantasy;
		else if (ui->radB_warG->isChecked() == true)
			buff_genre = war;
		else if (ui->radB_fantasticG->isChecked() == true)
			buff_genre = fantastic;
		else if (ui->radB_horrorG->isChecked() == true)
			buff_genre = horror;
		else if (ui->radB_romanticG->isChecked() == true)
			buff_genre = romantic;
		else if (ui->radB_dramaG->isChecked() == true)
			buff_genre = drama;
		else if (ui->radB_militanrG->isChecked() == true)
			buff_genre = militant;
		else
			buff_genre = genre_not_selected;
	}
	catch (InputException excp)
	{
		QMessageBox::information(this, "", excp.get_exp_msg());
		enter_is_correct = false;
		Film emptyFilm;
		return emptyFilm;
	}
	                                                              // создание фильма с полученной информацией
	Film film_to_return(buff_name, buff_prod_year, buff_production_country, buff_genre);
	enter_is_correct = true;
	return film_to_return;
}

// получить информацию об одиночном полнометражном фильме
Movie& FilmInfo::get_movie_info(bool _is_first) {    // булева переменная указывает на то, из первого или второго поля(если их 2) считать информацию 
	Film returned_film = get_film_info(_is_first);   // считывание общей информации о фильме
	Movie movie_to_return;
	if (enter_is_correct == false)
		return  movie_to_return;

	int buff_hour;                                   // буффер, куда будет занесен и проверен час
	int buff_minute;                                 // буффер, куда будет занесена и проверена минута

	try{
		QString enter_line = ui->lnEd_hoursAmount->text();        // считывание и проверка часа
		if (_is_first == false) enter_line = ui->lnEd_hoursAmount_2->text();
		if (enter_line.size() > 2)
			throw InputException(0, "HoursAmount_field is too long");
		if (enter_line == "" && f_id != -2)
			throw InputException(1, "HoursAmount_field is empty");
		QByteArray ba = enter_line.toLocal8Bit();                // преобразование QString в char*
		char* ch_buff = ba.data();
		if (CheckInputFunctions::check_on_digits(ch_buff) == false)
			throw InputException(2, "HoursAmount_field must contain only digits");
		buff_hour = enter_line.toInt();                          // преобразование QString в int
		if (buff_hour < 0 || buff_hour > 30)
			throw InputException(2, "HoursAmount_field has minimum value - 0 and maximum - 30");
		if (enter_line.isEmpty() == true)
			buff_hour = -1;

		enter_line = ui->lneEd_minsAmount->text();               // считывание и проверка минуты
		if (_is_first == false) enter_line = ui->lneEd_minsAmount_2->text();
		if (enter_line.size() > 2)
			throw InputException(0, "MinutesAmount_field is too long");
		if (enter_line == "" && f_id != -2)
			throw InputException(1, "MinutesAmount_field is empty");
		ba = enter_line.toLocal8Bit();                           // преобразование QString в char*
		ch_buff = ba.data();
		if (CheckInputFunctions::check_on_digits(ch_buff) == false)
			throw InputException(2, "MinutesAmount_field must contain only digits");
		buff_minute = enter_line.toInt();                        // преобразование QString в int
		if (buff_minute < 0 || buff_minute > 60)
			throw InputException(2, "MinutesAmount_field has minimum value - 0 and maximum - 60");
		if (enter_line.isEmpty() == true)
			buff_minute = -1;

	}
	catch (InputException excp)
	{
		QMessageBox::information(this, "", excp.get_exp_msg());
		enter_is_correct = false;
		return movie_to_return;
	}
	                                                      // занесение полученной информации
	dynamic_cast<Film&>(movie_to_return).operator=(returned_film);
	movie_to_return.set_hour(buff_hour);
	movie_to_return.set_minute(buff_minute);
	enter_is_correct = true;
	return movie_to_return;
}

// получить информацию о сериале
Serial& FilmInfo::get_serial_info(bool _is_first) { // булева переменная указывает на то, из первого или второго поля(если их 2) считать информацию 
	Film returned_film = get_film_info(_is_first);  // считывание общей информации о фильме
	Serial serial_to_return;
	if (enter_is_correct == false)
		return  serial_to_return;

	int buff_season_num;                             // буффер, куда будет занесен и проверен номер сезона
	int buff_eps_num;                                // буффер, куда будет занесено и проверено количество серий
	int buff_ep_len;                                 // буффер, куда будет занесена и проверена длина серии

	try {
		QString enter_line = ui->lnEd_seasonNum->text();                    // считывание и проверка номера сезона
		if (_is_first == false) enter_line = ui->lnEd_seasonNum_2->text();
		if (enter_line.size() > 2)
			throw InputException(0, "SeasonNumber_field is too long");
		if (enter_line == "" && f_id != -2)
			throw InputException(1, "SeasonNumber_field is empty");
		QByteArray ba = enter_line.toLocal8Bit();                           // преобразование QString в char*
		char* ch_buff = ba.data();
		if (CheckInputFunctions::check_on_digits(ch_buff) == false)
			throw InputException(2, "SeasonNumber_field must contain only digits");
		buff_season_num = enter_line.toInt();                               // преобразование QString в int
		if ((f_id != -2 && buff_season_num < 1) || buff_season_num > 50)
			throw InputException(2, "SeasonNumber_field has minimum value - 1 and maximum - 50");

		enter_line = ui->lnEd_epsAmount->text();                            // считывание и проверка количества серий
		if (_is_first == false) enter_line = ui->lnEd_epsAmount_2->text();
		if (enter_line.size() > 6)
			throw InputException(0, "EpisodesAmount_field is too long");
		if (enter_line == "" && f_id != -2)
			throw InputException(1, "EpisodesAmount_field is empty");
		ba = enter_line.toLocal8Bit();                                      // преобразование QString в char*
		ch_buff = ba.data();
		if (CheckInputFunctions::check_on_digits(ch_buff) == false)
			throw InputException(2, "EpisodesAmount_field must contain only digits");
		buff_eps_num = enter_line.toInt();                                 // преобразование QString в int
		if ((f_id != -2 && buff_eps_num < 1) || buff_eps_num > 999999)
			throw InputException(2, "EpisodesAmount_field has minimum value - 1 and maximum - 999999");

		enter_line = ui->lnEd_epLen->text();                                // считывание и проверка длины серии
		if (_is_first == false) enter_line = ui->lnEd_epLen_2->text();
		if (enter_line.size() > 3)
			throw InputException(0, "EpisodeLenght_field is too long");
		if (enter_line == "" && f_id != -2)
			throw InputException(1, "EpisodeLenght_field is empty");
		ba = enter_line.toLocal8Bit();                                      // преобразование QString в char*
		ch_buff = ba.data();
		if (CheckInputFunctions::check_on_digits(ch_buff) == false)
			throw InputException(2, "EpisodesAmount_field must contain only digits");
		buff_ep_len = enter_line.toInt();                                   // преобразование QString в int
		if ((f_id != -2 && buff_ep_len < 1) || buff_ep_len > 180)
			throw InputException(2, "EpisodeLenght_field has minimum value - 1 and maximum - 180");

	}
	catch (InputException excp)
	{
		QMessageBox::information(this, "", excp.get_exp_msg());
		enter_is_correct = false;
		return serial_to_return;
	}

	dynamic_cast<Film&>(serial_to_return).operator=(returned_film);       // занесение полученной информации
	serial_to_return.set_seasonNum(buff_season_num);
	serial_to_return.set_episodesAmount(buff_eps_num);
	serial_to_return.set_episodeLenght(buff_ep_len);
	enter_is_correct = true;
	return serial_to_return;
}

// получить информацию о серийном полнометражном фильме
SeriesMovie& FilmInfo::get_seria_movie_info(bool _is_first) { // булева переменная указывает на то, из первого или второго поля(если их 2) считать информацию 
	Film returned_film = get_film_info(_is_first);            // считывание общей информации о фильме
	SeriesMovie seria_movie_to_return;
	if (enter_is_correct == false)
		return  seria_movie_to_return;

	int buff_hour;                                            // буффер, куда будет занесен и проверен час
	int buff_minute;                                          // буффер, куда будет занесена и проверена минута
	int buff_serNum;                                          // буффер, куда будет занесен и проверен фильма в серии
	SeriesMovietype buff_type;                                // буффер, куда будет занесен и проверен тип серийного фильма

	try {
		QString enter_line = ui->lnEd_hoursAmount_serM->text();       // считывание и проверка часа
		if (_is_first == false) enter_line = ui->lnEd_hoursAmount_serM_2->text();
		if (enter_line.size() > 2)
			throw InputException(0, "HoursAmount_field is too long");
		if (enter_line == "" && f_id != -2)
			throw InputException(1, "HoursAmount_field is empty");
		QByteArray ba = enter_line.toLocal8Bit();                     // преобразование QString в char*
		char* ch_buff = ba.data();
		if (CheckInputFunctions::check_on_digits(ch_buff) == false)
			throw InputException(2, "HoursAmount_field must contain only digits");
		buff_hour = enter_line.toInt();                               // преобразование QString в int
		if (buff_hour < 0 || buff_hour > 30)
			throw InputException(2, "HoursAmount_field has minimum value - 0 and maximum - 30");
		if (enter_line.isEmpty() == true)
			buff_hour = -1;

		enter_line = ui->lnEd_minsAmount_serM->text();                 // считывание и проверка часа
		if (_is_first == false) enter_line = ui->lnEd_minsAmount_serM_2->text();
		if (enter_line.size() > 2)
			throw InputException(0, "MinutesAmount_field is too long");
		if (enter_line == "" && f_id != -2)
			throw InputException(1, "MinutesAmount_field is empty");
		ba = enter_line.toLocal8Bit();                                 // преобразование QString в char*
		ch_buff = ba.data();
		if (CheckInputFunctions::check_on_digits(ch_buff) == false)
			throw InputException(2, "MinutesAmount_field must contain only digits");
		buff_minute = enter_line.toInt();                              // преобразование QString в int
		if (buff_minute < 0 || buff_minute > 60)
			throw InputException(2, "MinutesAmount_field has minimum value - 0 and maximum - 60");
		if (enter_line.isEmpty() == true)
			buff_minute = -1;

		enter_line = ui->lnEd_seriaNum_serM->text();                   // считывание и проверка номера в серии
		if (_is_first == false) enter_line = ui->lnEd_seriaNum_serM_2->text();
		if (enter_line.size() > 2)
			throw InputException(0, "SeriaNumber_field is too long");
		if (enter_line == "" && f_id != -2)
			throw InputException(1, "SeriaNumber_field is empty");
		ba = enter_line.toLocal8Bit();                                 // преобразование QString в char*
		ch_buff = ba.data();
		if (CheckInputFunctions::check_on_digits(ch_buff) == false)
			throw InputException(2, "SeriaNumber_field must contain only digits");
		buff_serNum = enter_line.toInt();                              // преобразование QString в int
		if ((f_id != -2 && buff_serNum < 1) || buff_serNum > 30)
			throw InputException(2, "SeriaNumber has minimum value - 0 and maximum - 30");

		                                                               // обработка выбранной радио-кнопки
		if (ui->radB_original->isChecked() == true)                    // для получения информации о типе серийного фильма
			buff_type = original;
		else if (ui->radB_prequel->isChecked() == true)
			buff_type = prequel;
		else if (ui->radB_sequel->isChecked() == true)
			buff_type = sequel;
		else if (ui->radB_spinoff->isChecked() == true)
			buff_type = spinoff;
		else buff_type = sm_type_not_selected;

	}
	catch (InputException excp)
	{
		QMessageBox::information(this, "", excp.get_exp_msg());
		enter_is_correct = false;
		return seria_movie_to_return;
	}

	dynamic_cast<Film&>(seria_movie_to_return).operator=(returned_film); // занесение полученной информации
	seria_movie_to_return.set_hour(buff_hour);
	seria_movie_to_return.set_minute(buff_minute);
	seria_movie_to_return.set_seria_type(buff_type);
	seria_movie_to_return.set_seria_num(buff_serNum);
	enter_is_correct = true;
	return seria_movie_to_return;
}

// редактировать информацию в выбранном фильме 
void FilmInfo::redact_chosen_film_info() {
	Film* chos_film = (*dep_p).find_film(f_id);

	if (film_page == 0)
		redact_single_movie_info(dynamic_cast<Movie*>(chos_film));
	if (film_page == 1)
		redact_serial_info(dynamic_cast<Serial*>(chos_film));
	if (film_page == 2)
		redact_seria_movie_info(dynamic_cast<SeriesMovie*>(chos_film));
}

// редактировать информацию об одиночном полнометражном фильме
void FilmInfo::redact_single_movie_info(Movie* _mov) {
	Movie returned_movie = get_movie_info(1);
	if (enter_is_correct == false)
		return;

	QMessageBox::information(this, "", "Information is successfully redacted!");
	this->close();
	(*_mov).copy_data(returned_movie);
}

// редактировать информацию о сериале
void FilmInfo::redact_serial_info(Serial* _serial) {
	Serial returned_serial = get_serial_info(1);
	if (enter_is_correct == false)
		return;

	QMessageBox::information(this, "", "Information is successfully redacted!");
	this->close();
	(*_serial).copy_data(returned_serial);
}

// редактировать информацию о серийном полнометражном фильме
void FilmInfo::redact_seria_movie_info(SeriesMovie* _serMov) {
	SeriesMovie returned_movie = get_seria_movie_info(1);
	if (enter_is_correct == false)
		return;

	QMessageBox::information(this, "", "Information is successfully redacted!");
	this->close();
	(*_serMov).copy_data(returned_movie);
}

// получить информацию для поиска фильма
void FilmInfo::search_params_info() {
	if (film_page == 0)
		search_movie_params_info();
	if (film_page == 1)
		search_serial_params_info();
	if (film_page == 2)
		search_serMovie_params_info();
}

// получить информацию для поиска одиночного полнометражного фильма
void FilmInfo::search_movie_params_info() {
	Movie returned_movie = get_movie_info(1);      // считать информацию для поиска одиночного полнометражного фильма
 	if (enter_is_correct == false) return;         // для "левой" и "правой" границ 
	Movie returned_sec_movie = get_movie_info(0);
	if (enter_is_correct == false) return;
	Movie empty_movie;

	int buff_rat_mark = this->get_rating(1);      // считать и установить информацию о рейтинге для поиска
	if (enter_is_correct == false) return;        
	returned_movie.set_rating((float)(buff_rat_mark));
	buff_rat_mark = this->get_rating(0);
	if (enter_is_correct == false) return;
	returned_sec_movie.set_rating((float)(buff_rat_mark));

	if (empty_movie == returned_movie && empty_movie == returned_sec_movie) {
		QMessageBox::information(this, "", "You didn't choose Parametres for Searching");
		return;
	}
	if (!(returned_movie <= returned_sec_movie)) {
		QMessageBox::information(this, "", "Some of the First Parametres is bigger than Second ones");
		return;
	}

	QMessageBox::information(this, "", "Movie Search Information is successfully added!");
	this->close();                               // установить параметры поиска
	dep_p->acts.movie_search_lb = returned_movie;
	dep_p->acts.movie_search_hb = returned_sec_movie;
}
// получить информацию для поиска сериала
void FilmInfo::search_serial_params_info() {
	Serial returned_serial = get_serial_info(1);          // считать информацию для поиска сериала
	if (enter_is_correct == false) return;                // для "левой" и "правой" границ 
	Serial returned_sec_serial = get_serial_info(0);
	if (enter_is_correct == false) return;
	Serial empty_movie;

	int buff_rat_mark = this->get_rating(1);              // считать и установить информацию о рейтинге для поиска
	if (enter_is_correct == false) return;
	returned_serial.set_rating((float)(buff_rat_mark));
	buff_rat_mark = this->get_rating(0);
	if (enter_is_correct == false) return;
	returned_sec_serial.set_rating((float)(buff_rat_mark));

	if (empty_movie == returned_serial && empty_movie == returned_sec_serial) {
		QMessageBox::information(this, "", "You didn't choose Parametres for Searching");
		return;
	}
	if (!(returned_serial <= returned_sec_serial)) {
		QMessageBox::information(this, "", "Some of the First Parametres is bigger than Second ones");
		return;
	}

	QMessageBox::information(this, "", "Serial Search Information is successfully added!");
	this->close();
	dep_p->acts.serial_search_lb = returned_serial;        // установить параметры поиска
	dep_p->acts.serial_search_hb = returned_sec_serial;
}

// получить информацию для поиска серийного полнометражноого фильма
void FilmInfo::search_serMovie_params_info() {
	SeriesMovie returned_serMovie = get_seria_movie_info(1);      // считать информацию для поиска серийного полнометражного фильма
	if (enter_is_correct == false) return;                        // для "левой" и "правой" границ 
	SeriesMovie returned_sec_serMovie = get_seria_movie_info(0);
	if (enter_is_correct == false) return;
	SeriesMovie empty_SerMovie;

	int buff_rat_mark = this->get_rating(1);
	if (enter_is_correct == false) return;
	returned_serMovie.set_rating((float)(buff_rat_mark));
	buff_rat_mark = this->get_rating(0);
	if (enter_is_correct == false) return;
	returned_sec_serMovie.set_rating((float)(buff_rat_mark));

	if (empty_SerMovie == returned_serMovie && empty_SerMovie == returned_sec_serMovie) {
		QMessageBox::information(this, "", "You didn't choose Parametres for Searching");
		return;
	}
	if (!(returned_serMovie <= returned_sec_serMovie)) {
		QMessageBox::information(this, "", "Some of the First Parametres is bigger than Second ones");
		return;
	}

	QMessageBox::information(this, "", "Seria Movie Search Information is successfully added!");
	this->close();
	dep_p->acts.serMovie_search_lb = returned_serMovie;       // установить параметры поиска
	dep_p->acts.serMovie_search_hb = returned_sec_serMovie;
}

// считать информацию о рейтинге
int FilmInfo::get_rating(bool _is_first) {  // булева переменная указывает на то, из первого или второго поля(если их 2) считать информацию 
	int buff_mark = -1;                     // буффер, куда будет занесен и проверена отметка о рейтинге

	try {
		QString enter_line = ui->lnEd_ratMark->text();          // считывание и проверка отметки о рейтинге
		if (_is_first == false) enter_line = ui->lnEd_ratmark_2->text();
		if (enter_line.size() > 2)
			throw InputException(0, "RatingMark_field is too long");
		if (enter_line.isEmpty() == false) {
			QByteArray ba = enter_line.toLocal8Bit();           // преобразование QString в char*      
			char* ch_buff = ba.data();
			if (CheckInputFunctions::check_on_digits(ch_buff) == false)
				throw InputException(2, "RatingMark_field must contain only digits");
			buff_mark = enter_line.toInt();                     // преобразование QString в int
			if (buff_mark < 0 || buff_mark > 10)
				throw InputException(2, "RatingMark_field has minimum value - 0 and maximum - 10");
		}
		else
			buff_mark = -1;
	}
	catch (InputException excp)
	{
		QMessageBox::information(this, "", excp.get_exp_msg());
		enter_is_correct = false;
		return buff_mark;
	}
	enter_is_correct = true;
	return buff_mark;
}

// заполнить поля данными для поиска
void FilmInfo::fill_with_search_info() {
	fill_with_chosen_info(1);
	fill_with_chosen_info(0);
}

// поличить информацию о границах поиска
Film* FilmInfo::get_search_info_from_depot(bool _is_first) { // буллева переменная указывает на то, данные какой границы брать
	Film* fp_to_ret = NULL;
	if (film_page == 0) {
		if (_is_first)
			fp_to_ret = dynamic_cast<Film*>(&(dep_p->acts.movie_search_lb));
		else fp_to_ret = dynamic_cast<Film*>(&(dep_p->acts.movie_search_hb));
	}
	if (film_page == 1) {
		if (_is_first)
			fp_to_ret = dynamic_cast<Film*>(&(dep_p->acts.serial_search_lb));
		else fp_to_ret = dynamic_cast<Film*>(&(dep_p->acts.serial_search_hb));
	}
	if (film_page == 2) {
		if (_is_first)
			fp_to_ret = dynamic_cast<Film*>(&(dep_p->acts.serMovie_search_lb));
		else fp_to_ret = dynamic_cast<Film*>(&(dep_p->acts.serMovie_search_hb));
	}
	return fp_to_ret;
}

// зполнить поля окна
void FilmInfo::fill_with_chosen_info(bool _is_first) { // булева переменная указывает на то, в первое или второе поля(если их 2) записать информацию 
	Film* chos_film;
	if (f_id != -2)                                           // если задан режим для редактирования
		chos_film = dep_p->find_film(f_id);                   // ищем указатель на фильм в хранилище с фильмами
	else chos_film = get_search_info_from_depot(_is_first);   // получаем информацию о параметре поиска

	ui->lnEd_filmName->setText(chos_film->get_name());        // занесение информации
	if (chos_film->get_prod_year() != 0) {
		if (_is_first == true)
			ui->lnEd_prodYear->setText(QString::number(chos_film->get_prod_year()));
		else ui->lnEd_prodYear_2->setText(QString::number(chos_film->get_prod_year()));
	}
	ui->lnEd_prodCountry->setText(chos_film->get_prod_country());

	genres ch_g = chos_film->get_genre();
	if (ch_g == comedy)
		ui->radB_comedyG->setChecked(true);
	if (ch_g == militant)
		ui->radB_militanrG->setChecked(true);
	if (ch_g == romantic)
		ui->radB_romanticG->setChecked(true);
	if (ch_g == drama)
		ui->radB_dramaG->setChecked(true);
	if (ch_g == horror)
		ui->radB_horrorG->setChecked(true);
	if (ch_g == war)
		ui->radB_warG->setChecked(true);
	if (ch_g == fantasy)
		ui->radB_fantasyG->setChecked(true);
	if (ch_g == fantastic)
		ui->radB_fantasticG->setChecked(true);
	if (ch_g == genre_not_selected)
		ui->radB_notSelectedG->setChecked(true);

	if (chos_film->get_rating() != -1) {
		if (_is_first == true)
			ui->lnEd_ratMark->setText(QString::number(chos_film->get_rating()));
		else ui->lnEd_ratmark_2->setText(QString::number(chos_film->get_rating()));
	}

	if (film_page == 0)
		fill_sinMovie_info(dynamic_cast<Movie*>(chos_film), _is_first);
	if (film_page == 1)
		fill_serial_info(dynamic_cast<Serial*>(chos_film), _is_first);
	if (film_page == 2)
		fill_serMovie_info(dynamic_cast<SeriesMovie*>(chos_film), _is_first);
}

// заполнить информацию об одиночном полнометражном фильме
void FilmInfo::fill_sinMovie_info(Movie* _movie, bool _is_first) {
	if (_movie->get_hour() != -1) {
		if (_is_first)
			ui->lnEd_hoursAmount->setText(QString::number(_movie->get_hour()));
		else ui->lnEd_hoursAmount_2->setText(QString::number(_movie->get_hour()));
	}
	if (_movie->get_minute() != -1) {
		if (_is_first)
			ui->lneEd_minsAmount->setText(QString::number(_movie->get_minute()));
		else ui->lneEd_minsAmount_2->setText(QString::number(_movie->get_minute()));
	}
}

// заполнить информацию о сериале
void FilmInfo::fill_serial_info(Serial* _serial, bool _is_first) {
	if (_serial->get_seasonNum() != 0) {
		if (_is_first)
			ui->lnEd_seasonNum->setText(QString::number(_serial->get_seasonNum()));
		else ui->lnEd_seasonNum_2->setText(QString::number(_serial->get_seasonNum()));
	}
	if (_serial->get_episodesAmount() != 0) {
		if (_is_first)
			ui->lnEd_epsAmount->setText(QString::number(_serial->get_episodesAmount()));
		else ui->lnEd_epsAmount_2->setText(QString::number(_serial->get_episodesAmount()));
	}
	if (_serial->get_episodeLenght() != 0) {
		if (_is_first)
			ui->lnEd_epLen->setText(QString::number(_serial->get_episodeLenght()));
		else ui->lnEd_epLen_2->setText(QString::number(_serial->get_episodeLenght()));
	}
}

// заполнить информацию о сериальном полнометражном фильме
void FilmInfo::fill_serMovie_info(SeriesMovie* _serMovie, bool _is_first) {
	if (_serMovie->get_hour() != -1) {
		if (_is_first)
			ui->lnEd_hoursAmount_serM->setText(QString::number(_serMovie->get_hour()));
		else ui->lnEd_hoursAmount_serM_2->setText(QString::number(_serMovie->get_hour()));
	}
	if (_serMovie->get_minute() != -1) {
		if (_is_first)
			ui->lnEd_minsAmount_serM->setText(QString::number(_serMovie->get_minute()));
		else ui->lnEd_minsAmount_serM_2->setText(QString::number(_serMovie->get_minute()));
	}
	if (_serMovie->get_seria_num() != 0) {
		if (_is_first)
			ui->lnEd_seriaNum_serM->setText(QString::number(_serMovie->get_seria_num()));
		else ui->lnEd_seriaNum_serM_2->setText(QString::number(_serMovie->get_seria_num()));
	}

	SeriesMovietype type = _serMovie->get_seria_type();
	if (type == original)
		ui->radB_original->setChecked(true);
	if (type == spinoff)
		ui->radB_spinoff->setChecked(true);
	if (type == prequel)
		ui->radB_prequel->setChecked(true);
	if (type == sequel)
		ui->radB_sequel->setChecked(true);
	if (type == sm_type_not_selected)
		ui->radB_notselectedType->setChecked(true);
}