#include "InformationDepot.h"

// конструктор
InformationDepot::InformationDepot() : current_user(){
	user_is_authed = false;
}

// деструктор
InformationDepot::~InformationDepot() {
	save_info_in_bin_file();
}

// найти фильм в базе по его ID
Film* InformationDepot::find_film(int _id) {
	Film* f_to_ret = NULL;
	if (_id / 1000 == 0) {
		Movie movie_obj;
		movie_obj.set_id(_id);
		f_to_ret = dynamic_cast<Film*>(&(*(Algorithm::find_match<Ring<Movie>::Iterator, Movie>(moviesInfo_r.begin(), moviesInfo_r.end(), movie_obj))));
	}
	if (_id / 1000 == 1) {
		Serial serial_obj;
		serial_obj.set_id(_id);
		f_to_ret = dynamic_cast<Film*>(&(*(Algorithm::find_match<Ring<Serial>::Iterator, Serial>(serialsInfo_r.begin(), serialsInfo_r.end(), serial_obj))));
	}
	if (_id / 1000 == 2) {
		SeriesMovie serMovie_obj;
		serMovie_obj.set_id(_id);
		f_to_ret = dynamic_cast<Film*>(&(*(Algorithm::find_match<Ring<SeriesMovie>::Iterator, SeriesMovie>
			(seriesMoviesInfo_r.begin(), seriesMoviesInfo_r.end(), serMovie_obj))));
	}
	return f_to_ret;
}

// найти заметку о фильме в определенном пользовательском списке текущего пользователя
FilmMark* InformationDepot::find_film_mark_in_user_list(int _id, int user_list_num) {
	FilmMark* fm_to_ret = NULL;
	if (user_list_num == 1 || user_list_num == 2 || user_list_num == 4) {
		FilmMark mark_obj;
		mark_obj.set_id(_id);
		if (user_list_num == 1)
			fm_to_ret = &(*(Algorithm::find_match<Ring<FilmMark>::Iterator, FilmMark>
				((*current_user).lists.in_plans.begin(), (*current_user).lists.in_plans.end(), mark_obj)));
		if (user_list_num == 2)
			fm_to_ret = &(*(Algorithm::find_match<Ring<FilmMark>::Iterator, FilmMark>
				((*current_user).lists.watched.begin(), (*current_user).lists.watched.end(), mark_obj)));
		if (user_list_num == 4)
			fm_to_ret = &(*(Algorithm::find_match<Ring<FilmMark>::Iterator, FilmMark>
				((*current_user).lists.favours.begin(), (*current_user).lists.favours.end(), mark_obj)));
	}
	if (user_list_num == 5) {
		FilmRatingMark rat_mark_obj;
		rat_mark_obj.set_id(_id);
		fm_to_ret = dynamic_cast<FilmMark*>(&(*(Algorithm::find_match<Ring<FilmRatingMark>::Iterator, FilmRatingMark>
			((*current_user).lists.rated.begin(), (*current_user).lists.rated.end(), rat_mark_obj))));
	}
	if (user_list_num == 0 || user_list_num == 3) {
		FilmStopPointMark sp_mark_obj;
		sp_mark_obj.set_id(_id);
		if (user_list_num == 0)
			fm_to_ret = dynamic_cast<FilmMark*>(&(*(Algorithm::find_match<Ring<FilmStopPointMark>::Iterator, FilmStopPointMark>
				((*current_user).lists.in_process.begin(), (*current_user).lists.in_process.end(), sp_mark_obj))));
		if (user_list_num == 3)
			fm_to_ret = dynamic_cast<FilmMark*>(&(*(Algorithm::find_match<Ring<FilmStopPointMark>::Iterator, FilmStopPointMark>
				((*current_user).lists.abandoned.begin(), (*current_user).lists.abandoned.end(), sp_mark_obj))));
	}

	return fm_to_ret;
}

