#include "InputExceptions.h"

// конструктор с кодом и сообщением по умолчанию
InputException::InputException(int _code) : MyException(_code, "") {
	switch (_code) {
	case(0):     // код 0 - введена слишком длинная строки
		set_exp_msg("You entered too long string. Enter less one");
		break;
	case(1):     // код 1 - введена пустая строка
		set_exp_msg("Entered string is empty");
		break;
	case(2):     // код 2 - строка содержит недопустимые символы
		set_exp_msg("String has invalid symbols");
		break;
	default:     // ошибка без предусмотренного кода
		set_exp_msg("UNKNOWN ERROR");
	}
}

// конструктор с кодом и заданным сообщением
InputException::InputException(int _code, const char* _msg) : MyException(_code, _msg) {}