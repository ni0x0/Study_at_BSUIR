#include "HFsLib.h"

HFsLib::HFsLib(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HFsLibClass())
{
    ui->setupUi(this);                              // создание и настройка пользовательского интерфейса
    setWindowTitle("HFL");                          // установка названия приложения
    setStyleSheet("background-color:lightblue");    // установка названия приложения
    ui->stWd_main->setCurrentIndex(0);              // установка начальной страницы

    // подключение возможности вызова меню по нажатию правой кнопки мыши 
    ui->centralWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->centralWidget, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showContextMenu()));
 
    // связь со слотами на главной странице 
    connect(ui->pB_goToUs_lists, SIGNAL(clicked()), this, SLOT(go_to_user_lists()));
    connect(ui->pB_goToFBase, SIGNAL(clicked()), this, SLOT(go_to_film_lib()));
    connect(ui->act_saveLib, SIGNAL(triggered()), this, SLOT(save_progress()));

    // связь со слотами на странице - библиотеке
    connect(ui->pB_back, SIGNAL(clicked()), this, SLOT(go_back_to_main_page()));
    connect(ui->pB_add, SIGNAL(clicked()), this, SLOT(add_film_info()));
    connect(ui->pB_see_info, SIGNAL(clicked()), this, SLOT(see_film_info()));
    connect(ui->pB_del_info, SIGNAL(clicked()), this, SLOT(del_film_info()));
    connect(ui->pB_redact_inf, SIGNAL(clicked()), this, SLOT(redact_film_info()));
    connect(ui->pB_search, SIGNAL(clicked()), this, SLOT(search_film()));
    connect(ui->pB_freeSearch, SIGNAL(clicked()), this, SLOT(free_search_film()));
    connect(ui->pB_sort, SIGNAL(clicked()), this, SLOT(show_sort_menu()));
    connect(ui->pB_cancelAction, SIGNAL(clicked()), this, SLOT(cancel_action()));
    connect(ui->pB_randomFilm, SIGNAL(clicked()), this, SLOT(get_random_film()));
    
    // связь со слотами на странице пользователя
    connect(ui->pB_back_user, SIGNAL(clicked()), this, SLOT(go_back_to_main_page()));
    connect(ui->pB_leave_account, SIGNAL(clicked()), this, SLOT(leave_account()));

    // настройка чекбоксов на пользовательской странице
    ui->chBox_sinMovies->setChecked(true);
    ui->chBox_serMovies->setChecked(true);
    ui->chBox_serials->setChecked(true);
    connect(ui->chBox_sinMovies, SIGNAL(clicked()), this, SLOT(sinMovies_visibility_control()));
    connect(ui->chBox_serials, SIGNAL(clicked()), this, SLOT(serials_visibility_control()));
    connect(ui->chBox_serMovies, SIGNAL(clicked()), this, SLOT(serMovies_visibility_control()));

    load_info_base();                     // загрузить информацию из файлов
    load_films_in_list_widg();            // загрузить информацию о фильмах в лист-виджеты

    // настройка шрифтов лист-виджетов
    QFont font("Lucida Console");
    ui->listWidg_marked->setFont(font);
    ui->listWidg_inProc->setFont(font);
    ui->listWidg_inPlans->setFont(font);
    ui->listWidg_watched->setFont(font);
    ui->listWidg_abandoned->setFont(font);
    ui->listWidg_favours->setFont(font);

    QFont font2("Comic Sans MS");
    ui->listWidg_serial->setFont(font2);
    ui->listWidg_sinMovie->setFont(font2);
    ui->listWidg_serMovie->setFont(font2);
}

HFsLib::~HFsLib()
{
    delete ui;
}

//------------------------ слоты на первой странице-----------------------------------------------
// обработка нажатия на кнопку для перехода к пользовательским спискам
void HFsLib::go_to_user_lists() {
    if (info_depot.user_is_authed == false) {                 // если пользователь не автормизован
        AuthorWindow auth_wind(&info_depot);                  // запуск окна авторизации
        auth_wind.setModal(true);
        auth_wind.setStyleSheet("background-color:lightblue");
        auth_wind.exec();

        if (info_depot.user_is_authed == true)                // если пользователь успешно авторизовался
            load_user_lists_in_widgets();
    }
    else {
        ui->stWd_main->setCurrentIndex(2);
        ui->lb_user_name->setText((*(info_depot.current_user)).get_name());
    }
}

// сохранить информацию
void HFsLib::save_progress() {
    sort_by_id_ascendingly();
    info_depot.save_info_in_bin_file();
}

// переход к странице с фильмами
void HFsLib::go_to_film_lib() {
    ui->stWd_main->setCurrentIndex(1);
}

// ------------------визуальная работа со списками текущего пользователя-------------
// выгрузить информацию о списках текущ. пользователя в соответствующие лист-виджеты
void HFsLib::load_user_lists_in_widgets() {
    load_just_marked_users_list_in_widget(ui->listWidg_favours, &((*(info_depot.current_user)).lists.favours));
    load_just_marked_users_list_in_widget(ui->listWidg_inPlans, &((*(info_depot.current_user)).lists.in_plans));
    load_just_marked_users_list_in_widget(ui->listWidg_watched, &((*(info_depot.current_user)).lists.watched));
    load_rating_mark_user_list_in_widget();
    load_stop_point_marked_lists_in_widget(ui->listWidg_inProc, &((*(info_depot.current_user)).lists.in_process));
    load_stop_point_marked_lists_in_widget(ui->listWidg_abandoned, &((*(info_depot.current_user)).lists.abandoned));
}
// выгрузить информацию о простых заметках
void HFsLib::load_just_marked_users_list_in_widget(QListWidget* _listWidg, Ring<FilmMark>* _ring) {
    _listWidg->clear();                           // очистка виджета
    QString headStr = "Name";                     // установка "шапки"
    _listWidg->addItem(headStr);
    (_listWidg->item(0))->setFlags((_listWidg->item(0))->flags() & ~Qt::ItemIsSelectable);

    if (_ring->get_nodesNumber()) {                         // процесс выгрузки информации
        Ring<FilmMark>::Iterator f_iter = _ring->begin();   // итератор для прохода по кольцу с заметками

        int curr_id;                                        // полученный ID
        Film* curr_film;                                    // полученный указатель на текущ. фильм
        do {                                                // проход по кольцу с заметками
            curr_id = (*f_iter).get_film_id();              // получение текущ. ID
            curr_film = info_depot.find_film(curr_id);      // получение указателя на текущ. фильм
            if (curr_film)                                  // занесение информации о нем в лист - виджет
                _listWidg->addItem(new FilmQItem(curr_film->get_name(), curr_film->get_id()));

            f_iter++;
        } while (f_iter != _ring->begin());
    }
}
// выгрузить информацию о заметках c оценками фильма
void HFsLib::load_rating_mark_user_list_in_widget() {
    ui->listWidg_marked->clear();                 // очистка виджета
    QString headStr = "Name";                     // установка "шапки"
    headStr.insert(58, "Mark");
    ui->listWidg_marked->addItem(headStr);
    (ui->listWidg_marked->item(0))->setFlags((ui->listWidg_marked->item(0))->flags() & ~Qt::ItemIsSelectable);

    if ((*(info_depot.current_user)).lists.rated.get_nodesNumber()) {   // процесс выгрузки информации
        Ring<FilmRatingMark>::Iterator f_iter =                         // итератор для прохода по кольцу с заметками
            (*(info_depot.current_user)).lists.rated.begin();

        int curr_id;                                             // полученный ID
        Film* curr_film;                                         // полученный указатель на текущ. фильм
        do {                                                     // проход по кольцу с заметками
            curr_id = (*f_iter).get_film_id();                   // получение текущ. ID
            curr_film = info_depot.find_film(curr_id);           // получение указателя на текущ. фильм
            if (curr_film) {                                     // занесение информации о нем в лист - виджет
                FilmQItem* film_info_to_add = new FilmQItem(curr_film->get_name(), curr_film->get_id());
                film_info_to_add->set_ratingMarkStr((*f_iter).get_ratingM());
                ui->listWidg_marked->addItem(film_info_to_add);
            }

            f_iter++;
        } while (f_iter != (*(info_depot.current_user)).lists.rated.begin());
    }
}

