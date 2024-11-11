#include "ContainerExceptions.h"

// конструктор с кодом и сообщением по умолчанию
ContainerException::ContainerException(int _code) : MyException(_code, "") {
	switch (_code) {
	case(0):
		set_exp_msg("CONTAINER IS EMPTY");
		break;
	case(1):
		set_exp_msg("ITERATOR IS EMPTY");
		break;
	case(2):
		set_exp_msg("CONTAINER SEARCH ERROR");
		break;
	default:
		set_exp_msg("UNKNOWN ERROR");
	}
}

// конструктор с кодом и заданным сообщением
ContainerException::ContainerException(int _code, const char* _msg) : MyException(_code, _msg) {}