//// восстановить информацию из бинарного файла
//void InformationDepot::get_info_from_bin_file() {
//	
//	RingSaveFunctions<UserInfo>::get_info_from_bin_file("0users.bin", &usersInfo_r);
//	if (usersInfo_r.get_nodesNumber())
//		UserInfo::set_id_counter((*(usersInfo_r.end())).get_id() + 1);
//
//	RingSaveFunctions<Movie>::get_info_from_bin_file("0sin_movies.bin", &moviesInfo_r);
//	if (moviesInfo_r.get_nodesNumber())
//		Movie::set_movie_id_c((*(moviesInfo_r.end())).get_id() + 1);
//	
//	RingSaveFunctions<Serial>::get_info_from_bin_file("0serials.bin", &serialsInfo_r);
//	if (serialsInfo_r.get_nodesNumber())
//		Serial::set_ser_id_c((*(serialsInfo_r.end())).get_id() + 1);
//
//	RingSaveFunctions<SeriesMovie>::get_info_from_bin_file("0ser_movies.bin", &seriesMoviesInfo_r);
//	if (seriesMoviesInfo_r.get_nodesNumber())
//		SeriesMovie::set_seria_movie_id_c((*(seriesMoviesInfo_r.end())).get_id() + 1);
//
//	get_users_lists_info_from_bin_file();
//}
//
//// восстановить информацию о пользовательских списках
//void InformationDepot::get_users_lists_info_from_bin_file() {
//	UserListsSaveFunctions<FilmMark>::get_template_users_lists_info_from_bin_file("0favours", &usersInfo_r, 1);         // favourites
//	UserListsSaveFunctions<FilmMark>::get_template_users_lists_info_from_bin_file("0inPlans", &usersInfo_r, 2);         // inPlans
//	UserListsSaveFunctions<FilmMark>::get_template_users_lists_info_from_bin_file("0watched", &usersInfo_r, 3);         // watched
//
//	UserListsSaveFunctions<FilmRatingMark>::get_template_users_lists_info_from_bin_file("0marked", &usersInfo_r, 4);    // marked
//
//	UserListsSaveFunctions<FilmStopPointMark>::get_template_users_lists_info_from_bin_file("0inProc", &usersInfo_r, 5);       // inProcess
//	UserListsSaveFunctions<FilmStopPointMark>::get_template_users_lists_info_from_bin_file("0abandoned", &usersInfo_r, 6);    // abandoned
//}
//
//// сохранить всю информацию в бинарный файл
//void InformationDepot::save_info_in_bin_file() {
//
//	RingSaveFunctions<UserInfo>::save_info_in_bin_file("0users.bin", &usersInfo_r);
//	RingSaveFunctions<Movie>::save_info_in_bin_file("0sin_movies.bin", &moviesInfo_r);
//	RingSaveFunctions<Serial>::save_info_in_bin_file("0serials.bin", &serialsInfo_r);
//	RingSaveFunctions<SeriesMovie>::save_info_in_bin_file("0ser_movies.bin", &seriesMoviesInfo_r);
//
//	save_users_lists_info_in_bin_file();
//}
//
//// сохранить информацию пользовательских списков
//void InformationDepot::save_users_lists_info_in_bin_file() {
//	UserListsSaveFunctions<FilmMark>::save_template_users_lists_info_in_bin_file("0favours", &usersInfo_r, 1);         // favourites
//	UserListsSaveFunctions<FilmMark>::save_template_users_lists_info_in_bin_file("0inPlans", &usersInfo_r, 2);         // inPlans
//	UserListsSaveFunctions<FilmMark>::save_template_users_lists_info_in_bin_file("0watched", &usersInfo_r, 3);         // watched
//
//	UserListsSaveFunctions<FilmRatingMark>::save_template_users_lists_info_in_bin_file("0marked", &usersInfo_r, 4);    // marked
//
//	UserListsSaveFunctions<FilmStopPointMark>::save_template_users_lists_info_in_bin_file("0inProc", &usersInfo_r, 5);    // inProcess
//	UserListsSaveFunctions<FilmStopPointMark>::save_template_users_lists_info_in_bin_file("0abandoned", &usersInfo_r, 6); // abandoned
//}

