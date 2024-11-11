#ifndef CONTAINER_EXCEPTIONS_H
#define CONTAINER_EXCEPTIONS_H

#include "MyExceptions.h"

// класс исключений, вызванных при работе с контейнером
class ContainerException : public MyException {
public:
	ContainerException(int _code);                     // конструктор с кодом и сообщением по умолчанию
	ContainerException(int _code, const char* _msg);   // конструктор с кодом и заданным сообщением
};

#endif