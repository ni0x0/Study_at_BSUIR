#include "HFsLibTemplate.h"

// добавить фильмы из кольца в лист-виджет
template<class T>
void HFsLibTemplate::add_filmInfo_in_listWidg(QListWidget* _listWidg, Ring<T>* _ring) {
    if (_ring->get_nodesNumber() == 0)
        return;
    _listWidg->clear();
    if (_ring->get_nodesNumber()) {
        class Ring<T>::Iterator curr = _ring->begin();

        do {
            _listWidg->addItem(new FilmQItem((*curr).get_name(), (*curr).get_id()));
            curr++;
        } while (curr != _ring->begin());
    }
}

// установить видимость фильмов, подходящих условию поиска
template<class T>
void HFsLibTemplate::show_searched_filmInfo_in_listWidg(QListWidget* _listWidg, Ring<T>* _ring, T& l_obj, T& h_obj) {
    if (_ring->get_nodesNumber() == 0)
        return;
    class Ring<T>::Iterator curr = _ring->begin();
    int i = 0;

    do {
        if (*curr >= l_obj && *curr <= h_obj)
            _listWidg->setRowHidden(i, 0);
        else  _listWidg->setRowHidden(i, 1);

        i++;
        curr++;
    } while (curr != _ring->begin());
}

// сортировать фильмы по имени
template<class T>
void HFsLibTemplate::sort_by_name_template(QListWidget* _listWidg, Ring<T>* _ring) {
    if (_ring->get_nodesNumber() == 0)
        return;
    Algorithm::sort_by_condition<class Ring<T>::Iterator, T>(_ring->begin(), _ring->end(),
        [](T& obj1, T& obj2) {
            if (strcmp(obj1.get_name(), obj2.get_name()) > 0)
            return false;
            else
                return true;
        });
    HFsLibTemplate::add_filmInfo_in_listWidg<T>(_listWidg, _ring);
}

// сортировать фильмы по ID 
template<class T>
void HFsLibTemplate::sort_by_id_template(QListWidget* _listWidg, Ring<T>* _ring, bool _is_increase) {
    if (_ring->get_nodesNumber() == 0)
        return;
    if (_is_increase == true) {
        Algorithm::sort_by_condition<class Ring<T>::Iterator, T>(_ring->begin(), _ring->end(),
            [](T& obj1, T& obj2) {
                if (obj1.get_id() > obj2.get_id())
                return false;
                else
                    return true;
            });
    }
    else {
        Algorithm::sort_by_condition<class Ring<T>::Iterator, T>(_ring->begin(), _ring->end(),
            [](T& obj1, T& obj2) {
                if (obj1.get_id() < obj2.get_id())
                return false;
                else
                    return true;
            });
    }
    HFsLibTemplate::add_filmInfo_in_listWidg<T>(_listWidg, _ring);
}

// сортировать фильмы по стране производства
template<class T>
void HFsLibTemplate::sort_by_country_template(QListWidget* _listWidg, Ring<T>* _ring) {
    if (_ring->get_nodesNumber() == 0)
        return;
    Algorithm::sort_by_condition<class Ring<T>::Iterator, T>(_ring->begin(), _ring->end(),
        [](T& obj1, T& obj2) {
            if (strcmp(obj1.get_prod_country(), obj2.get_prod_country()) > 0)
            return false;
            else
                return true;
        });
    HFsLibTemplate::add_filmInfo_in_listWidg<T>(_listWidg, _ring);
}

// сортировать фильмы по году производства
template<class T>
void  HFsLibTemplate::sort_by_year_template(QListWidget* _listWidg, Ring<T>* _ring, bool _is_increase){
    if (_ring->get_nodesNumber() == 0)
        return;
    if (_is_increase == true) {
        Algorithm::sort_by_condition<class Ring<T>::Iterator, T>(_ring->begin(), _ring->end(),
            [](T& obj1, T& obj2) {
                if (obj1.get_prod_year() > obj2.get_prod_year())
                return false;
                else
                    return true;
            });
    }
    else {
        Algorithm::sort_by_condition<class Ring<T>::Iterator, T>(_ring->begin(), _ring->end(),
            [](T& obj1, T& obj2) {
                if (obj1.get_prod_year() < obj2.get_prod_year())
                return false;
                else
                    return true;
            });
    }
    HFsLibTemplate::add_filmInfo_in_listWidg<T>(_listWidg, _ring);
}

// сортировать фильмы по жанру
template<class T>
void HFsLibTemplate::sort_by_genre_template(QListWidget* _listWidg, Ring<T>* _ring) {
    if (_ring->get_nodesNumber() == 0)
        return;
    Algorithm::sort_by_condition<class Ring<T>::Iterator, T>(_ring->begin(), _ring->end(),
        [](T& obj1, T& obj2) {
            if (obj1.get_genre() > obj2.get_genre())
            return false;
            else
                return true;
        });
    HFsLibTemplate::add_filmInfo_in_listWidg<T>(_listWidg, _ring);
}

// сортировать фильмы по рейтингу
template<class T>
void HFsLibTemplate::sort_by_rating_template(QListWidget* _listWidg, Ring<T>* _ring) {
    if (_ring->get_nodesNumber() == 0)
        return;
    Algorithm::sort_by_condition<class Ring<T>::Iterator, T>(_ring->begin(), _ring->end(),
        [](T& obj1, T& obj2) {
            if (obj1.get_rating() < obj2.get_rating())
            return false;
            else
                return true;
        });
    HFsLibTemplate::add_filmInfo_in_listWidg<T>(_listWidg, _ring);
}

// получить ID случайного фильма из списка
template<class T>
int HFsLibTemplate::get_random_film_id(Ring<T>* _ring, InformationDepot* info_d) {
    if (_ring->get_nodesNumber() == 0)
        return -1;

    int random_id = 0;
    int max_f_id = (*(_ring->end())).get_id();
    int min_f_id = (*(_ring->begin())).get_id();

    random_device random_device;                                 // источник энтропии
    mt19937 generator(random_device());                          // генератор случайных чисел
    uniform_int_distribution<> distribution(min_f_id, max_f_id); // диапазон распределения
    while (1) {
        random_id = distribution(generator);                     // получение случайного числа
        Film* _film_to_check = info_d->find_film(random_id);
        if (_film_to_check)
            break;
    }
    return random_id;
}
