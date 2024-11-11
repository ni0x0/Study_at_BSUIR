#ifndef FILE_EXCEPTIONS_H
#define FILE_EXCEPTIONS_H
#include "MyExceptions.h"

// класс исключений, вызванных при работе с файлом
class FileException : public MyException {
public:
	FileException(int _code);                     // конструктор с кодом и сообщением по умолчанию
	FileException(int _code, const char* _msg);   // конструктор с кодом и заданным сообщением
};

#endif