// выгрузить информацию о заметках c точкой остановки просмотра
void HFsLib::load_stop_point_marked_lists_in_widget(QListWidget* _listWidg, Ring<FilmStopPointMark>* _ring) {
    _listWidg->clear();                                        // очистка виджета
    QString headStr = "Name";                                  // установка "шапки"
    headStr.insert(52, "Stop Point");
    _listWidg->addItem(headStr);
    (_listWidg->item(0))->setFlags((_listWidg->item(0))->flags() & ~Qt::ItemIsSelectable);

    if (_ring->get_nodesNumber()) {                                // процесс выгрузки информации
        Ring<FilmStopPointMark>::Iterator f_iter = _ring->begin(); // итератор для прохода по кольцу с заметками

        int curr_id;                                                // полученный ID
        Film* curr_film;                                            // полученный указатель на текущ. фильм
        do {                                                        // проход по кольцу с заметками
            curr_id = (*f_iter).get_film_id();                      // получение ID
            curr_film = info_depot.find_film(curr_id);              // получение указателя на текущ. фильм
            if (curr_film) {                                        // занесение информации о нем в лист - виджет

                FilmQItem* film_info_to_add = new FilmQItem(curr_film->get_name(), curr_film->get_id());
                if (curr_id / 1000 == 1)
                    film_info_to_add->set_serialStopPointStr((*f_iter).get_episodeM());
                else film_info_to_add->set_movieStopPointStr((*f_iter).get_hourM(), (*f_iter).get_minuteM());
                _listWidg->addItem(film_info_to_add);
            }

            f_iter++;
        } while (f_iter != _ring->begin());
    }
}
// -------------------слоты на странице - библиотеке----------------------------
// возврат на главную страницу
void HFsLib::go_back_to_main_page() {
    ui->stWd_main->setCurrentIndex(0);
}

// добавить информацию о фильме
void HFsLib::add_film_info() {
    int film_is = ui->tabWidg_filmBase->currentIndex();         // получение иформации о нужном типе фильма
    FilmInfo addFilm_wind(&info_depot, film_is, -1);            // создание объекта окна для добавления информации о фильме
    addFilm_wind.setModal(true);
    addFilm_wind.setStyleSheet("background-color:lightblue");

    int prev_films_num;                                         // сохранение предыдущего количества фильмов
    if (film_is == 0)
        prev_films_num = info_depot.moviesInfo_r.get_nodesNumber();
    else if (film_is == 1)
        prev_films_num = info_depot.serialsInfo_r.get_nodesNumber();
    else
        prev_films_num = info_depot.seriesMoviesInfo_r.get_nodesNumber();

    addFilm_wind.exec();                                                    // вызов окна для добавления информации о фильме
    if (film_is == 0)                                                       // если число фильмов изменилось,
        if (prev_films_num != info_depot.moviesInfo_r.get_nodesNumber())    // выгрузить новый фильм в соответствующий лист-виджет
            ui->listWidg_sinMovie->addItem(new FilmQItem((*(info_depot.moviesInfo_r.end())).get_name(), (*(info_depot.moviesInfo_r.end())).get_id()));
    if (film_is == 1)
        if (prev_films_num != info_depot.serialsInfo_r.get_nodesNumber())
            ui->listWidg_serial->addItem(new FilmQItem((*(info_depot.serialsInfo_r.end())).get_name(), (*(info_depot.serialsInfo_r.end())).get_id()));
    if (film_is == 2)
            if (prev_films_num != info_depot.seriesMoviesInfo_r.get_nodesNumber())
                ui->listWidg_serMovie->addItem(new FilmQItem((*(info_depot.seriesMoviesInfo_r.end())).get_name(), (*(info_depot.seriesMoviesInfo_r.end())).get_id()));
}

// просмотреть информацию о выбранном фильме
void HFsLib::see_film_info() {
    FilmQItem* curr_film_it = get_chosen_film();         // получить указатель на выбранный объект в виджете
     
    if (curr_film_it == NULL) {                          // проверка на его наличие
        QMessageBox::information(this, "", "You didn't choose the film");
    }
    else {
        int film_is = curr_film_it->get_filmQItem_id();     // получить ID выбранного фильма
                                                         
        FilmConstInfo filmInfo_wind(&info_depot, film_is);  // создание и вызов окна
        filmInfo_wind.setModal(true);                       // для просмотра информации о фильме
        filmInfo_wind.setStyleSheet("background-color:lightblue");

        filmInfo_wind.exec();
    }
}

// удалить информацию о выбранном фильме
void HFsLib::del_film_info() {
    FilmQItem* curr_film_it = get_chosen_film();               // получить указатель на выбранный объект в виджете

    if (curr_film_it == NULL) {                                // проверка на его наличие
        QMessageBox::information(this, "", "You didn't choose the film");
    }
    else {
        try {
            int film_id = curr_film_it->get_filmQItem_id();    // получить ID выбранного фильма
            Film* f_to_del = info_depot.find_film(film_id);    // получить указатель на выбранный фильм

            if (f_to_del) {                                    // если тот найден:
                delete curr_film_it;                           // удалить объект в лист - виджете

                info_depot.del_film_info_from_users_lists(f_to_del);  // удалить информацию о выбранном фильме из пользовательских списков
                if (info_depot.user_is_authed == true)                // если пользователь авторизован
                    load_user_lists_in_widgets();                     // загрузить обновленные пользовательские списки в виджеты

                info_depot.save_previous_film_info(film_id);          // сохранить информацию о выбранном фильме в стек для отмены
                if (film_id / 1000 == 0)                              // в зависимости от текущей страницы удалить фильм из "базы"  
                    info_depot.moviesInfo_r.delete_all_match_elems(*(dynamic_cast<Movie*>(f_to_del)));
                else if (film_id / 1000 == 1)
                    info_depot.serialsInfo_r.delete_all_match_elems(*(dynamic_cast<Serial*>(f_to_del)));
                else if (film_id / 1000 == 2)
                    info_depot.seriesMoviesInfo_r.delete_all_match_elems(*(dynamic_cast<SeriesMovie*>(f_to_del)));

                QMessageBox::information(this, "", "Chosen information is deleted");
                info_depot.acts.actions_st.push(film_id);            // занести в стек действий ID удаленного фильма
                info_depot.acts.actions_st.push(1);                  // занести в стек действий 1 как знак действия отмены
            }
            else throw ContainerException(2, "ERROR.Film isn't found in base");
        }
        catch (ContainerException excp)
        {
            QMessageBox::information(this, "", excp.get_exp_msg());
        }
    }
}

