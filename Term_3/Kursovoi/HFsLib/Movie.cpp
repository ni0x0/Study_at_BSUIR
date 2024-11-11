#include "Movie.h"

// установка ID счетчика
int Movie::movie_id_counter = 0;

// конструктор класса Movie по умолчанию
Movie::Movie() : Film() {
	hour = -1;
	minute = -1;
}

// конструктор класса Movie c параметрами
Movie::Movie(const char* _name, int _prod_year, const char* _country, genres _genre,
	int _hour, int _minute) : Film(_name, _prod_year, _country, _genre) {

	hour = _hour;
	minute = _minute;

	ID = movie_id_counter++;
}

// конструктор копирования
Movie::Movie(const Movie& oth) : Film(oth) {
	hour = oth.hour;
	minute = oth.minute;
}

// перегрузка оператора присваивания
Movie& Movie::operator = (Movie& oth) {
	if (this == &oth)
		return *this;

	dynamic_cast<Film&>(*this).operator=(oth);
	this->hour = oth.hour;
	this->minute = oth.minute;

	return *this;
}

// перегрузка оператора присваивания
bool Movie::operator == (const Movie& oth) {
	if (this == &oth)
		return true;

	if (((dynamic_cast<Film&>(*this)).operator==(oth)) == false) return false;
	if (this->hour != oth.hour && oth.hour != -1) return false;
	if (this->minute != oth.minute && oth.minute != -1) return false;

	return true;
}

// перегрузка оператора  "меньше равно"
bool Movie::operator <= (const Movie& oth) {
	if (this == &oth)
		return true;

	if (((dynamic_cast<Film&>(*this)).operator<=(oth)) == false) return false;
	if (this->hour > oth.hour && oth.hour != -1) return false;
	if (this->minute > oth.minute && oth.minute != -1 && this->hour >= oth.hour) return false;
	                  
	return true;
}

// перегрузка оператора  "больше равно"
bool Movie::operator >= (const Movie& oth) {
	if (this == &oth)
		return true;

	if (((dynamic_cast<Film&>(*this)).operator>=(oth)) == false) return false;
	if (this->hour < oth.hour && oth.hour != -1) return false;
	if (this->minute < oth.minute && oth.minute != -1) return false;  

	return true;
}

// перегрузка оператора записи в бинарный файл
fstream& operator << (fstream& out, Movie& oth) {
	out << dynamic_cast<Film&>(oth);

	out.write(reinterpret_cast<char*>(&oth.hour), sizeof(oth.hour));
	out.write(reinterpret_cast<char*>(&oth.minute), sizeof(oth.minute));

	return out;
}

// перегрузка оператора считывания из бинарного файла
fstream& operator >> (fstream& in, Movie& oth) {
	in >> dynamic_cast<Film&>(oth);

	in.read(reinterpret_cast<char*>(&oth.hour), sizeof(oth.hour));
	in.read(reinterpret_cast<char*>(&oth.minute), sizeof(oth.minute));

	return in;
}

// скопировать все данные другого фильма, кроме рейтинга и ID
void Movie::copy_data(Movie& oth) {
	dynamic_cast<Film&>(*this).copy_data(oth);
	this->hour = oth.hour;
	this->minute = oth.minute;
}

// методы для изменения значений в полях
void Movie::set_hour(int _hour) {
	this->hour = _hour;
}
void Movie::set_minute(int _minute) {
	this->minute = _minute;
}

// методы для получения значений из полей
int Movie::get_hour() {
	return this->hour;
}
int Movie::get_minute() {
	return this->minute;
}

// статические методы, изменяющие значение счетчика ID
void Movie::set_movie_id_c(int _pointer) {
	movie_id_counter = _pointer;
}
int Movie::get_movie_id_c() {
	return movie_id_counter;
}
