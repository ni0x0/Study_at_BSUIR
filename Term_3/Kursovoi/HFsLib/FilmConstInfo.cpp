#include "FilmConstInfo.h"

// коструктор 
FilmConstInfo::FilmConstInfo(InformationDepot* dep, int _id, QWidget *parent) 
	: QDialog(parent)
	, ui(new Ui::FilmConstInfoClass())
{
	ui->setupUi(this);                                                         // создание и настройка пользовательского интерфейса

	try
	{
		Film* serched_film_pointer = (*dep).find_film(_id);                        // поиск фильма в хранилище по ID
		if (serched_film_pointer == NULL)
			throw ContainerException(2, "ERROR. Film isn't found in base");
		ui->lb_setName->setText(serched_film_pointer->get_name());                 // заполнение всех полей
		ui->lb_setProdCountry->setText(serched_film_pointer->get_prod_country());
		ui->lb_setProdYear->setText(QString::number(serched_film_pointer->get_prod_year()));
		genres temp_genre = serched_film_pointer->get_genre();
		if (temp_genre == comedy)
			ui->lb_setGenre->setText("comedy");
		if (temp_genre == militant)
			ui->lb_setGenre->setText("militant");
		if (temp_genre == romantic)
			ui->lb_setGenre->setText("romantic");
		if (temp_genre == drama)
			ui->lb_setGenre->setText("drama");
		if (temp_genre == horror)
			ui->lb_setGenre->setText("horror");
		if (temp_genre == war)
			ui->lb_setGenre->setText("war");
		if (temp_genre == fantasy)
			ui->lb_setGenre->setText("fantasy");
		if (temp_genre == fantastic)
			ui->lb_setGenre->setText("fantastic");

		if (serched_film_pointer->get_rating() == -1)
			ui->lb_setRating->setText("has no rating");
		else ui->lb_setRating->setText(QString::number(serched_film_pointer->get_rating()));

		ui->lb_setID->setText(QString::number(serched_film_pointer->get_id()));

		if (_id / 1000 == 0) {                                                      // заполнение полей, касающихся
			ui->lb_FilmNameInfo->setText("Single Movie Info");                      // одиночных полнометражных фильмов

			Movie* m_info = static_cast<Movie*>(serched_film_pointer);
			ui->lb_setHoursAmount->setText(QString::number(m_info->get_hour()));
			ui->lb_setMinsAmount->setText(QString::number(m_info->get_minute()));

		}
		else if (_id / 1000 == 1) {                                                 // заполнение полей, касающихся
			ui->lb_FilmNameInfo->setText("Serial Info");                            // cериалов

			Serial* s_info = static_cast<Serial*>(serched_film_pointer);
			ui->lb_setSeasNum->setText(QString::number(s_info->get_seasonNum()));
			ui->lb_setEpLen->setText(QString::number(s_info->get_episodeLenght()));
			ui->lb_setEpsNum->setText(QString::number(s_info->get_episodesAmount()));
		}
		else if (_id / 1000 == 2) {                                                 // заполнение полей, касающихся
			ui->lb_FilmNameInfo->setText("Seria Movie Info");                       // cерийных полнометражных фильмов

			SeriesMovie* sm_info = static_cast<SeriesMovie*>(serched_film_pointer);
			ui->lb_setHoursAmount_serM->setText(QString::number(sm_info->get_hour()));
			ui->lb_setMinsAmount_serM->setText(QString::number(sm_info->get_minute()));
			SeriesMovietype t_type = sm_info->get_seria_type();
			if (t_type == original)
				ui->lb_setStatus->setText("original");
			if (t_type == spinoff)
				ui->lb_setStatus->setText("spinoff");
			if (t_type == prequel)
				ui->lb_setStatus->setText("prequel");
			if (t_type == sequel)
				ui->lb_setStatus->setText("sequel");
			ui->lb_setSeriaNum->setText((QString::number(sm_info->get_seria_num())));
		}
		ui->stWidg_specInfo->setCurrentIndex(_id / 1000);
	}
	catch (ContainerException excp)	{
		QMessageBox::information(this, "", excp.get_exp_msg());
	}
}

// деструктор
FilmConstInfo::~FilmConstInfo()
{
	delete ui;
}