// редактировать информацию о выбранном фильме
void HFsLib::redact_film_info() {
    FilmQItem* curr_film_it = get_chosen_film();          // получить указатель на выбранный объект в виджете  

    if (curr_film_it == NULL) {                           // проверка на его наличие
        QMessageBox::information(this, "", "You didn't choose the film");
    }
    else {
        int film_id = curr_film_it->get_filmQItem_id();   // получить ID выбранного фильма
        info_depot.save_previous_film_info(film_id);      // сохранить информацию о выбранном фильме в стек для отмены

        FilmInfo redactFilm_wind(&info_depot, film_id / 1000, film_id);  // создать и открыть окно для 
        redactFilm_wind.setModal(true);                                  // редактирования информации о фильме
        redactFilm_wind.exec();

        if (info_depot.check_saved_information_on_repeat(film_id) == false) {     // проверить на совпадение сохраненную в стек информацию
            (info_depot.find_film(film_id))->get_name();                          // если та поменялась,
            curr_film_it->setText((info_depot.find_film(film_id))->get_name());   // переписать имя выбранного объекта
            info_depot.acts.actions_st.push(film_id);                             // занести в стек действий ID отредактированного фильма
            info_depot.acts.actions_st.push(0);                                   // занести в стек действий 0 как знак действия редактирования
            
            if (info_depot.user_is_authed == true)
                load_user_lists_in_widgets();
        }
    }
}

// установить параметры поиска
void HFsLib::search_film() {
    FilmInfo redactFilm_wind(&info_depot, ui->tabWidg_filmBase->currentIndex(), -2); // создание
    redactFilm_wind.setModal(true);                 // и открытие окна для задания параметров поиска
    redactFilm_wind.exec();

    set_visible_stats_in_searches();
}

// очистить параметры поиска
void HFsLib::free_search_film() {
    if (ui->tabWidg_filmBase->currentIndex() == 0) {
        Movie emptyMovie;
        info_depot.acts.movie_search_lb = info_depot.acts.movie_search_hb = emptyMovie;
    }
    if (ui->tabWidg_filmBase->currentIndex() == 1) {
        Serial emptySerial;
        info_depot.acts.serial_search_lb = info_depot.acts.serial_search_hb = emptySerial;
    }
    if (ui->tabWidg_filmBase->currentIndex() == 2) {
        SeriesMovie emptySerMovie;
        info_depot.acts.serMovie_search_lb = info_depot.acts.serMovie_search_hb = emptySerMovie;
    }
    set_visible_stats_in_searches();
}

// вывести информацию случайного фильма
void HFsLib::get_random_film() {
    int f_id;                                        // выбор случайного ID в зависимости от страницы
    sort_by_id_ascendingly();                        // отсортировать по ID  
    if (ui->tabWidg_filmBase->currentIndex() == 0)
        f_id = HFsLibTemplate::get_random_film_id<Movie>(&info_depot.moviesInfo_r, &info_depot);
    else if (ui->tabWidg_filmBase->currentIndex() == 1)
        f_id = HFsLibTemplate::get_random_film_id<Serial>(&info_depot.serialsInfo_r, &info_depot);
    else
        f_id = HFsLibTemplate::get_random_film_id<SeriesMovie>(&info_depot.seriesMoviesInfo_r, &info_depot);
    if (f_id == -1)
        return;
    FilmConstInfo filmInfo_wind(&info_depot, f_id);  // создание и открытие окна с информацией о фильме
    filmInfo_wind.setModal(true);
    filmInfo_wind.setStyleSheet("background-color:lightblue");

    filmInfo_wind.exec();
}

// вывести меню выбора сортировки
void HFsLib::show_sort_menu() {
    QMenu* m_enu = new QMenu(this);                                        // создание меню
    m_enu->setStyleSheet("color: white;" "background-color: skyblue;"
        "selection-color: violet;" "selection-background-color: white;");
                                                                           // создание его действий
    QAction* sort_by_id_ascendingly_act = new QAction(tr("sort by ID (from first to last)"), this);
    QAction* sort_by_id_descendingly_act = new QAction(tr("sort by ID (from last to first)"), this);
    QAction* sort_by_name_act = new QAction(tr("sort by name"), this);
    QAction* sort_by_country_act = new QAction(tr("sort by country"), this);
    QAction* sort_by_year_ascendingly_act = new QAction(tr("sort by year (from oldest to newest)"), this);
    QAction* sort_by_year_descendingly_act = new QAction(tr("sort by year (from newest to oldest)"), this);
    QAction* sort_by_genre_act = new QAction(tr("sort by genre"), this);
    QAction* sort_by_rating_act = new QAction(tr("sort by rating"), this);
                                                                          // связь действий с методами
    connect(sort_by_id_ascendingly_act, SIGNAL(triggered()), this, SLOT(sort_by_id_ascendingly()));
    connect(sort_by_id_descendingly_act, SIGNAL(triggered()), this, SLOT(sort_by_id_descendingly()));
    connect(sort_by_name_act, SIGNAL(triggered()), this, SLOT(sort_by_name()));
    connect(sort_by_country_act, SIGNAL(triggered()), this, SLOT(sort_by_country()));
    connect(sort_by_year_ascendingly_act, SIGNAL(triggered()), this, SLOT(sort_by_year_ascendingly()));
    connect(sort_by_year_descendingly_act, SIGNAL(triggered()), this, SLOT(sort_by_year_descendingly()));
    connect(sort_by_genre_act, SIGNAL(triggered()), this, SLOT(sort_by_genre()));
    connect(sort_by_rating_act, SIGNAL(triggered()), this, SLOT(sort_by_rating()));
                                                                          // добавление действий в меню
    m_enu->addAction(sort_by_id_ascendingly_act);
    m_enu->addAction(sort_by_id_descendingly_act);
    m_enu->addAction(sort_by_name_act);
    m_enu->addAction(sort_by_country_act);
    m_enu->addAction(sort_by_year_ascendingly_act);
    m_enu->addAction(sort_by_year_descendingly_act);
    m_enu->addAction(sort_by_genre_act);
    m_enu->addAction(sort_by_rating_act);

    m_enu->popup(QCursor::pos());                                        // вывод меню
}