// восстановить информацию из бинарного файла
void InformationDepot::get_info_from_bin_file() {

    RingSaveFunctions<UserInfo>::get_info_from_bin_file("0users.bin", &usersInfo_r);
    if (usersInfo_r.get_nodesNumber())
        UserInfo::set_id_counter((*(usersInfo_r.end())).get_id() + 1);

    RingSaveFunctions<Movie>::get_info_from_bin_file("0sin_movies.bin", &moviesInfo_r);
    if (moviesInfo_r.get_nodesNumber())
        Movie::set_movie_id_c((*(moviesInfo_r.end())).get_id() + 1);

    RingSaveFunctions<Serial>::get_info_from_bin_file("0serials.bin", &serialsInfo_r);
    if (serialsInfo_r.get_nodesNumber())
        Serial::set_ser_id_c((*(serialsInfo_r.end())).get_id() + 1);

    RingSaveFunctions<SeriesMovie>::get_info_from_bin_file("0ser_movies.bin", &seriesMoviesInfo_r);
    if (seriesMoviesInfo_r.get_nodesNumber())
        SeriesMovie::set_seria_movie_id_c((*(seriesMoviesInfo_r.end())).get_id() + 1);

    get_users_lists_info_from_bin_file();
}

// восстановить информацию о пользовательских списках
void InformationDepot::get_users_lists_info_from_bin_file() {
    UserListsSaveFunctions<FilmMark>::get_template_users_lists_info_from_bin_file("0favours.bin", &usersInfo_r, 1);         // favourites
    UserListsSaveFunctions<FilmMark>::get_template_users_lists_info_from_bin_file("0inPlans.bin", &usersInfo_r, 2);         // inPlans
    UserListsSaveFunctions<FilmMark>::get_template_users_lists_info_from_bin_file("0watched.bin", &usersInfo_r, 3);         // watched

    UserListsSaveFunctions<FilmRatingMark>::get_template_users_lists_info_from_bin_file("0marked.bin", &usersInfo_r, 4);    // marked

    UserListsSaveFunctions<FilmStopPointMark>::get_template_users_lists_info_from_bin_file("0inProc.bin", &usersInfo_r, 5);       // inProcess
    UserListsSaveFunctions<FilmStopPointMark>::get_template_users_lists_info_from_bin_file("0abandoned.bin", &usersInfo_r, 6);    // abandoned
}

// сохранить всю информацию в бинарный файл
void InformationDepot::save_info_in_bin_file() {

    RingSaveFunctions<UserInfo>::save_info_in_bin_file("0users.bin", &usersInfo_r);
    RingSaveFunctions<Movie>::save_info_in_bin_file("0sin_movies.bin", &moviesInfo_r);
    RingSaveFunctions<Serial>::save_info_in_bin_file("0serials.bin", &serialsInfo_r);
    RingSaveFunctions<SeriesMovie>::save_info_in_bin_file("0ser_movies.bin", &seriesMoviesInfo_r);

    save_users_lists_info_in_bin_file();
}

// сохранить информацию пользовательских списков
void InformationDepot::save_users_lists_info_in_bin_file() {
    UserListsSaveFunctions<FilmMark>::save_template_users_lists_info_in_bin_file("0favours.bin", &usersInfo_r, 1);         // favourites
    UserListsSaveFunctions<FilmMark>::save_template_users_lists_info_in_bin_file("0inPlans.bin", &usersInfo_r, 2);         // inPlans
    UserListsSaveFunctions<FilmMark>::save_template_users_lists_info_in_bin_file("0watched.bin", &usersInfo_r, 3);         // watched

    UserListsSaveFunctions<FilmRatingMark>::save_template_users_lists_info_in_bin_file("0marked.bin", &usersInfo_r, 4);    // marked

    UserListsSaveFunctions<FilmStopPointMark>::save_template_users_lists_info_in_bin_file("0inProc.bin", &usersInfo_r, 5);    // inProcess
    UserListsSaveFunctions<FilmStopPointMark>::save_template_users_lists_info_in_bin_file("0abandoned.bin", &usersInfo_r, 6); // abandoned
}

