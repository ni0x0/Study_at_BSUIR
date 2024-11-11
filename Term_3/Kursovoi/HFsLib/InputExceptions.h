#ifndef INPUT_EXCP_H
#define INPUT_EXCP_H

#include "MyExceptions.h"

// класс исключений, вызванных при вводе
class InputException : public MyException {
public:
	InputException(int _code);                     // конструктор с кодом и сообщением по умолчанию
	InputException(int _code, const char* _msg);   // конструктор с кодом и заданным сообщением
};

#endif