// отмена действия (редактирования или удаления фильма из базы)
void HFsLib::cancel_action() {
    if (info_depot.acts.actions_st.empty() == true)
        QMessageBox::information(this, "", "There are not actions to cancel");
    else {
        int action = info_depot.acts.actions_st.top();
        info_depot.acts.actions_st.pop();
        if (action == 0)
            return_previous_redact_info();
        if (action == 1) {
            recover_deleted_info();
            QMessageBox::information(this, "", "Deleted Information was restored\nSearch Parametres were reset");
        }
    }
}

// ------------------ПКМ меню----------------------------------------------------------------
// выбор контекстного меню для создания
void HFsLib::showContextMenu() {                   
    if (ui->stWd_main->currentIndex() != 0) {     
        if (ui->stWd_main->currentIndex() == 1)
            show_context_menu_from_film_base();

        else if (ui->stWd_main->currentIndex() == 2)
            show_comtext_menu_from_users_lists();
    }
}

// создание контекстного меню для страницы - библиотеки
void HFsLib::show_context_menu_from_film_base() {
    QMenu* m_enu = new QMenu(this);                      // создание меню
    m_enu->setStyleSheet("color: white;" "background-color: skyblue;"
        "selection-color: violet;" "selection-background-color: white;");

    if (info_depot.user_is_authed == true) {            // если пользователь авторизован, создание действий для пользовательских списков
        QAction* add_to_inProcess_act = new QAction(tr("add to \"in Process\""), this);
        QAction* add_to_inPlans_act = new QAction(tr("add to \"in Plans\""), this);
        QAction* add_to_watched_act = new QAction(tr("add to \"Watched\""), this);
        QAction* add_to_abandoned_act = new QAction(tr("add to \"Abandoned\""), this);
        QAction* add_to_favourites_act = new QAction(tr("add to \"Favourites\""), this);
        QAction* add_to_marked_act = new QAction(tr("add Mark"), this);
                                                        // связь действий с методами
        connect(add_to_inPlans_act, SIGNAL(triggered()), this, SLOT(add_to_inPlans()));
        connect(add_to_watched_act, SIGNAL(triggered()), this, SLOT(add_to_watched()));
        connect(add_to_favourites_act, SIGNAL(triggered()), this, SLOT(add_to_favourites()));
        connect(add_to_marked_act, SIGNAL(triggered()), this, SLOT(add_to_marked()));
        connect(add_to_abandoned_act, SIGNAL(triggered()), this, SLOT(add_to_abandoned()));
        connect(add_to_inProcess_act, SIGNAL(triggered()), this, SLOT(add_to_inProcess()));
                                                        // связь действий с меню
        m_enu->addAction(add_to_inProcess_act);
        m_enu->addAction(add_to_inPlans_act);
        m_enu->addAction(add_to_watched_act);
        m_enu->addAction(add_to_abandoned_act);
        m_enu->addAction(add_to_favourites_act);
        m_enu->addAction(add_to_marked_act);
    }
    else {                                              // вывод предложения авторизоваться, если пользователь не авторизован
        QAction* log_in_act = new QAction(tr("You need to log in for opportunity add films to your lists"), this);
        connect(log_in_act, SIGNAL(triggered()), this, SLOT(go_to_user_lists()));
        m_enu->addAction(log_in_act);
    }

    m_enu->popup(QCursor::pos());                      // вывод меню
}

// создание контекстного иеню для страницы пользователя
void HFsLib::show_comtext_menu_from_users_lists() {
    QMenu* m_enu = new QMenu(this);                    // создать меню
    m_enu->setStyleSheet("color: white;"
        "background-color: skyblue;"
        "selection-color: violet;"
        "selection-background-color: white;");
                                                       // создать действия для него
    QAction* del_from_user_list_act = new QAction(tr("delete from current list"), this);
    QAction* show_in_film_base_act = new QAction(tr("show in films base"), this);
    QAction* add_to_another_list_act = new QAction(tr("add to another list"), this);
                                                       // связать их с методами
    connect(del_from_user_list_act, SIGNAL(triggered()), this, SLOT(delete_from_user_list()));
    connect(show_in_film_base_act, SIGNAL(triggered()), this, SLOT(show_in_film_base()));
    connect(add_to_another_list_act, SIGNAL(triggered()), this, SLOT(show_context_menu_from_film_base()));
                                                       // добавить действия в меню
    m_enu->addAction(del_from_user_list_act);
    m_enu->addAction(show_in_film_base_act);
    m_enu->addAction(add_to_another_list_act);
                                                       // добавить действия редаткирования заметки, где возможно
    int curr_user_lists_page = ui->tabWidg_userLists->currentIndex();
    if (curr_user_lists_page == 0 || curr_user_lists_page == 3 || curr_user_lists_page == 5) {
        QAction* redact_mark_act = new QAction(tr("redact mark"), this);
        connect(redact_mark_act, SIGNAL(triggered()), this, SLOT(redact_mark()));
        m_enu->addAction(redact_mark_act);
    }
    m_enu->popup(QCursor::pos());                      // вывод меню
}

//-----------------------слоты на странице пользователя---------------------------------
// покинуть аккаунт
void HFsLib::leave_account() {
    info_depot.user_is_authed = false;
    go_back_to_main_page();

    ui->listWidg_inProc->clear();
    ui->listWidg_inPlans->clear();
    ui->listWidg_abandoned->clear();
    ui->listWidg_favours->clear();
    ui->listWidg_marked->clear();
    ui->listWidg_watched->clear();
}

// контроль отображения одиночных полнометражных фильмов
void HFsLib::sinMovies_visibility_control() {
    bool hidden_cond;                  // если чек-бокс нажат - фильм отображается 
    if (ui->chBox_sinMovies->isChecked() == true)
        hidden_cond = false;
    else hidden_cond = true;

    set_visibility_in_userListWidgets(hidden_cond, 0);
}
// контроль отображения сериалов
void HFsLib::serials_visibility_control() {
    bool hidden_cond;                   // если чек-бокс нажат - фильм отображается 
    if (ui->chBox_serials->isChecked() == true)
        hidden_cond = false;
    else hidden_cond = true;

    set_visibility_in_userListWidgets(hidden_cond, 1);
}
// контроль отображения серийных полнометражных фильмов 
void HFsLib::serMovies_visibility_control() {
    bool hidden_cond;                    // если чек-бокс нажат - фильм отображается 
    if (ui->chBox_serMovies->isChecked() == true)
        hidden_cond = false;
    else hidden_cond = true;

    set_visibility_in_userListWidgets(hidden_cond, 2);
}

