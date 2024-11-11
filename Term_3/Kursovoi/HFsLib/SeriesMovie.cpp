#include "SeriesMovie.h"

// установка ID счетчика
int SeriesMovie::seria_movie_id_counter = 2000;

// конструктор класса SeriesMovie по умолчанию
SeriesMovie::SeriesMovie() : Movie() {
	seriaStatus = sm_type_not_selected;
	seriaNum = 0;
}

// конструктор класса SeriesMovie c параметрами
SeriesMovie::SeriesMovie(const char* _name, int _prod_year, const char* _country, genres _genre,
	int _hour, int _minute, SeriesMovietype _type, int _seria) : Movie(_name, _prod_year, _country, _genre, _hour, _minute) {

	seriaStatus = _type;
	seriaNum = _seria;

	ID = seria_movie_id_counter++;
}

// конструктор копирования
SeriesMovie::SeriesMovie(const SeriesMovie& oth) : Movie(oth) {
	seriaStatus = oth.seriaStatus;
	seriaNum = oth.seriaNum;
}

// перегрузка оператора присваивания
SeriesMovie& SeriesMovie::operator = (SeriesMovie& oth) {
	if (this == &oth)
		return *this;

	dynamic_cast<Movie&>(*this).operator=(oth);
	this->seriaStatus = oth.seriaStatus;
	this->seriaNum = oth.seriaNum;

	return *this;
}

// перегрузка оператора присваивания
bool SeriesMovie::operator == (const SeriesMovie& oth) {
	if (this == &oth)
		return true;

	if (((dynamic_cast<Movie&>(*this)).operator==(oth)) == false) return false;
	if (this->seriaStatus != oth.seriaStatus && oth.seriaStatus != sm_type_not_selected) return false;
	if (this->seriaNum != oth.seriaNum && oth.seriaNum != 0) return false;

	return true;
}

// перегрузка оператора  "меньше равно"
bool SeriesMovie::operator <= (const SeriesMovie& oth) {
	if (this == &oth)
		return true;

	if (((dynamic_cast<Movie&>(*this)).operator<=(oth)) == false) return false;
	if (this->seriaStatus != oth.seriaStatus && oth.seriaStatus != sm_type_not_selected) return false;
	if (this->seriaNum > oth.seriaNum && oth.seriaNum != 0) return false;

	return true;
}

// перегрузка оператора  "больше равно"
bool SeriesMovie::operator >= (const SeriesMovie& oth) {
	if (this == &oth)
		return true;

	if (((dynamic_cast<Movie&>(*this)).operator>=(oth)) == false) return false;
	if (this->seriaStatus != oth.seriaStatus && oth.seriaStatus != sm_type_not_selected) return false;
	if (this->seriaNum < oth.seriaNum && oth.seriaNum != 0) return false;

	return true;
}

// перегрузка оператора записи в бинарный файл
fstream& operator << (fstream& out, SeriesMovie& oth) {
	out << dynamic_cast<Movie&>(oth);

	out.write(reinterpret_cast<char*>(&oth.seriaStatus), sizeof(oth.seriaStatus));
	out.write(reinterpret_cast<char*>(&oth.seriaNum), sizeof(oth.seriaNum));

	return out;
}

// перегрузка оператора считывания из бинарного файла
fstream& operator >> (fstream& in, SeriesMovie& oth) {
	in >> dynamic_cast<Movie&>(oth);

	in.read(reinterpret_cast<char*>(&oth.seriaStatus), sizeof(oth.seriaStatus));
	in.read(reinterpret_cast<char*>(&oth.seriaNum), sizeof(oth.seriaNum));

	return in;
}

// статические методы, изменяющие значение счетчика ID
void SeriesMovie::copy_data(SeriesMovie& oth) {
	dynamic_cast<Movie&>(*this).copy_data(oth);
	this->seriaStatus = oth.seriaStatus;
	this->seriaNum = oth.seriaNum;
}

// методы для изменения значений в полях
void SeriesMovie::set_seria_type(SeriesMovietype _type) {
	this->seriaStatus = _type;
}
void SeriesMovie::set_seria_num(int _ser_num) {
	this->seriaNum = _ser_num;
}

// методы для получения значений из полей
SeriesMovietype SeriesMovie::get_seria_type() {
	return this->seriaStatus;
}
int SeriesMovie::get_seria_num() {
	return this->seriaNum;
}

// статические методы, изменяющие значение счетчика ID
void SeriesMovie::set_seria_movie_id_c(int _pointer) {
	seria_movie_id_counter = _pointer;
}
int SeriesMovie::set_seria_movie_id_c() {
	return seria_movie_id_counter;
}
