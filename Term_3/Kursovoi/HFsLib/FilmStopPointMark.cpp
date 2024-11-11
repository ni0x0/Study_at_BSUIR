#include "FilmStopPointMark.h"

// конструктор по умочанию
FilmStopPointMark::FilmStopPointMark() : FilmMark() {
	stop_point.in_movie.hour = -1;
	stop_point.in_movie.minute = -1;
}
// конструктор с параметром - указателем на фильм
FilmStopPointMark::FilmStopPointMark(Film* _fp) : FilmMark(_fp) {
	stop_point.in_movie.hour = -1;
	stop_point.in_movie.minute = -1;
}
// конструктор с параметром - ID фильма
FilmStopPointMark::FilmStopPointMark(int _f_id) : FilmMark(_f_id) {
	stop_point.in_movie.hour = -1;
	stop_point.in_movie.minute = -1;
}
// конструктор с параметрами - указателем на фильм, точкой остановки в просмотре - эпизод
FilmStopPointMark::FilmStopPointMark(Film* _fp, int _sEp) : FilmMark(_fp) {
	stop_point.in_serial.episode = _sEp;
	stop_point.in_movie.minute = -1;
}
// конструктор с параметрами - указателем на фильм, точкой остановки в просмотре - час и минута
FilmStopPointMark::FilmStopPointMark(Film* _fp, int _sHour, int _sMin) : FilmMark(_fp) {
	stop_point.in_movie.hour = _sHour;
	stop_point.in_movie.minute = _sMin;
}
// конструктор с параметрами - ID фильма, точкой остановки в просмотре - эпизод
FilmStopPointMark::FilmStopPointMark(int _f_id, int _sEp) : FilmMark(_f_id) {
	stop_point.in_serial.episode = _sEp;
	stop_point.in_movie.minute = -1;
}
// конструктор с параметрами - ID фильма, точкой остановки в просмотре - час и минута
FilmStopPointMark::FilmStopPointMark(int _f_id, int _sHour, int _sMin) : FilmMark(_f_id) {
	stop_point.in_movie.hour = _sHour;
	stop_point.in_movie.minute = _sMin;
}

// перегрузка оператора присваивания
FilmStopPointMark& FilmStopPointMark::operator = (FilmStopPointMark& oth) {
	if (this == &oth)
		return *this;

	dynamic_cast<FilmMark&>(*this).operator=(oth);
	this->stop_point.in_movie.hour = oth.stop_point.in_movie.hour;
	this->stop_point.in_movie.minute = oth.stop_point.in_movie.minute;

	return *this;
}

// перегрузка оператора присваивания
bool FilmStopPointMark::operator == (const FilmStopPointMark& oth) {
	if (this == &oth)
		return true;

	if (((dynamic_cast<FilmMark&>(*this)).operator==(oth)) == false) return false;
	if (this->stop_point.in_movie.hour != oth.stop_point.in_movie.hour && oth.stop_point.in_movie.hour != -1) return false;
	if (this->stop_point.in_movie.minute != oth.stop_point.in_movie.minute && oth.stop_point.in_movie.minute != -1) return false;

	return true;
}

// перегрузка оператора записи в бинарный файл
fstream& operator << (fstream& out, FilmStopPointMark& oth) {
	out << dynamic_cast<FilmMark&>(oth);

	out.write(reinterpret_cast<char*>(&oth.stop_point.in_movie.hour), sizeof(oth.stop_point.in_movie.hour));
	out.write(reinterpret_cast<char*>(&oth.stop_point.in_movie.minute), sizeof(oth.stop_point.in_movie.minute));

	return out;
}

// перегрузка оператора считывания из бинарного файла
fstream& operator >> (fstream& in, FilmStopPointMark& oth) {
	in >> dynamic_cast<FilmMark&>(oth);

	in.read(reinterpret_cast<char*>(&oth.stop_point.in_movie.hour), sizeof(oth.stop_point.in_movie.hour));
	in.read(reinterpret_cast<char*>(&oth.stop_point.in_movie.minute), sizeof(oth.stop_point.in_movie.minute));

	return in;
}

// методы для изменения значений в полях
void FilmStopPointMark::set_episodeM(int _sEp) {
	stop_point.in_serial.episode = _sEp;
}
int FilmStopPointMark::get_episodeM() {
	return stop_point.in_serial.episode;
}
void FilmStopPointMark::set_hourM(int _sHour) {
	stop_point.in_movie.hour = _sHour;
}
// методы для ролучения значений в полях
int FilmStopPointMark::get_hourM() {
	return stop_point.in_movie.hour;
}
void FilmStopPointMark::set_minuteM(int _sMin) {
	stop_point.in_movie.minute = _sMin;
}
int FilmStopPointMark::get_minuteM() {
	return stop_point.in_movie.minute;
}