// установить видимость выбранного типа фильма
void HFsLib::set_visibility_in_userListWidgets(bool hidden_cond, int film) {
    set_visibility_in_chosenUserListWidget(hidden_cond, film, ui->listWidg_inPlans);
    set_visibility_in_chosenUserListWidget(hidden_cond, film, ui->listWidg_inProc);
    set_visibility_in_chosenUserListWidget(hidden_cond, film, ui->listWidg_abandoned);
    set_visibility_in_chosenUserListWidget(hidden_cond, film, ui->listWidg_favours);
    set_visibility_in_chosenUserListWidget(hidden_cond, film, ui->listWidg_marked);
    set_visibility_in_chosenUserListWidget(hidden_cond, film, ui->listWidg_watched);
}
// установить видимость выбранного типа фильма в определенном листе-виджете
void HFsLib::set_visibility_in_chosenUserListWidget(bool hidden_cond, int film, QListWidget* _listWidg) {
    FilmQItem* curr_film_item = NULL;
    for (int i = 1; i < _listWidg->count(); i++) {   // проход по списку и установка параметров видимости
        curr_film_item = dynamic_cast<FilmQItem*>(_listWidg->item(i));
        if (curr_film_item->get_filmQItem_id() / 1000 == film)
            _listWidg->setRowHidden(i, hidden_cond);
    }
}

// загрузить информацию из файлов
void HFsLib::load_info_base() {
    try {
        info_depot.get_info_from_bin_file();
    }
    catch (FileException excp) {
        QMessageBox::information(this, "", excp.get_exp_msg());
        if (excp.get_exp_code() == 1)
            this->close();
    }
}

// добавить загруженные из файла фильмы 
void HFsLib::load_films_in_list_widg() {
    HFsLibTemplate::add_filmInfo_in_listWidg<Movie>(ui->listWidg_sinMovie, &(info_depot.moviesInfo_r));
    HFsLibTemplate::add_filmInfo_in_listWidg<Serial>(ui->listWidg_serial, &(info_depot.serialsInfo_r));
    HFsLibTemplate::add_filmInfo_in_listWidg<SeriesMovie>(ui->listWidg_serMovie, &(info_depot.seriesMoviesInfo_r));
}

// получить указатель на выбранный пользователем фильм
FilmQItem* HFsLib::get_chosen_film() {
    FilmQItem* curr_film_it = NULL;
                                                 // выбор лист виджета в зависимости от текущей страницы
    if (ui->stWd_main->currentIndex() == 1) {
        int film_is = ui->tabWidg_filmBase->currentIndex();
        if (film_is == 0)
            curr_film_it = dynamic_cast<FilmQItem*>(ui->listWidg_sinMovie->currentItem());
        if (film_is == 1)
            curr_film_it = dynamic_cast<FilmQItem*>(ui->listWidg_serial->currentItem());
        if (film_is == 2)
            curr_film_it = dynamic_cast<FilmQItem*>(ui->listWidg_serMovie->currentItem());
    }
    if (ui->stWd_main->currentIndex() == 2) {
        int list_is = ui->tabWidg_userLists->currentIndex();

        if (list_is == 0)
            curr_film_it = dynamic_cast<FilmQItem*>(ui->listWidg_inProc->currentItem());
        if (list_is == 1)
            curr_film_it = dynamic_cast<FilmQItem*>(ui->listWidg_inPlans->currentItem());
        if (list_is == 2)
            curr_film_it = dynamic_cast<FilmQItem*>(ui->listWidg_watched->currentItem());
        if (list_is == 3)
            curr_film_it = dynamic_cast<FilmQItem*>(ui->listWidg_abandoned->currentItem());
        if (list_is == 4)
            curr_film_it = dynamic_cast<FilmQItem*>(ui->listWidg_favours->currentItem());
        if (list_is == 5)
            curr_film_it = dynamic_cast<FilmQItem*>(ui->listWidg_marked->currentItem());
    }
    return curr_film_it;
}

// установка параметров видимости после выполнения поиска
void HFsLib::set_visible_stats_in_searches() {
    if (ui->tabWidg_filmBase->currentIndex() == 0)
        HFsLibTemplate::show_searched_filmInfo_in_listWidg<Movie>(ui->listWidg_sinMovie, &(info_depot.moviesInfo_r),
            info_depot.acts.movie_search_lb, info_depot.acts.movie_search_hb);
    if (ui->tabWidg_filmBase->currentIndex() == 1)
        HFsLibTemplate::show_searched_filmInfo_in_listWidg<Serial>(ui->listWidg_serial, &(info_depot.serialsInfo_r),
            info_depot.acts.serial_search_lb, info_depot.acts.serial_search_hb);
    if (ui->tabWidg_filmBase->currentIndex() == 2)
        HFsLibTemplate::show_searched_filmInfo_in_listWidg<SeriesMovie>(ui->listWidg_serMovie, &(info_depot.seriesMoviesInfo_r),
            info_depot.acts.serMovie_search_lb, info_depot.acts.serMovie_search_hb);
}

// сортировки списков по определенному параметру:
void  HFsLib::sort_by_id_ascendingly() {        // - по ID по возрастанию
    HFsLibTemplate::sort_by_id_template<Movie>(ui->listWidg_sinMovie, &(info_depot.moviesInfo_r), 1);
    HFsLibTemplate::sort_by_id_template<Serial>(ui->listWidg_serial, &(info_depot.serialsInfo_r), 1);
    HFsLibTemplate::sort_by_id_template<SeriesMovie>(ui->listWidg_serMovie, &(info_depot.seriesMoviesInfo_r), 1);
}
void  HFsLib::sort_by_id_descendingly() {       // - по ID по убыванию
    HFsLibTemplate::sort_by_id_template<Movie>(ui->listWidg_sinMovie, &(info_depot.moviesInfo_r), 0);
    HFsLibTemplate::sort_by_id_template<Serial>(ui->listWidg_serial, &(info_depot.serialsInfo_r), 0);
    HFsLibTemplate::sort_by_id_template<SeriesMovie>(ui->listWidg_serMovie, &(info_depot.seriesMoviesInfo_r), 0);
}
void  HFsLib::sort_by_name() {                  // - по имени
    HFsLibTemplate::sort_by_name_template<Movie>(ui->listWidg_sinMovie, &(info_depot.moviesInfo_r));
    HFsLibTemplate::sort_by_name_template<Serial>(ui->listWidg_serial, &(info_depot.serialsInfo_r));
    HFsLibTemplate::sort_by_name_template<SeriesMovie>(ui->listWidg_serMovie, &(info_depot.seriesMoviesInfo_r));
}
void HFsLib::sort_by_country() {                // - по стране
    HFsLibTemplate::sort_by_country_template<Movie>(ui->listWidg_sinMovie, &(info_depot.moviesInfo_r));
    HFsLibTemplate::sort_by_country_template<Serial>(ui->listWidg_serial, &(info_depot.serialsInfo_r));
    HFsLibTemplate::sort_by_country_template<SeriesMovie>(ui->listWidg_serMovie, &(info_depot.seriesMoviesInfo_r));
}
void HFsLib::sort_by_year_ascendingly() {       // - по году по возрастанию
    HFsLibTemplate::sort_by_year_template<Movie>(ui->listWidg_sinMovie, &(info_depot.moviesInfo_r), 1);
    HFsLibTemplate::sort_by_year_template<Serial>(ui->listWidg_serial, &(info_depot.serialsInfo_r), 1);
    HFsLibTemplate::sort_by_year_template<SeriesMovie>(ui->listWidg_serMovie, &(info_depot.seriesMoviesInfo_r), 1);
}
void HFsLib::sort_by_year_descendingly() {      // - по году по убыванию
    HFsLibTemplate::sort_by_year_template<Movie>(ui->listWidg_sinMovie, &(info_depot.moviesInfo_r), 0);
    HFsLibTemplate::sort_by_year_template<Serial>(ui->listWidg_serial, &(info_depot.serialsInfo_r), 0);
    HFsLibTemplate::sort_by_year_template<SeriesMovie>(ui->listWidg_serMovie, &(info_depot.seriesMoviesInfo_r), 0);
}
void HFsLib::sort_by_genre() {                  // - по жанру
    HFsLibTemplate::sort_by_genre_template<Movie>(ui->listWidg_sinMovie, &(info_depot.moviesInfo_r));
    HFsLibTemplate::sort_by_genre_template<Serial>(ui->listWidg_serial, &(info_depot.serialsInfo_r));
    HFsLibTemplate::sort_by_genre_template<SeriesMovie>(ui->listWidg_serMovie, &(info_depot.seriesMoviesInfo_r));
}
void HFsLib::sort_by_rating() {                 // - по рейтингу
    HFsLibTemplate::sort_by_rating_template<Movie>(ui->listWidg_sinMovie, &(info_depot.moviesInfo_r));
    HFsLibTemplate::sort_by_rating_template<Serial>(ui->listWidg_serial, &(info_depot.serialsInfo_r));
    HFsLibTemplate::sort_by_rating_template<SeriesMovie>(ui->listWidg_serMovie, &(info_depot.seriesMoviesInfo_r));
}

