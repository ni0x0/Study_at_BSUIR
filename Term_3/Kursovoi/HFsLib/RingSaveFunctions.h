#ifndef RING_SAVE_FUNCTIONS_H
#define RING_SAVE_FUNCTIONS_H

#include "RingTemplate.cpp"
#include "FileExceptions.h"

template <class T>
class RingSaveFunctions
{
public:
	// записать информацию из файла в кольцо
	static void get_info_from_bin_file(const char* _fName, Ring<T>* _ring);

	// записать информацию из кольца в файл
	static void save_info_in_bin_file(const char* _fName, Ring<T>* _ring);
};

#endif