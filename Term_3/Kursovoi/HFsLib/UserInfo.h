#ifndef USER_INFO_H
#define USER_INFO_H

#include<iostream>
#include <fstream>
#include "UserLists.h"
using namespace std;

#define USER_NAME_LEN 15                 // длина имени пользователя по умочанию
#define PASSWORD_LEN 10                  // длина пароля по умолчанию

// класс с информацией о пользователе
class UserInfo
{
	char name[USER_NAME_LEN];            // имя
	int ID;                              // ID
	char password[PASSWORD_LEN];         // пароль

	static int user_id_counter;          // статический счетчик ID пользователей

public:
	UserInfo();                          // конструкторы
	UserInfo(const char* _name, const char* _passw);
	UserInfo(const UserInfo& oth);
	~UserInfo() {}
	UserLists lists;                    // списки с пользовательскими пометками о фильме

	UserInfo& operator = (const UserInfo& oth);                       // перегрузка оператора присваивания
	bool operator == (const UserInfo& oth);                           // перегрузка оператора "равно"

	friend fstream& operator << (fstream& out, UserInfo& oth);        // перегрузка оператора записи в бинарный файл
	friend fstream& operator >> (fstream& in, UserInfo& oth);         // перегрузка оператора считывания из бинарного файла

	void set_name(const char*);                                       // методы для изменения значений в полях
	void set_id(int _id);
	void set_passw(const char*);
	static void set_id_counter(int _id);

	char* get_name();                                                 // методы для получения значений из полей
	int get_id();
	char* get_passw();
	static int get_id_counter();
};

#endif