// методы добавления фильмов в пользовательские списки:
void HFsLib::add_to_inPlans() {                   // - "в планах"
    add_filmMark_info_in_user_list(ui->listWidg_inPlans, &((*(info_depot.current_user)).lists.in_plans));
}
void HFsLib::add_to_watched() {                   // - "просмотрено"
    add_filmMark_info_in_user_list(ui->listWidg_watched, &((*(info_depot.current_user)).lists.watched));
}
void HFsLib::add_to_favourites() {                // - "любимое"
    add_filmMark_info_in_user_list(ui->listWidg_favours, &((*(info_depot.current_user)).lists.favours));
}
void HFsLib::add_to_inProcess() {                 // - "в процессе"
    add_filmMark_stopPointInfo_in_user_list(ui->listWidg_inProc, &((*(info_depot.current_user)).lists.in_process));
}
void HFsLib::add_to_abandoned() {                 // - "брошено"
    add_filmMark_stopPointInfo_in_user_list(ui->listWidg_abandoned, &((*(info_depot.current_user)).lists.abandoned));
}

void HFsLib::add_to_marked() {                    // - "дана оценка"
    FilmQItem* curr_film_it = get_chosen_film();  // получить указатель на выбранный объект в виджете

    if (curr_film_it == NULL)
        QMessageBox::information(this, "", "You didn't choose the film");
    else {
        try {
            int curr_film_id = curr_film_it->get_filmQItem_id();            // получитьь ID выбранного фильма
            Film* curr_film_p = info_depot.find_film(curr_film_id);         // получить указатель на выбранный фильм
            if (curr_film_p == NULL)
                throw ContainerException(2, "ERROR. Film isn't found in base");
            if ((*(info_depot.current_user)).lists.rated.get_nodesNumber()) // проверить фильм на наличие в даннои списке
                if ((Algorithm::find_match<Ring<FilmRatingMark>::Iterator, FilmRatingMark>((*(info_depot.current_user)).lists.rated.begin(),
                    (*(info_depot.current_user)).lists.rated.end(), FilmRatingMark(curr_film_p))).check()) {
                    QMessageBox::information(this, "", "This film is already present in chosen list");
                    return;
                }

            int mark = -1;                                                  // оценка по умочанию
            FilmMarkNInfoWind mark_info_wind(0, &mark);                     // создание и открытие окна для 
            mark_info_wind.setModal(true);                                  // датия оценки
            mark_info_wind.setStyleSheet("background-color:skyblue");
            mark_info_wind.exec();

            if (mark == -1)                                                 // отмена, если оценка
                return;                                                     // не была дана

            FilmRatingMark rat_mark(curr_film_p, mark);                     // добавить в пользовательский спискок
            (*(info_depot.current_user)).lists.rated.add(rat_mark);

            curr_film_p->add_rating_mark(mark);                             // добавить оценку к фильму

            // добавить вывод в пользовательских списках
            FilmQItem* film_info_to_add = new FilmQItem(curr_film_p->get_name(), curr_film_p->get_id());
            film_info_to_add->set_ratingMarkStr(mark);
            ui->listWidg_marked->addItem(film_info_to_add);
        }
        catch (ContainerException excp) {
            QMessageBox::information(this, "", excp.get_exp_msg());
        }
    }
}

// добавить обычную заметку о фильме в пользовательский список
void HFsLib::add_filmMark_info_in_user_list(QListWidget* _listWidg, Ring<FilmMark>* _ring) {
    FilmQItem* curr_film_item = get_chosen_film();                        // получить указатель на выбранный объект в виджете
    if (curr_film_item == NULL)
        QMessageBox::information(this, "", "You didn't choose the film");
    else {
        try {
            int curr_id = curr_film_item->get_filmQItem_id();             // получить ID выбранного фильма
            Film* f_pointer = info_depot.find_film(curr_id);              // получить указатель на выбранный фильм
            if (f_pointer == NULL)
                throw ContainerException(2, "ERROR. Film isn't found in base");
            FilmMark f_mark_to_add(f_pointer);                            // заметка чтобы добавить

            if (_ring->get_nodesNumber())                                 // проверить фильм на наличие в даннои списке
                if ((Algorithm::find_match<Ring<FilmMark>::Iterator, FilmMark>(_ring->begin(), _ring->end(), f_mark_to_add)).check()) {
                    QMessageBox::information(this, "", "This film is already present in chosen list");
                    return;
                }
            (*_ring).add(f_mark_to_add);                                  // добавить заметку и вывод в пользовательских списках
            _listWidg->addItem(new FilmQItem(f_pointer->get_name(), f_pointer->get_id()));
        }
        catch (ContainerException excp) {
            QMessageBox::information(this, "", excp.get_exp_msg());
        }
    }
}

