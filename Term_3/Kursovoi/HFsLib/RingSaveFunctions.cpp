#include "RingSaveFunctions.h"

// записать информацию из файла в кольцо
template <class T>
void RingSaveFunctions<T>::get_info_from_bin_file(const char* _fName, Ring<T>* _ring) {
	T emptyInfo;

	fstream ff;
	ff.open(_fName, ios_base::binary | ios_base::in);
	if (ff.good() == false)
		throw FileException(1, "FILES OPENING ERROR\nSOME DATA LOST");
	if (ff.is_open()) {
		while (1) {
			T tempInfo;
			ff >> tempInfo;

			if (emptyInfo == tempInfo)
				break;
			(*_ring).add(tempInfo);
		}
	}
	else
		throw FileException(0, "FILES ARE CORRUPTED\nPROGRAM CANNOT WORK IN RIGHT WAY");
	ff.close();
}

// записать информацию из кольца в файл
template <class T>
void RingSaveFunctions<T>::save_info_in_bin_file(const char* _fName, Ring<T>* _ring) {
	fstream ff;
	ff.open(_fName, ios_base::binary | ios_base::out);
	if ((*_ring).get_nodesNumber()) {
		class Ring<T>::Iterator info_it = (*_ring).begin();

		do {
			ff << (*info_it);
			info_it++;
		} while (info_it != (*_ring).begin());
	}
	ff.close();
}