// удалить информацию о выбранном фильме тз пользовательских списков
void InformationDepot::del_film_info_from_users_lists(Film* _fp) {
    if (this->usersInfo_r.get_nodesNumber()) {                     // проход по всем пользователям
        class Ring<UserInfo>::Iterator user_iter = this->usersInfo_r.begin();
        int users_count = 0;                                       // cчетчик пользователей, у которых в списках присутствует удаляемый фильм
        do {                                                       // сохранить в стек информацию о заметках с удаляемым фильмом текущ. пользователя
            if (save_chosen_previous_user_lists_info(_fp, *user_iter)) {
                (*user_iter).lists.in_plans.delete_all_match_elems(FilmMark(_fp));
                (*user_iter).lists.favours.delete_all_match_elems(FilmMark(_fp));
                (*user_iter).lists.watched.delete_all_match_elems(FilmMark(_fp));
                (*user_iter).lists.rated.delete_all_match_elems(FilmRatingMark(_fp));
                (*user_iter).lists.abandoned.delete_all_match_elems(FilmStopPointMark(_fp));
                (*user_iter).lists.in_process.delete_all_match_elems(FilmStopPointMark(_fp));

                users_count++;
            }

            user_iter++;
        } while (user_iter != this->usersInfo_r.begin());
        this->acts.actions_st.push(users_count);                  // занести в стек количество пользователей, сохранивших фильм
    }
}

// сохранить в стек информацию о заметках об удаляемом фильме у текущего пользователя
bool  InformationDepot::save_chosen_previous_user_lists_info(Film* _fp, UserInfo& _user) {
    int lists_amount = 0;
    // пройтись по спискам и сохранить информацию о нужных заметках
    if (save_film_mark_info(_fp, &(_user.lists.in_plans))) {
        this->acts.actions_st.push(1);
        lists_amount++;
    }
    if (save_film_mark_info(_fp, &(_user.lists.watched))) {
        this->acts.actions_st.push(2);
        lists_amount++;
    }
    if (save_film_mark_info(_fp, &(_user.lists.favours))) {
        this->acts.actions_st.push(4);
        lists_amount++;
    }
    if (save_film_rating_mark_info(_fp, &(_user.lists.rated))) {
        this->acts.actions_st.push(5);
        lists_amount++;
    }
    if (save_film_stopP_mark_info(_fp, &(_user.lists.in_process))) {
        this->acts.actions_st.push(0);
        lists_amount++;
    }
    if (save_film_stopP_mark_info(_fp, &(_user.lists.abandoned))) {
        this->acts.actions_st.push(3);
        lists_amount++;
    }
    
    if (lists_amount) {
        this->acts.actions_st.push(lists_amount);
        this->acts.actions_st.push(_user.get_id());
        return true;
    }
    else return false;
}

// получить информацию о наличии обычной заметки
bool  InformationDepot::save_film_mark_info(Film* _fp, Ring<FilmMark>* _ring) {
    if (_ring->get_nodesNumber()) {
        if ((Algorithm::find_match(_ring->begin(), _ring->end(), FilmMark(_fp)).check()))
            return true;
        return false;
    }
    else return false;
}

// получить информацию о наличии рейтинговой заметки  
bool  InformationDepot::save_film_rating_mark_info(Film* _fp, Ring<FilmRatingMark>* _ring) {
    if (_ring->get_nodesNumber()) {
        if ((Algorithm::find_match(_ring->begin(), _ring->end(), FilmRatingMark(_fp)).check())) {
            FilmRatingMark rat_m = *(Algorithm::find_match(_ring->begin(), _ring->end(), FilmRatingMark(_fp)));
            this->acts.actions_st.push(rat_m.get_ratingM());  // занести в стек данную пользователем оценку
            return true;
        }
        return false;
    }
    else return false;
}

// получить информацию о наличии заметки - точки остановки 
bool  InformationDepot::save_film_stopP_mark_info(Film* _fp, Ring<FilmStopPointMark>* _ring) {
    if (_ring->get_nodesNumber()) {
        if ((Algorithm::find_match(_ring->begin(), _ring->end(), FilmStopPointMark(_fp)).check())) {
            FilmStopPointMark sp_mark = *(Algorithm::find_match(_ring->begin(), _ring->end(), FilmStopPointMark(_fp)));
            this->acts.actions_st.push(sp_mark.get_minuteM());  // занести в стек час остновки
            this->acts.actions_st.push(sp_mark.get_hourM());    // занести в стек минуту остновки
            return true;
        }
        return false;
    }
    else return false;
}