// добавить заметку об остановке в просмотре в пользовательский список
void HFsLib::add_filmMark_stopPointInfo_in_user_list(QListWidget* _listWidg, Ring<FilmStopPointMark>* _ring) {
    FilmQItem* curr_film_item = get_chosen_film();                   // получить указатель на выбранный объект в виджете
    if (curr_film_item == NULL)
        QMessageBox::information(this, "", "You didn't choose the film");

    else {
        try {
            int curr_film_id = curr_film_item->get_filmQItem_id();  // получить ID выбранного фильма
            Film* curr_film_p = info_depot.find_film(curr_film_id); // получить указатель на выбранный фильм
            if (curr_film_p == NULL)
                throw ContainerException(2, "ERROR. Film isn't found in base");

            if (_ring->get_nodesNumber())                           // проверить фильм на наличие в даннои списке
                if ((Algorithm::find_match<Ring<FilmStopPointMark>::Iterator, FilmStopPointMark>(_ring->begin(), _ring->end(), FilmStopPointMark(curr_film_p))).check()) {
                    QMessageBox::information(this, "", "This film is already present in chosen list");
                    return;
                }

            int mark1 = -1;               // отметки по умолчанию
            int mark2 = -1;

            FilmMarkNInfoWind mark_info_wind(2, &mark1);    // создание и открытие окна для 
            mark_info_wind.setModal(true);                  // задания отметок
            mark_info_wind.setStyleSheet("background-color:skyblue");
            if (curr_film_id / 1000 != 1) {
                mark_info_wind.set_second_mark(&mark2);
                mark_info_wind.set_movieStopPointMode();
            }

            mark_info_wind.exec();
            if (mark1 == -1)                               // отмена, если отметки не были заданы
                return;
                                                           // создать заметку с точкой остановки в просмотре
            FilmStopPointMark stopPoint(curr_film_p, mark1, mark2);
            (*_ring).add(stopPoint);

            // добавить вывод в пользовательских списках
            FilmQItem* film_info_to_add = new FilmQItem(curr_film_p->get_name(), curr_film_p->get_id());
            if (curr_film_id / 1000 == 1)
                film_info_to_add->set_serialStopPointStr(stopPoint.get_episodeM());
            else
                film_info_to_add->set_movieStopPointStr(stopPoint.get_hourM(), stopPoint.get_minuteM());
            _listWidg->addItem(film_info_to_add);
        }
        catch (ContainerException excp) {
            QMessageBox::information(this, "", excp.get_exp_msg());
        }
    }
}
//
//// перенести к выбранному фильму на странице - "базе"
//void HFsLib::show_in_film_base() {
//    FilmQItem* curr_film_item = get_chosen_film();                     // получить указатель на выбранный объект в виджете
//    if (curr_film_item == NULL)
//        QMessageBox::information(this, "", "You didn't choose the film");
//    else {
//        int film_id = get_chosen_film()->get_filmQItem_id();           // получить ID выбранного фильма
//
//        ui->stWd_main->setCurrentIndex(1);                             // задать страницу - библиотеку
//        ui->tabWidg_filmBase->setCurrentIndex(film_id / 1000);         // выбрать нужный таб исходя из ID
//
//        FilmQItem* filp_pointer_in_film_base = NULL;                   // найти выбранный фильм в базе
//        if (film_id / 1000 == 0) {                                     // и пролистать к нему
//            filp_pointer_in_film_base = FilmQItem::get_FilmQItem_in_listWidget(ui->listWidg_sinMovie, film_id);
//            if (filp_pointer_in_film_base) {
//                ui->listWidg_sinMovie->scrollToItem(filp_pointer_in_film_base);
//                ui->listWidg_sinMovie->setCurrentItem(filp_pointer_in_film_base);
//            }
//        }
//        if (film_id / 1000 == 1) {
//            filp_pointer_in_film_base = FilmQItem::get_FilmQItem_in_listWidget(ui->listWidg_serial, film_id);
//            if (filp_pointer_in_film_base) {
//                ui->listWidg_serial->scrollToItem(filp_pointer_in_film_base);
//                ui->listWidg_serial->setCurrentItem(filp_pointer_in_film_base);
//            }
//        }
//        if (film_id / 1000 == 2) {
//            filp_pointer_in_film_base = FilmQItem::get_FilmQItem_in_listWidget(ui->listWidg_serMovie, film_id);
//            if (filp_pointer_in_film_base) {
//                ui->listWidg_serMovie->scrollToItem(filp_pointer_in_film_base);
//                ui->listWidg_serMovie->setCurrentItem(filp_pointer_in_film_base);
//            }
//        }
//    }
//}

// перенести к выбранному фильму на странице - "базе"
void HFsLib::show_in_film_base() {
    FilmQItem* curr_film_item = get_chosen_film();                     // получить указатель на выбранный объект в виджете
    if (curr_film_item == NULL)
        QMessageBox::information(this, "", "You didn't choose the film");
    else {
        int film_id = get_chosen_film()->get_filmQItem_id();           // получить ID выбранного фильма

        ui->stWd_main->setCurrentIndex(1);                             // задать страницу - библиотеку
        ui->tabWidg_filmBase->setCurrentIndex(film_id / 1000);         // выбрать нужный таб исходя из ID

        //FilmQItem* curr_film_item = NULL;                   // найти выбранный фильм в базе
        if (film_id / 1000 == 0) {                                     // и пролистать к нему
            curr_film_item = FilmQItem::get_FilmQItem_in_listWidget(ui->listWidg_sinMovie, film_id);
            if (curr_film_item) {
                ui->listWidg_sinMovie->scrollToItem(curr_film_item);
                ui->listWidg_sinMovie->setCurrentItem(curr_film_item);
            }
        }
        if (film_id / 1000 == 1) {
            curr_film_item = FilmQItem::get_FilmQItem_in_listWidget(ui->listWidg_serial, film_id);
            if (curr_film_item) {
                ui->listWidg_serial->scrollToItem(curr_film_item);
                ui->listWidg_serial->setCurrentItem(curr_film_item);
            }
        }
        if (film_id / 1000 == 2) {
            curr_film_item = FilmQItem::get_FilmQItem_in_listWidget(ui->listWidg_serMovie, film_id);
            if (curr_film_item) {
                ui->listWidg_serMovie->scrollToItem(curr_film_item);
                ui->listWidg_serMovie->setCurrentItem(curr_film_item);
            }
        }
    }
}

// удалить заметку из пользовательского списка
void HFsLib::delete_from_user_list() {
    FilmQItem* curr_film_it = get_chosen_film();                     // получить указатель на выбранный объект в виджете
    if (curr_film_it == NULL)
        QMessageBox::information(this, "", "You didn't choose the film");
    else {
        int film_id = curr_film_it->get_filmQItem_id();              // получить ID выбранного фильма
        int user_list_ind = ui->tabWidg_userLists->currentIndex();   // получить номер текущего списка и найти заметку
        FilmMark* mark_to_del = info_depot.find_film_mark_in_user_list(film_id, user_list_ind);

        if (user_list_ind == 0)                                      // удалить ее в соответствующем списке
            (*(info_depot.current_user)).lists.in_process.delete_all_match_elems(*(static_cast<FilmStopPointMark*>(mark_to_del)));
        if (user_list_ind == 1)
            (*(info_depot.current_user)).lists.in_plans.delete_all_match_elems(*(mark_to_del));
        if (user_list_ind == 2)
            (*(info_depot.current_user)).lists.watched.delete_all_match_elems(*(mark_to_del));
        if (user_list_ind == 3)
            (*(info_depot.current_user)).lists.abandoned.delete_all_match_elems(*(static_cast<FilmStopPointMark*>(mark_to_del)));
        if (user_list_ind == 4)
            (*(info_depot.current_user)).lists.favours.delete_all_match_elems(*(mark_to_del));
        if (user_list_ind == 5) {                                   // удалить оценку пользователя
            delete_rating_mark(static_cast<FilmRatingMark*>(mark_to_del));
            (*(info_depot.current_user)).lists.rated.delete_all_match_elems(*(static_cast<FilmRatingMark*>(mark_to_del)));
        }

        delete curr_film_it;                                        // удалить объект в списке
        QMessageBox::information(this, "", "Chosen information is deleted");
    }
}

