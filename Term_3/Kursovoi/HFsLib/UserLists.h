#ifndef USER_LISTS_H
#define USER_LISTS_H

#include "FilmRatingMark.h"
#include "FilmStopPointMark.h"

#include "RingSaveFunctions.cpp"

// класс, содержащий пользовательские заметки о фильмах
class UserLists
{
public:                                      // списки заметок: 
	Ring<FilmStopPointMark> in_process;      //   - в процессе
	Ring<FilmMark> in_plans;                 //   - в планах
	Ring<FilmMark> watched;                  //   - просмотрено
	Ring<FilmMark> favours;                  //   - любимые
	Ring<FilmStopPointMark> abandoned;       //   - брошено
	Ring<FilmRatingMark> rated;              //   - дана оценка
};

#endif
