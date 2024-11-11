#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "HFsLibTemplate.cpp"

QT_BEGIN_NAMESPACE   
namespace Ui { class HFsLibClass; };
QT_END_NAMESPACE

class HFsLib : public QMainWindow         // класс HFsLib, унаследованный от QT класса QMainWindow -
{                                         // основное окно приложния
    Q_OBJECT                              // макрос, позволяющий работать с макросами QT

public:
    HFsLib(QWidget *parent = nullptr);    // конструктор объекта класса HFsLib
    ~HFsLib();                            // деструктор

private slots:                            // слот - функция, вызываемая в ответ на определенный сигнал

    // слоты на первой странице
    void go_to_user_lists();              // перейти на страницу пользователя
    void save_progress();                 // сохранить информацию
    void go_to_film_lib();                // на страницу - библиотеку с фильмами

    // визуальная работа со списками текущего пользователя
    void load_user_lists_in_widgets();                           // выгрузить информацию о списках текущ. пользователя в соответствующие лист-виджеты
    void load_just_marked_users_list_in_widget(                  // выгрузить информацию о простых заметках
        QListWidget* _listWidg, Ring<FilmMark>* _ring);
    void load_rating_mark_user_list_in_widget();                 // выгрузить информацию о заметках c оценками фильма
    void load_stop_point_marked_lists_in_widget(                 // выгрузить информацию о заметках c точкой остановки просмотра
        QListWidget* _listWidg, Ring<FilmStopPointMark>* _ring);

    // слоты на странице - библиотеке
    void go_back_to_main_page();          // возврат на главную страницу
    void add_film_info();                 // добавить информацию о фильме
    void see_film_info();                 // просмотреть информацию о фильме
    void del_film_info();                 // удалить информацию о фильме
    void redact_film_info();              // редактировать информацию о фильме
    void search_film();                   // поиск фильма
    void free_search_film();              // очистить параметры поиска
    void get_random_film();               // вывести информацию о случайном фильме
    void show_sort_menu();                // вывод меню выбора сортировки
    void cancel_action();                 // отмена действия (редактирования или удаления фильма из базы)

    // работа с ПКМ меню
    void showContextMenu();                   // выбор контекстного меню для создания
    void show_context_menu_from_film_base();  // создание контекстного меню для страницы - библиотеки
    void show_comtext_menu_from_users_lists();// создание контекстного иеню для страницы пользователя

    // слоты на странице пользователя
    void leave_account();                     // покинуть аккаунт
    void sinMovies_visibility_control();      // контроль отображения одиночных полнометражных фильмов
    void serials_visibility_control();        // контроль отображения сериалов
    void serMovies_visibility_control();      // контроль отображения серийных полнометражных фильмов 

    void set_visibility_in_userListWidgets(          // установить видимость выбранного типа фильма
        bool hidden_cond, int film);
    void set_visibility_in_chosenUserListWidget(             // установить видимость выбранного типа фильма 
        bool hidden_cond, int film, QListWidget* _listWidg); // в выбранном пользовательском листе виджете

    void load_info_base();                         // загрузить информацию из файлов
    void load_films_in_list_widg();                // выгрузить информацию о фильмах в виджеты в окне
    FilmQItem* get_chosen_film();                  // получить указатель на выбранный пользователем фильм
    void set_visible_stats_in_searches();          // установка параметров видимости после выполнения поиска

    // сортировки списков по определенному параметру:
    void sort_by_id_ascendingly();     // - по ID по возрастанию
    void sort_by_id_descendingly();    // - по ID по убыванию
    void sort_by_name();               // - по имени
    void sort_by_country();            // - по стране
    void sort_by_year_ascendingly();   // - по году по возрастанию
    void sort_by_year_descendingly();  // - по году по убыванию
    void sort_by_genre();              // - по жанру
    void sort_by_rating();             // - по рейтингу

    // методы добавления фильмов в пользовательские списки:
    void add_to_inPlans();         // - "в планах"
    void add_to_watched();         // - "просмотрено"
    void add_to_favourites();      // - "любимое"
    void add_to_inProcess();       // - "в процессе"
    void add_to_abandoned();       // - "брошено"
    void add_to_marked();          // - "дана оценка"
    void add_filmMark_info_in_user_list                   // добавить обычную заметку о фильме в пользовательский список
    (QListWidget* _listWidg, Ring<FilmMark>* _ring);
    void add_filmMark_stopPointInfo_in_user_list          // добавить заметку об остановке в просмотре в пользовательский список
    (QListWidget* _listWidg, Ring<FilmStopPointMark>* _ring);

    void show_in_film_base();                       // перенести к выбранному фильму на странице - "базе"
    void delete_from_user_list();                   // удалить заметку из пользовательского списка
    void delete_rating_mark(FilmRatingMark* _rm);   // удалить рейтинговую оценку о фильме
    void redact_mark();                                  // редактировать пользовательскую заметку
    void redact_rating_mark(FilmRatingMark* _rm);        // редактировать заметку - оценку 
    void redact_stopPoint_mark(FilmStopPointMark* _spM); // редактировать заметку - точку остановки в просмотре

    void return_previous_redact_info();       // отменить редактирование фильма
    void recover_deleted_info();              // отменить удаление фильма
private:
    Ui::HFsLibClass *ui;                           // указатель на объект "User Interface"
    InformationDepot info_depot;                   // "хранилище" с информацией
};

#endif