// удалить рейтинговую оценку о фильме
void HFsLib::delete_rating_mark(FilmRatingMark* _rm) {
    Ring<UserInfo>::Iterator us_it = info_depot.current_user;       // итератор с текущим пользователем
    us_it++;
    FilmRatingMark compare_mark;                                    // заметка для поиска совпадающей
    compare_mark.set_id(_rm->get_film_id());
    FilmRatingMark* rm_p = NULL;

    while (us_it != info_depot.current_user) {                      // поиск оценки выбранному фильму другого пользователя
        if ((*us_it).lists.rated.get_nodesNumber())
            rm_p = &(*(Algorithm::find_match<Ring<FilmRatingMark>::Iterator, FilmRatingMark>((*us_it).lists.rated.begin(), (*us_it).lists.rated.end(), compare_mark)));

        if (rm_p != NULL)
            break;
        us_it++;
    }
    if (rm_p == NULL)                                                    // если текущий пользователь - единственный, кто дал оценку
        (info_depot.find_film(_rm->get_film_id()))->reset_rating_mark(); // обнулить ее
    else                                                                 // нет - вычесть удаляемую оценку из рейтинга
        (info_depot.find_film(_rm->get_film_id()))->delete_rating_mark(_rm->get_ratingM());
}

// редактировать пользовательскую заметку
void HFsLib::redact_mark() {
    FilmQItem* curr_film_it = get_chosen_film();                         // получить указатель на выбранный объект в виджете
    if (curr_film_it == NULL)
        QMessageBox::information(this, "", "You didn't choose the film");
    else {                                                               // выбор метода - редактирования в зависимости от текущей станицы
        int curr_user_list_page = ui->tabWidg_userLists->currentIndex();
        FilmMark* curr_mark_p = info_depot.find_film_mark_in_user_list(curr_film_it->get_filmQItem_id(), curr_user_list_page);
        if (curr_user_list_page == 0 || curr_user_list_page == 3) {
            redact_stopPoint_mark(static_cast<FilmStopPointMark*>(curr_mark_p));
            if (curr_film_it->get_filmQItem_id() / 1000 == 1)              // обновление строки в виджете
                curr_film_it->set_serialStopPointStr((static_cast<FilmStopPointMark*>(curr_mark_p))->get_episodeM());
            else
                curr_film_it->set_movieStopPointStr((static_cast<FilmStopPointMark*>(curr_mark_p))->get_hourM(), (static_cast<FilmStopPointMark*>(curr_mark_p))->get_minuteM());
        }
        if (curr_user_list_page == 5) {
            redact_rating_mark(static_cast<FilmRatingMark*>(curr_mark_p)); // обновление строки в виджете
            curr_film_it->set_ratingMarkStr((static_cast<FilmRatingMark*>(curr_mark_p))->get_ratingM());
        }
    }
}

// редактировать заметку - оценку 
void HFsLib::redact_rating_mark(FilmRatingMark* _rm) {
    int curr_rm;                                    // сохранение текущей заметки
    int old_rm = curr_rm = _rm->get_ratingM();

    FilmMarkNInfoWind mark_info_wind(0, &curr_rm);  // создание и вывод окна для редактирования заметки
    mark_info_wind.setModal(true);
    mark_info_wind.setStyleSheet("background-color:skyblue");
    mark_info_wind.exec();

    if (curr_rm != old_rm) {                        // если заметка - оценка изменилась
        delete_rating_mark(_rm);                    // удалить старую оценку
        info_depot.find_film(_rm->get_film_id())->add_rating_mark(curr_rm);
        _rm->set_ratingM(curr_rm);                  // и добавить новую
    }
}

// редактировать заметку - точку остановки в просмотре
void HFsLib::redact_stopPoint_mark(FilmStopPointMark* _spM) {
    int curr_spMark1 = _spM->get_hourM();                          // сохранение текущей заметки
    int curr_spMark2 = _spM->get_minuteM();

    FilmMarkNInfoWind mark_info_wind(2, &curr_spMark1);            // создание  и открытие окна для ее редактирования
    mark_info_wind.setModal(true);
    mark_info_wind.setStyleSheet("background-color:skyblue");
    if (curr_spMark2 != -1) {                                      // если это заметка не о сериале
        mark_info_wind.set_second_mark(&curr_spMark2);             // добавить доп. информацию 
        mark_info_wind.set_movieStopPointMode();
    }
    mark_info_wind.exec();

    _spM->set_hourM(curr_spMark1);                                 // установить новые параметры заметки
    _spM->set_minuteM(curr_spMark2);
}

// отменить редактирование фильма
void HFsLib::return_previous_redact_info() {
    int film_id = info_depot.acts.actions_st.top();       // считать ID измененного фильма
    info_depot.acts.actions_st.pop();                     // удалить ID из стека с действиями 
    Film* film_pointer = info_depot.find_film(film_id);   // получить указатель на измененный фильм
    FilmQItem* list_wig_pointer;                          // указатель на объект в виджете

    if (film_id / 1000 == 0) {              // отмена редактирования в зависимости от ID
        (*(static_cast<Movie*>(film_pointer))) = info_depot.acts.sinMovies_st.top();
        info_depot.acts.sinMovies_st.pop();
        list_wig_pointer = FilmQItem::get_FilmQItem_in_listWidget(ui->listWidg_sinMovie, film_id);
    }
    else if (film_id / 1000 == 1) {
        (*(static_cast<Serial*>(film_pointer))) = info_depot.acts.serial_st.top();
        info_depot.acts.serial_st.pop();
        list_wig_pointer = FilmQItem::get_FilmQItem_in_listWidget(ui->listWidg_serial, film_id);
    }
    else {
        (*(static_cast<SeriesMovie*>(film_pointer))) = info_depot.acts.serMovies_st.top();
        info_depot.acts.serMovies_st.pop();
        list_wig_pointer = FilmQItem::get_FilmQItem_in_listWidget(ui->listWidg_serMovie, film_id);
    }                                       // восстановление прежнего имени
    list_wig_pointer->setText(film_pointer->get_name());
    if (info_depot.user_is_authed == true)
        load_user_lists_in_widgets();
}

// восстановить удаленный фильм
void HFsLib::recover_deleted_info() {
    int film_id = info_depot.recover_deleted_film();
    load_films_in_list_widg();

    info_depot.recover_deleted_film_in_user_lists(film_id);
    if (info_depot.user_is_authed == true)
        load_user_lists_in_widgets();
}