// сохранить в стек текущую информацию о фильме
void InformationDepot::save_previous_film_info(int film_id) {
	    if (film_id / 1000 == 0) 
	        this->acts.sinMovies_st.push(*(static_cast<Movie*>(this->find_film(film_id))));
	    if (film_id / 1000 == 1)
	        this->acts.serial_st.push(*(static_cast<Serial*> (this->find_film(film_id))));
	    if (film_id / 1000 == 2) 
	        this->acts.serMovies_st.push(*(static_cast<SeriesMovie*> (this->find_film(film_id))));
}

// проверить на совпадение последнюю занесенную в стек информацию
bool InformationDepot::check_saved_information_on_repeat(int film_id) {
    Film* fp_to_check = this->find_film(film_id);
    if (film_id / 1000 == 0) {
        if (this->acts.sinMovies_st.top() == *(static_cast<Movie*>(fp_to_check)))
            return true;
    }
    else if (film_id / 1000 == 1) {
        if (this->acts.serial_st.top() == *(static_cast<Serial*>(fp_to_check)))
            return true;
    }
    else {
        if (this->acts.serMovies_st.top() == *(static_cast<SeriesMovie*>(fp_to_check)))
            return true;
    }
    return false;
}

// восстановить удаленный фильм
int InformationDepot::recover_deleted_film() {
    int film_id = this->acts.actions_st.top();   // получить ID удаленного фильма
    this->acts.actions_st.pop();
    if (film_id / 1000 == 0) {                          // восстановить информацию о нем
        this->moviesInfo_r.add(this->acts.sinMovies_st.top());
        this->acts.sinMovies_st.pop();
    }
    else if (film_id / 1000 == 1) {
        this->serialsInfo_r.add(this->acts.serial_st.top());
        this->acts.serial_st.pop();
    }
    else {
        this->seriesMoviesInfo_r.add(this->acts.serMovies_st.top());
        this->acts.serMovies_st.pop();
    }
    return film_id;
}

// восстановить информацию об удаленном фильме в пользовательских списках
void  InformationDepot::recover_deleted_film_in_user_lists(int film_id) {
    int _users_count = this->acts.actions_st.top();            // получить число
    this->acts.actions_st.pop();

    for (int i = 0; i < _users_count; i++) {
        int user_id = this->acts.actions_st.top();
        this->acts.actions_st.pop();

        UserInfo* curr_user;
        UserInfo user_to_find; user_to_find.set_id(user_id);
        curr_user = &(*(Algorithm::find_match<Ring<UserInfo>::Iterator, UserInfo>(this->usersInfo_r.begin(), this->usersInfo_r.end(), user_to_find)));

        int lists_count = this->acts.actions_st.top();
        this->acts.actions_st.pop();
        for (int i = 0; i < lists_count; i++) {
            recover_deleted_film_in_chosen_user_lists(curr_user, film_id);
        }
    }
}

void  InformationDepot::recover_deleted_film_in_chosen_user_lists(UserInfo* _user, int film_id) {
    int list_num = this->acts.actions_st.top();
    this->acts.actions_st.pop();

    if (list_num == 1 || list_num == 2 || list_num == 4) {
        FilmMark fm(film_id);
        if (list_num == 1)
            _user->lists.in_plans.add(fm);
        if (list_num == 2)
            _user->lists.watched.add(fm);
        if (list_num == 4)
            _user->lists.favours.add(fm);
    }
    if (list_num == 0 || list_num == 3) {
        int stPoint1 = this->acts.actions_st.top();
        this->acts.actions_st.pop();
        FilmStopPointMark fm(film_id, stPoint1, this->acts.actions_st.top());
        this->acts.actions_st.pop();
        if (list_num == 0)
            _user->lists.in_process.add(fm);
        if (list_num == 3)
            _user->lists.abandoned.add(fm);
    }
    if (list_num == 5) {
        FilmRatingMark fm(film_id, this->acts.actions_st.top());
        this->acts.actions_st.pop();
        _user->lists.rated.add(fm);
    }
}