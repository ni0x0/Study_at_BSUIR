#include "UserInfo.h"

int UserInfo::user_id_counter = 0;

// конструктор класса UserInfo по умолчанию
UserInfo::UserInfo() {
	name[0] = '\0';
	ID = -1;
	password[0] = '\0';
}

// конструктор класса UserInfo c параметрами
UserInfo::UserInfo(const char* _name, const char* _passw) {
	strcpy_s(name, USER_NAME_LEN, _name);
	strcpy_s(password, PASSWORD_LEN, _passw);

	ID = user_id_counter++;
}

// конструктор копирования
UserInfo::UserInfo(const UserInfo& oth) {
	strcpy_s(this->name, USER_NAME_LEN, oth.name);
	this->ID = oth.ID;
	strcpy_s(this->password, PASSWORD_LEN, oth.password);
}

// перегрузка оператора присваивания
UserInfo& UserInfo::operator = (const UserInfo& oth) {
	if (this == &oth)
		return *this;

	strcpy_s(this->name, USER_NAME_LEN, oth.name);
	this->ID = oth.ID;
	strcpy_s(this->password, PASSWORD_LEN, oth.password);

	return *this;
}

// перегрузка оператора "равно"
bool UserInfo::operator == (const UserInfo& oth) {
	if (this == &oth)
		return true;

	if (strcmp(this->name, oth.name) && oth.name[0] != '\0') return false;
	if (this->ID != oth.ID && oth.ID != -1) return false;
	if (strcmp(this->password, oth.password) && oth.password[0] != '\0') return false;
	return true;
}

// перегрузка оператора записи в бинарный файл
fstream& operator << (fstream& out, UserInfo& oth) {
	out.write(reinterpret_cast<char*>(&oth.name), sizeof(oth.name));
	out.write(reinterpret_cast<char*>(&oth.ID), sizeof(oth.ID));
	out.write(reinterpret_cast<char*>(&oth.password), sizeof(oth.password));

	return out;
}

// перегрузка оператора считывания из бинарного файла
fstream& operator >> (fstream& in, UserInfo& oth) {
	in.read(reinterpret_cast<char*>(&oth.name), sizeof(oth.name));
	in.read(reinterpret_cast<char*>(&oth.ID), sizeof(oth.ID));
	in.read(reinterpret_cast<char*>(&oth.password), sizeof(oth.password));

	return in;
}

// методы для изменения полей
void UserInfo::set_name(const char* _name) {
	strcpy_s(this->name, USER_NAME_LEN, _name);
}
void UserInfo::set_id(int _id) {
	this->ID = _id;
}
void UserInfo::set_passw(const char* _passw) {
	strcpy_s(this->password, PASSWORD_LEN, _passw);
}
void UserInfo::set_id_counter(int _id) {
	user_id_counter = _id;
}

// методы для получения значений из полей
char* UserInfo::get_name() {
	return this->name;
}
int UserInfo::get_id() {
	return this->ID;
}
char* UserInfo::get_passw() {
	return this->password;
}
int UserInfo::get_id_counter() {
	return user_id_counter;
}