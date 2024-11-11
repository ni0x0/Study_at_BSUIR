#ifndef CHECK_INPUT_H
#define CHECK_INPUT_H
#include <iostream>

class CheckInputFunctions   // класс, содержащий статические методы для проверки символьных массивов на удоволетворение заданному условию
{   public:
	static bool check_on_eng_letters_n_digits(const char*);   // проверка на содержание в строке только английских букв и цифр
	static bool check_on_digits(const char* str);             // проверка на содержание в строке только цифр
	static bool check_on_eng_letters_spaces(const char* str); // проверка на содержание в строке только английских букв и пробелов
	static bool check_on_title(const char*);                  // проверка на соответствие названию (допускаются англ. буквы, пробелы, цифры b символы:-\:,)
};

#endif

