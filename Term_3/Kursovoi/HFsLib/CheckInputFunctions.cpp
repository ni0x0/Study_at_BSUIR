#include "CheckInputFunctions.h"

// проверка на содержание в строке только английских букв и цифр
bool CheckInputFunctions::check_on_eng_letters_n_digits(const char* str) {
	int i = 0;
	while (str[i] != '\0') {
		if (!(str[i] >= 'A' && str[i] <= 'Z') && !(str[i] >= 'a' && str[i] <= 'z')
			&& !(str[i] >= '0' && str[i] <= '9'))
			return false;
		i++;
	}
	return true;
}

// проверка на содержание в строке только цифр
bool CheckInputFunctions::check_on_digits(const char* str) {
	int i = 0;
	while (str[i] != '\0') {
		if (!(str[i] >= '0' && str[i] <= '9'))
			return false;
		i++;
	}
	return true;
}

// проверка на содержание в строке только английских букв и пробелов
bool CheckInputFunctions::check_on_eng_letters_spaces(const char* str) {
	int i = 0;
	while (str[i] != '\0') {
		if (!(str[i] >= 'A' && str[i] <= 'Z') && !(str[i] >= 'a' && str[i] <= 'z') && !(str[i] == ' '))
			return false;
		i++;
	}
	return true;
}


bool CheckInputFunctions::check_on_title(const char* str) {
	int i = 0;
	while (str[i] != '\0') {
		if (!(str[i] >= 'A' && str[i] <= 'Z') && !(str[i] >= 'a' && str[i] <= 'z')
			&& !(str[i] >= '0' && str[i] <= '9') && !(str[i] == ' ' && !(str[i] == ','))
			&& !(str[i] == '\'') && !(str[i] == ':') && !(str[i] == '-'))
			return false;
		i++;
	}
	return true;
}