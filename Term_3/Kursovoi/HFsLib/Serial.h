#ifndef SERIAL_H
#define SERIAL_H
#include "Film.h"

// класс с информацией о сериале
class Serial : public Film
{
protected:
	int seasonNum;           // номер сезона
	int episodesAmount;      // количество серий
	int episodeLenght;       // длина серии

	static int serial_id_counter;  // статический ID счетчик сериалов
public:
	Serial();                      // конструкторы
	Serial(const char* _name, int _prod_year, const char* _country, genres _genre,
		int _season, int _ep_amount, int _ep_lenght);
	Serial(const Serial& oth);
	virtual ~Serial() {}

	Serial& operator = (Serial& oth);                        // перегрузка оператора присваивания
	bool operator == (const Serial& oth);                    // перегрузка оператора "равно"
	bool operator <= (const Serial& oth);                    // перегрузка оператора  "меньше равно"
	bool operator >= (const Serial& oth);                    // перегрузка оператора  "больше равно"

	friend fstream& operator << (fstream& out, Serial& oth);    // перегрузка оператора записи в бинарный файл
	friend fstream& operator >> (fstream& in, Serial& oth);     // перегрузка оператора считывания из бинарного файла

	virtual void copy_data(Serial& oth); // скопировать все данные другого фильма, кроме рейтинга и ID

	void set_seasonNum(int);             // методы для изменения значений в полях
	void set_episodesAmount(int);
	void set_episodeLenght(int);

	int get_seasonNum();                 // методы для получения значений из полей
	int get_episodesAmount();
	int get_episodeLenght();

	static void set_ser_id_c(int);       // статические методы, изменяющие значение счетчика ID
	static int get_ser_id_c();
};

#endif






















































//
//struct SerialChronoInfo {     // информацие о 
//
//
//
//};

