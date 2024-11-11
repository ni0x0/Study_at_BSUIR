#include "FilmRatingMark.h"

// конструктор по умолчанию
FilmRatingMark::FilmRatingMark(): FilmMark(){
	ratingMark = -1;
}
// конструктор с параметром - указателем на фильм
FilmRatingMark::FilmRatingMark(Film* _fp) : FilmMark(_fp) {
	ratingMark = -1;
}
// конструктор с параметром - ID фильма
FilmRatingMark::FilmRatingMark(int _f_id) : FilmMark(_f_id) {
	ratingMark = -1;
}
// конструктор с параметрами - указателем на фильм и оценкой
FilmRatingMark::FilmRatingMark(Film* _fp, int _rt): FilmMark(_fp) {
	ratingMark = _rt;
}
// конструктор с параметрами - ID фильма и оценкой
FilmRatingMark::FilmRatingMark(int _f_id, int _rt) : FilmMark(_f_id) {
	ratingMark = _rt;
}

// перегрузка оператора присваивания
FilmRatingMark& FilmRatingMark::operator = (FilmRatingMark& oth) {
	if (this == &oth)
		return *this;

	dynamic_cast<FilmMark&>(*this).operator=(oth);
	this->ratingMark = oth.ratingMark;
	return *this;
}

// перегрузка оператора присваивания
bool FilmRatingMark::operator == (const FilmRatingMark& oth) {
	if (this == &oth)
		return true;

	if (((dynamic_cast<FilmMark&>(*this)).operator==(oth)) == false) return false;
	if (this->ratingMark != oth.ratingMark && oth.ratingMark != -1) return false;
	return true;
}


// перегрузка оператора записи в бинарный файл
fstream& operator << (fstream& out, FilmRatingMark& oth) {
	out << dynamic_cast<FilmMark&>(oth);
	out.write(reinterpret_cast<char*>(&oth.ratingMark), sizeof(oth.ratingMark));

	return out;
}

// перегрузка оператора считывания из бинарного файла
fstream& operator >> (fstream& in, FilmRatingMark& oth) {
	in >> dynamic_cast<FilmMark&>(oth);
	in.read(reinterpret_cast<char*>(&oth.ratingMark), sizeof(oth.ratingMark));

	return in;
}

void FilmRatingMark::set_ratingM(int _rm) {
	ratingMark = _rm;
}
int FilmRatingMark::get_ratingM() {
	return ratingMark;
}
