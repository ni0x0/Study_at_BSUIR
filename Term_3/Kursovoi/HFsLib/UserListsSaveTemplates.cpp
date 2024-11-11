#include "UserListsSaveTemplates.h"

// шаблонный метод дозаписи информации о переданном списке пользователя в файл
template <class T>
void UserListsSaveFunctions<T>::additional_write_user_list_info_in_bin_file(fstream& _ff, Ring<T>* _user_ring, int us_id) {
	if (_user_ring->get_nodesNumber()) {
		class Ring<T>::Iterator filmMark_iter = _user_ring->begin();

		do {
			_ff.write(reinterpret_cast<char*>(&us_id), sizeof(us_id));
			_ff << (*filmMark_iter);
			filmMark_iter++;
		} while (filmMark_iter != _user_ring->begin());
	}
}

// шаблонный метод записи выбранных пользовательских списков в файл
template <class T>
void UserListsSaveFunctions<T>::save_template_users_lists_info_in_bin_file(const char* _fName, Ring<UserInfo>* _users, int list_num) {
	fstream ff;
	ff.open(_fName, ios_base::binary | ios_base::out);
	if (_users->get_nodesNumber()) {
		class Ring<UserInfo>::Iterator user_iter = _users->begin();

		do {
			switch (list_num) {     // запись определенного (по переменной list_num) пользовательского списка в файл 
			case 1:                 // 1 - список "любимые"
				additional_write_user_list_info_in_bin_file(ff, reinterpret_cast<Ring<T>*>(&((*(user_iter)).lists.favours)), (*(user_iter)).get_id());
				break;              // 2 - "в планах"
			case 2: additional_write_user_list_info_in_bin_file(ff, reinterpret_cast<Ring<T>*>(&((*(user_iter)).lists.in_plans)), (*(user_iter)).get_id());
				break;              // 3 - "просмотрено"
			case 3: additional_write_user_list_info_in_bin_file(ff, reinterpret_cast<Ring<T>*>(&((*(user_iter)).lists.watched)), (*(user_iter)).get_id());
				break;              // 4 - "дана оценка"
			case 4: additional_write_user_list_info_in_bin_file(ff, reinterpret_cast<Ring<T>*>(&((*(user_iter)).lists.rated)), (*(user_iter)).get_id());
				break;              // 3 - "в процессе"
			case 5: additional_write_user_list_info_in_bin_file(ff, reinterpret_cast<Ring<T>*>(&((*(user_iter)).lists.in_process)), (*(user_iter)).get_id());
				break;              // 3 - "брошено"
			case 6: additional_write_user_list_info_in_bin_file(ff, reinterpret_cast<Ring<T>*>(&((*(user_iter)).lists.abandoned)), (*(user_iter)).get_id());
				break;
			}

			user_iter++;
		} while (user_iter != _users->begin());
	}
	ff.close();
}

// шаблон считывания пользовательского списка из файла
template <class T>
void UserListsSaveFunctions<T>::get_template_users_lists_info_from_bin_file(const char* _fName, Ring<UserInfo>* _users, int list_num) {
	fstream ff;
	ff.open(_fName, ios_base::binary | ios_base::in);
	if (ff.bad())
		throw FileException(1, "FILES ARE CORRUPTED\nPROGRAM CANNOT WORK IN RIGHT WAY");
	if (ff.is_open()) {
		while (1) {
			int temp_id = -1;
			ff.read(reinterpret_cast<char*>(&temp_id), sizeof(temp_id));
			if (temp_id == -1)
				break;

			T tempInfo;                     // считанная информация о пользовательской заметке
			ff >> tempInfo;
			UserInfo user_to_find; user_to_find.set_id(temp_id);   
			UserInfo* read_user = NULL;

			if ((Algorithm::find_match<Ring<UserInfo>::Iterator, UserInfo>(_users->begin(), _users->end(), user_to_find)).check())
				read_user = &(*(Algorithm::find_match<Ring<UserInfo>::Iterator, UserInfo>(_users->begin(), _users->end(), user_to_find)));

			if (read_user != NULL) {                                     // если найден пользователь со считанным ID, заметка записывается в определенный 
					switch (list_num) {                                  // (по переменной list_num) пользовательский список
					case 1: read_user->lists.favours.add(tempInfo);      // 1 - список "любимые"
						break;                                           // 2 - "в планах"
					case 2: read_user->lists.in_plans.add(tempInfo);     
						break;                                           // 3 - "просмотрено"
					case 3: read_user->lists.watched.add(tempInfo);     
						break;                                           // 4 - "дана оценка"
					case 4: read_user->lists.rated.add(static_cast<FilmRatingMark&>(dynamic_cast<FilmMark&>(tempInfo)));
							break;                                       // 5 - "в процессе"
					case 5: read_user->lists.in_process.add(static_cast<FilmStopPointMark&>(dynamic_cast<FilmMark&>(tempInfo)));
						break;                                           // 6 - "брошено"
					case 6: read_user->lists.abandoned.add(static_cast<FilmStopPointMark&>(dynamic_cast<FilmMark&>(tempInfo)));
						break;
					}
			}
		}
	}
	else throw FileException(0, "FILES OPENINING ERROR\nSOME DATA LOST");
	ff.close();
}
