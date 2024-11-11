#ifndef USER_LISTS_SAVE_TEMPLATES_H
#define USER_LISTS_SAVE_TEMPLATES_H

#include "UserInfo.h"
#include "SeriesMovie.h"
#include "Serial.h"
#include "Algorithm.cpp"
#include "ContainerExceptions.h"

// шаблонный класс статических методов для записи/считывания из бинарного файла информации о пользовательском списке
template<class T>
class UserListsSaveFunctions
{
public:
	// шаблон дозаписи информации из кольца в файл
	static void additional_write_user_list_info_in_bin_file(fstream& _ff, Ring<T>* _ring, int us_id);

	// шаблон записи пользовательского списка в файл
	static void save_template_users_lists_info_in_bin_file(const char* _fName, Ring<UserInfo>* _users, int list_num);

	// шаблон считывания пользовательского списка из файла
	static void get_template_users_lists_info_from_bin_file(const char* _fName, Ring<UserInfo>* _users, int list_num);
};


#endif 