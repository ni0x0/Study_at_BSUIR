#include "FileExceptions.h"

// конструктор с кодом и сообщением по умолчанию
FileException::FileException(int _code) : MyException(_code, "") {
	switch (_code) {
	case(0):
		set_exp_msg("FILE OPENING PROBLEM");
		break;
	case(1):
		set_exp_msg("FILE IS CORRUPTED");
		break;
	default:
		set_exp_msg("UNKNOWN ERROR");
	}
}

// конструктор с кодом и заданным сообщением
FileException::FileException(int _code, const char* _msg) : MyException(_code, _msg) {}
