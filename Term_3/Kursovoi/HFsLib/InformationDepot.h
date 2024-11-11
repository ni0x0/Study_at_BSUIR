#ifndef INFORM_DEPOT_H
#define INFORM_DEPOT_H

#include "UserListsSaveTemplates.cpp"
#include "ActionParams.h"

// класс - "хранилище" информации для работы 
class InformationDepot
{
public:
	InformationDepot();                            // конмтруктор
	~InformationDepot();                           // деструктор

	Ring<UserInfo> usersInfo_r;                    // кольцо с информацией о пользователях
	bool user_is_authed;                           // показатель того, авторизован ли пользователь
	Ring<UserInfo>::Iterator current_user;         // итератор, указывающий на текущего пользователя

	Ring<Movie> moviesInfo_r;                      // кольцо с информацией об одиночных полнометражных фильмах
	Ring<Serial> serialsInfo_r;                    // кольцо с информацией о сериалах
	Ring<SeriesMovie> seriesMoviesInfo_r;          // кольцо с информацией о серийных полнометражных фильмах

	ActionsParams acts;                            // объект класса для сохранения информации о действиях:
	                                               // (отмена редактирования фильма/его удаления, поиск (его параметры))

	Film* find_film(int _id);                      // найти фильм в базе по его ID
	FilmMark* find_film_mark_in_user_list(         // найти заметку о фильме в определенном пользовательском списке
		int _id, int user_list_num);               // текущего пользователя

	void get_info_from_bin_file();               // восстановить информацию из бинарного файла
	void get_users_lists_info_from_bin_file();   // восстановить информацию о пользовательских списках

	void save_info_in_bin_file();                // сохранить всю информацию в бинарный файл
	void save_users_lists_info_in_bin_file();    // сохранить информацию о пользовательских списках 

	void del_film_info_from_users_lists(Film* _fp);        // удалить информацию о выбранном фильме тз пользовательских списков
	bool save_chosen_previous_user_lists_info(             // сохранить в стек информацию о заметках об удаляемом фильме у текущего пользователя
		Film* _fp, UserInfo& _user);
	bool save_film_mark_info(Film* _fp, Ring<FilmMark>* _ring);                // получить информацию о наличии обычной заметки
	bool save_film_rating_mark_info(Film* _fp, Ring<FilmRatingMark>* _ring);   // получить информацию о наличии рейтинговой заметки  
	bool save_film_stopP_mark_info(Film* _fp, Ring<FilmStopPointMark>* _ring); // получить информацию о наличии заметки - точки остановки 

	void save_previous_film_info(int film_id);                 // сохранить в стек текущую информацию о фильме
	bool check_saved_information_on_repeat(int film_id);       // проверить на совпадение последнюю занесенную в стек информацию

	int recover_deleted_film();                                // восстановить информацию об удаленном фильме
	void recover_deleted_film_in_user_lists(int film_id);      // восстановить информацию об удаленном фильме в пользовательских списках
	void recover_deleted_film_in_chosen_user_lists(            // восстановить информацию о фильме в списках выбранного пользователя 
		UserInfo* _user, int film_id);
};

#endif



























//void save_users_info();                   // сохранить информацию о пользователях
//void save_sin_movies_info();              // получить информацию об одиночных полнометражных фильмах
//void save_serials_info();                 // получить информацию о сериалах
//void save_ser_movies_info();              // получить информацию о серийных полнометражных фильмах

//void get_users_info();                    // получить информацию о пользователях
//void get_sin_movies_info();               // получить информацию об одиночных полнометражных фильмах
//void get_serials_info();                  // получить информацию о сериалах
//void get_ser_movies_info();               // получить информацию о серийных полнометражных фильмах
// 
// 
//	fstream ff;
//	ff.open("0users.bin", ios_base::binary | ios_base::out);
//	if (usersInfo_r.get_nodesNumber()) {
//		Ring<UserInfo>::Iterator us_info_it = usersInfo_r.begin();
//
//		do {
//			ff << (*us_info_it);
//			us_info_it++;
//		} while (us_info_it != usersInfo_r.begin());
//	}
//	ff.close();
//}


//T emptyInfo;
//
//fstream ff;
//ff.open(_fName, ios_base::binary | ios_base::in);
//if (ff.is_open()) {
//	while (1) {
//		T tempInfo;
//		ff >> tempInfo;
//
//		if (emptyInfo == tempInfo)
//			break;
//		//usersInfo_r.add(tempUserInfo);
//		(*_ring).add(tempInfo);
//	}
//}
//ff.close();






//friend class HSsLib;
	//
	//friend class AuthorWindow;


//#include "HFsLib.h"
//class HSsLib;

