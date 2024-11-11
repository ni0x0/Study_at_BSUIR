#include "MyExceptions.h"

// конструктор класса MyException 
MyException::MyException(int _code, const char* _msg) {
	this->code = _code;
	strcpy_s(this->message, MSG_LEN, _msg);
};

// получить код ошибки
int MyException::get_exp_code() {
	return this->code;
}

// получить сообщение об ошибке
char* MyException::get_exp_msg() {
	return message;
}

// записать сообщение об ошибке
void MyException::set_exp_msg(const char* _msg) {
	strcpy_s(this->message, MSG_LEN, _msg);
}