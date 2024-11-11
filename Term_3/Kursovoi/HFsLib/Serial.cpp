#include "Serial.h"

// установка ID счетчика
int Serial::serial_id_counter = 1000;

// конструктор класса Serial по умолчанию
Serial::Serial() : Film() {
	seasonNum = 0;
	episodesAmount = 0;
	episodeLenght = 0;
}

// конструктор класса Serial c параметрами
Serial::Serial(const char* _name, int _prod_year, const char* _country, genres _genre,
	int _season, int _ep_amount, int _ep_lenght) : Film(_name, _prod_year, _country, _genre) {

	seasonNum = _season;
	episodesAmount = _ep_amount;
	episodeLenght = _ep_lenght;

	ID = serial_id_counter++;
}

// конструктор копирования
Serial::Serial(const Serial& oth) : Film(oth) {
	seasonNum = oth.seasonNum;
	episodesAmount = oth.episodesAmount;
	episodeLenght = oth.episodeLenght;
}

// перегрузка оператора присваивания
Serial& Serial::operator = (Serial& oth) {
	if (this == &oth)
		return *this;

	dynamic_cast<Film&>(*this).operator=(oth);
	this->seasonNum = oth.seasonNum;
	this->episodesAmount = oth.episodesAmount;
	this->episodeLenght = oth.episodeLenght;

	return *this;
}

// перегрузка оператора присваивания
bool Serial::operator == (const Serial& oth) {
	if (this == &oth)
		return true;

	if (((dynamic_cast<Film&>(*this)).operator==(oth)) == false) return false;
	if (this->seasonNum != oth.seasonNum && oth.seasonNum != 0) return false;
	if (this->episodesAmount != oth.episodesAmount && oth.episodesAmount != 0) return false;
	if (this->episodeLenght != oth.episodeLenght && oth.episodeLenght != 0) return false;
	return true;
}

// перегрузка оператора  "меньше равно"
bool Serial::operator <= (const Serial& oth) {
	if (this == &oth)
		return true;

	if (((dynamic_cast<Film&>(*this)).operator<=(oth)) == false) return false;
	if (this->seasonNum > oth.seasonNum && oth.seasonNum != 0) return false;
	if (this->episodesAmount > oth.episodesAmount && oth.episodesAmount != 0) return false;
	if (this->episodeLenght > oth.episodeLenght && oth.episodeLenght != 0) return false;

	return true;
}


// перегрузка оператора  "больше равно"
bool Serial::operator >= (const Serial& oth) {
	if (this == &oth)
		return true;

	if (((dynamic_cast<Film&>(*this)).operator>=(oth)) == false) return false;
	if (this->seasonNum < oth.seasonNum && oth.seasonNum != 0) return false;
	if (this->episodesAmount < oth.episodesAmount && oth.episodesAmount != 0) return false;
	if (this->episodeLenght < oth.episodeLenght && oth.episodeLenght != 0) return false;

	return true;
}

// перегрузка оператора записи в бинарный файл
fstream& operator << (fstream& out, Serial& oth) {
	out << dynamic_cast<Film&>(oth);

	out.write(reinterpret_cast<char*>(&oth.seasonNum), sizeof(oth.seasonNum));
	out.write(reinterpret_cast<char*>(&oth.episodesAmount), sizeof(oth.episodesAmount));
	out.write(reinterpret_cast<char*>(&oth.episodeLenght), sizeof(oth.episodeLenght));

	return out;
}

// перегрузка оператора считывания из бинарного файла
fstream& operator >> (fstream& in, Serial& oth) {
	in >> dynamic_cast<Film&>(oth);

	in.read(reinterpret_cast<char*>(&oth.seasonNum), sizeof(oth.seasonNum));
	in.read(reinterpret_cast<char*>(&oth.episodesAmount), sizeof(oth.episodesAmount));
	in.read(reinterpret_cast<char*>(&oth.episodeLenght), sizeof(oth.episodeLenght));

	return in;
}

// скопировать все данные другого фильма, кроме рейтинга и ID
void Serial::copy_data(Serial& oth) {
	dynamic_cast<Film&>(*this).copy_data(oth);

	this->seasonNum = oth.seasonNum;
	this->episodesAmount = oth.episodesAmount;
	this->episodeLenght = oth.episodeLenght;
}

// методы для изменения значений в полях
void Serial::set_seasonNum(int _season_num) {
	this->seasonNum = _season_num;
}
void Serial::set_episodesAmount(int _ep_amount) {
	this->episodesAmount = _ep_amount;
}
void Serial::set_episodeLenght(int _ep_len) {
	this->episodeLenght = _ep_len;
}

// методы для получения значений из полей
int Serial::get_seasonNum() {
	return this->seasonNum;
}
int Serial::get_episodesAmount() {
	return this->episodesAmount;
}
int Serial::get_episodeLenght() {
	return this->episodeLenght;
}

// статические методы, изменяющие значение счетчика ID
void Serial::set_ser_id_c(int _pointer) {
	serial_id_counter = _pointer;
}
int Serial::get_ser_id_c() {
	return serial_id_counter;
}
