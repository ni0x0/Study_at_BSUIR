#ifndef MY_EXCEPTIONS
#define MY_EXCEPTIONS

#include<iostream>
using namespace std;
#define MSG_LEN 80                               // длина сообщения об ошибке по стандарту

// класс собственных исключений
class MyException {
	int code;                                    // код ошибки 
	char message[MSG_LEN];                       // описание ошибки
public:
	MyException(int _code, const char* _msg);         // конструктор класса MyException 
	int get_exp_code();                               // получить код ошибки
	char* get_exp_msg();                              // получить сообщение об ошибке
protected:
	void set_exp_msg(const char* _msg);               // записать сообщение об ошибке
};

#endif