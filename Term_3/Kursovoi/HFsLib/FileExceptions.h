#ifndef FILE_EXCEPTIONS_H
#define FILE_EXCEPTIONS_H
#include "MyExceptions.h"

// ����� ����������, ��������� ��� ������ � ������
class FileException : public MyException {
public:
	FileException(int _code);                     // ����������� � ����� � ���������� �� ���������
	FileException(int _code, const char* _msg);   // ����������� � ����� � �������� ����������
};

#endif