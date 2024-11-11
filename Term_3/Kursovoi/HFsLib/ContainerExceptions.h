#ifndef CONTAINER_EXCEPTIONS_H
#define CONTAINER_EXCEPTIONS_H

#include "MyExceptions.h"

// ����� ����������, ��������� ��� ������ � �����������
class ContainerException : public MyException {
public:
	ContainerException(int _code);                     // ����������� � ����� � ���������� �� ���������
	ContainerException(int _code, const char* _msg);   // ����������� � ����� � �������� ����������
};

#endif