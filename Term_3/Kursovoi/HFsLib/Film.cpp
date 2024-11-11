#include "Film.h"

// конструктор класса Film по умолчанию
Film::Film() {
	name[0] = '\0';
	ID = -1;
	productionYear = 0;
	productionCountry[0] = '\0';
	genre = genre_not_selected;
	rating = -1;
}

// конструктор класса Film c параметрами
Film::Film(const char* _name, int _prod_year, const char* _country, genres _genre) {
	ID = -1;
	strcpy_s(name, FILM_NAME_LEN, _name);
	productionYear = _prod_year;
	strcpy_s(productionCountry, COUNTRY_LEN, _country);
	genre = _genre;
	rating = -1;
}

// конструктор копирования
Film::Film(const Film& oth) {
	strcpy_s(this->name, FILM_NAME_LEN, oth.name);
	this->ID = oth.ID;
	this->productionYear = oth.productionYear;
	strcpy_s(this->productionCountry, COUNTRY_LEN, oth.productionCountry);
	this->genre = oth.genre;
	this->rating = oth.rating;
}

// перегрузка оператора присваивания
Film& Film::operator = (const Film& oth) {
	if (this == &oth)
		return *this;

	strcpy_s(this->name, FILM_NAME_LEN, oth.name);
	this->ID = oth.ID;
	this->productionYear = oth.productionYear;
	strcpy_s(this->productionCountry, COUNTRY_LEN, oth.productionCountry);
	this->genre = oth.genre;
	this->rating = oth.rating;

	return *this;
}

// перегрузка оператора "равно"
bool Film::operator == (const Film& oth) {
	if (this == &oth)
		return true;

	if (strcmp(this->name, oth.name) && oth.name[0] != '\0') return false;
	if (this->ID != oth.ID && oth.ID != -1) return false;
	if (this->productionYear != oth.productionYear && oth.productionYear != 0) return false;
	if (strcmp(this->productionCountry, oth.productionCountry) && oth.productionCountry[0] != '\0') return false;
	if (this->genre != oth.genre && oth.genre != genre_not_selected) return false;
	if (this->rating != oth.rating && oth.rating != -1) return false;
	return true;
}

// перегрузка оператора  "меньше равно"
bool Film::operator <= (const Film& oth) {
	if (this == &oth)
		return true;

	string s_to_check = this->name;
	if (s_to_check.find(oth.name) == string::npos && oth.name[0] != '\0') return false;
	if (this->ID > oth.ID && oth.ID != -1) return false;
	if (this->productionYear > oth.productionYear && oth.productionYear != 0) return false;
	s_to_check = this->productionCountry;
	if (s_to_check.find(oth.productionCountry) == string::npos && oth.productionCountry[0] != '\0') return false;
	if (this->genre != oth.genre && oth.genre != genre_not_selected) return false;
	if (this->rating > oth.rating && oth.rating != -1) return false;

	return true;
}


// перегрузка оператора  "больше равно"
bool Film::operator >= (const Film& oth) {
	if (this == &oth)
		return true;

	string s_to_check = this->name;
	if (s_to_check.find(oth.name) == string::npos && oth.name[0] != '\0') return false;
	if (this->ID < oth.ID && oth.ID != -1) return false;
	if (this->productionYear < oth.productionYear && oth.productionYear != 0) return false;
	s_to_check = this->productionCountry;
	if (s_to_check.find(oth.productionCountry) == string::npos && oth.productionCountry[0] != '\0') return false;
	if (this->genre != oth.genre && oth.genre != genre_not_selected) return false;
	if (this->rating < oth.rating && oth.rating != -1) return false;

	return true;
}

// перегрузка оператора записи в бинарный файл
fstream& operator << (fstream& out, Film& oth) {
	out.write(reinterpret_cast<char*>(&oth.name), sizeof(oth.name));
	out.write(reinterpret_cast<char*>(&oth.ID), sizeof(oth.ID));
	out.write(reinterpret_cast<char*>(&oth.productionYear), sizeof(oth.productionYear));
	out.write(reinterpret_cast<char*>(&oth.productionCountry), sizeof(oth.productionCountry));
	out.write(reinterpret_cast<char*>(&oth.genre), sizeof(oth.genre));
	out.write(reinterpret_cast<char*>(&oth.rating), sizeof(oth.rating));

	return out;
}

// перегрузка оператора считывания из бинарного файла
fstream& operator >> (fstream& in, Film& oth) {
	in.read(reinterpret_cast<char*>(&oth.name), sizeof(oth.name));
	in.read(reinterpret_cast<char*>(&oth.ID), sizeof(oth.ID));
	in.read(reinterpret_cast<char*>(&oth.productionYear), sizeof(oth.productionYear));
	in.read(reinterpret_cast<char*>(&oth.productionCountry), sizeof(oth.productionCountry));
	in.read(reinterpret_cast<char*>(&oth.genre), sizeof(oth.genre));
	in.read(reinterpret_cast<char*>(&oth.rating), sizeof(oth.rating));

	return in;
}

// добавить оценку к рейтингу фильма
void Film::add_rating_mark(int _mark) {
	if (this->rating != -1)
		this->rating = (this->rating + (float)_mark) / 2;
	else
		this->rating = (float)_mark;
}
// удалить оценку из рейтинга
void Film::delete_rating_mark(int _mark) {
	this->rating = 2 * (this->rating) - _mark;
}
// сбросить рейтинг фильма
void Film::reset_rating_mark() {
	this->rating = -1;
}
// скопировать такие данные другого фильма, как: имя, год, страна, жанр 
void Film::copy_data(Film& oth) {
	strcpy_s(this->name, FILM_NAME_LEN, oth.name);
	this->productionYear = oth.productionYear;
	strcpy_s(this->productionCountry, COUNTRY_LEN, oth.productionCountry);
	this->genre = oth.genre;
}

// методы для изменения полей
void Film::set_name(const char* _name) {
	strcpy_s(this->name, FILM_NAME_LEN, _name);
}
void Film::set_id(int _id) {
	this->ID = _id;
}
void Film::set_prod_year(int _year) {
	this->productionYear = _year;
}
void Film::set_prod_country(const char* _country) {
	strcpy_s(this->productionCountry, COUNTRY_LEN, _country);
}
void Film::set_genre(genres _genre) {
	this->genre = _genre;
}
void Film::set_rating(float _rat) {
	this->rating = _rat;
}

// методы для получения значений из полей
char* Film::get_name() {
	return this->name;
}
int Film::get_id() {
	return this->ID;
}
int Film::get_prod_year() {
	return this->productionYear;
}
char* Film::get_prod_country() {
	return this->productionCountry;
}
genres Film::get_genre() {
	return this->genre;
}
float Film::get_rating() {
	return this->rating;
}









