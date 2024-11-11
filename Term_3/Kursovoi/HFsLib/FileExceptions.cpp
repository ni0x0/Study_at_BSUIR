#include "FileExceptions.h"

// ����������� � ����� � ���������� �� ���������
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

// ����������� � ����� � �������� ����������
FileException::FileException(int _code, const char* _msg) : MyException(_code, _msg) {}
