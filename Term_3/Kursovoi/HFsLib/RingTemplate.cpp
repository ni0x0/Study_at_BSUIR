#include "RingTemplate.h"

// конструктор Ring по умолчанию
template <class T>
Ring<T>::Ring() {
	head = NULL;                                      // устанавливаем параметры по умолчанию
	nodesNumber = 0;
}

// конструктор копирования
template <class T>
Ring<T>::Ring(const Ring<T>& optRn) {
	this->head = NULL;                               // параметры по умолчанию
	this->nodesNumber = 0;

	if (optRn.head == NULL) return;                  // конец копирования если переданное кольцо пустое
	RingNode<T>* currentNode = optRn.head;           // начиная с головы
	do {                                             // копируем содержимое
		this->add(currentNode->nodeData);
		currentNode = currentNode->next;
	} while (currentNode != optRn.head);
}

// деструктор Ring
template <class T>
Ring<T>::~Ring() {
	free_ring();                                     // очищаем память кольца
}

// перегрузка оператора присваивания
template <class T>
Ring<T>& Ring<T>:: operator = (const Ring<T>& other) {
	this->free_ring();                              // очищаем память кольца
	if (other.head == NULL) return *this;           // конец присваивания если переданное кольцо пустое

	RingNode<T>* currentNode = other.head;          // начиная с головы
	do {
		this->add(currentNode->nodeData);           // поэлементно копируем содержимое
		currentNode = currentNode->next;
	} while (currentNode != other.head);

	return *this;
}

// добавить элемент в кольцо
template <class T>
void Ring<T>::add(T& node_to_add) {
	RingNode<T>* newNode = new RingNode<T>;                     // указатель на новозданный элемент
	newNode->nodeData = node_to_add;                            // копирование значений

	if (head == NULL) {                                  // если кольцо еще не создано (т.е. не содержит ни одного элемента) 
		newNode->next = newNode;                         // "замыкаем" единственный элемент на себя 
		newNode->prev = newNode;
		head = newNode;                                  // указатель на кольцо пока с единственным эл-том
	}
	else {                                               // иначе, добавляем новый элемент за голову (слева если рассматривать почасовой обход)
		newNode->next = head;                            // следующим после нового элемента становится голова
		newNode->prev = head->prev;                      // а предыдущим то, что стояло перед головой
		head->prev->next = newNode;                      // следующим после того теперь будет новый элемент
		head->prev = newNode;                            // а тот будет предыдущим перед головой
	}
	nodesNumber++;
}

// удалить все элементы, совпадающие по заданному значению
template <class T>
int Ring<T>::delete_all_match_elems(T node_to_del) {
	int deleted_elems_num = 0;                                  // количество удаленных элементов
	if (head == NULL) return 0;

	RingNode<T>* currentNode = head;                            // ук-ль для прохода по кольцу                       
	RingNode<T>* removed_node;                                  // указатель на удаляемый элемент

	do {                          // цикл удаления элементов
		if (head->next == head && head->nodeData == node_to_del) {    // если в кольце 1 эл-т и он искомый
			delete head;                                              // удаляем
			head = NULL;
			nodesNumber--;
			deleted_elems_num++;
			break;
		}                                                         // иначе:            
		if (currentNode->nodeData == node_to_del) {               // если текущий элемент надо удалить
			currentNode->prev->next = currentNode->next;          // производим замену указателей
			currentNode->next->prev = currentNode->prev;

			removed_node = currentNode;

			nodesNumber--;                                        // уменьшаем счетчик количества элементов в кольце 
			deleted_elems_num++;                                  // увеличиваем счетчик удаленных элементов 

			if (currentNode == head) {                            // если искомым оказалась голова
				head = head->next;                                // новой головой становится cледующий элемент
				currentNode = head;

				delete removed_node;
				continue;
			}
			else {
				currentNode = currentNode->prev;
				delete removed_node;
			}
		}

		currentNode = currentNode->next;                           // продвижение по кольцу                   
		if (currentNode == head) break;                            // заканчиваем цикл если вернулись к голове
	} while (1);
	return deleted_elems_num;                                      //возвращаем количество удаленных элементов
}

// получить кольцо со всеми элементами, совпадающими по заданному значению
template <class T>
Ring<T> Ring<T>::find_all_match_elems(T filter_node) {
	Ring<T> neededDataRing;
	if (head == NULL)
		return neededDataRing;
	else {
		RingNode<T>* currentNode = head;                      // ук-ль для прохода по кольцу
		do {
			if (currentNode->nodeData == filter_node)
				neededDataRing.add(currentNode->nodeData);
			currentNode = currentNode->next;                  // ук-ль указывает на след. элемент в кольце
		} while (currentNode != head);                        // цикл идет до тех пор, пока не мы вернулись к голове
	}
	return neededDataRing;
}

// получить cсылку на nodeData первого (справа или слева от головы) элемента, совпадающего по заданному значению
template <class T>
T* Ring<T>::get_access_to_elem_data(T filter_node, bool dir) {
	T* pointer_to_return = NULL;
	if (head == NULL)
		return NULL;
	else {
		RingNode<T>* currentNode;
		if (dir) currentNode = head;                      // ук-ль для прохода по кольцу
		else currentNode = head->prev;
		do {
			if (currentNode->nodeData == filter_node) {
				pointer_to_return = &(currentNode->nodeData);
				return pointer_to_return;
			}
			if (dir) currentNode = currentNode->next;                                    // ук-ль указывает на след. элемент в кольце
			else currentNode = currentNode->prev;
		} while ((dir && currentNode != head) || (!dir && currentNode != head->prev));   // цикл идет до тех пор, пока не мы вернулись к голове
	}
	return pointer_to_return;
}

// очистить содержимое кольца
template <class T>
void Ring<T>::free_ring() {
	if (head == NULL) return;

	if (head == head->next) {                                 // если в кольце 1 эл-т
		delete head; head = NULL;                             // очищаем память и возвращаем нулевое значение
		nodesNumber = 0;
		return;
	}
	else {
		RingNode<T>* pointer1 = head->next;                   // ук-ль для прохода по кольцу
		RingNode<T>* pointer2 = pointer1;                     // ук-ль на удаляемый эл-т кольца
		do {
			pointer2 = pointer1;                      // запоминаем текущий рассматриваемый эл-т кольца
			pointer1 = pointer1->next;                // запоминаем следующий
			delete pointer2;                          // удаляем текущий
		} while (pointer1 != head);                   // цикл идет до тех пор, пока мы не вернулись в голове кольца
	}
	delete head; head = NULL;                                 // очищаем память и возвращаем нулевое значение
	nodesNumber = 0;
}

// получить количество элементов в кольце
template <class T>
int Ring<T>::get_nodesNumber() {
	return nodesNumber;
}

///////////////////// методы связанные с итератором/////////////////////////////////////////////////

// конструктор по умолчанию
template<class T>
Ring<T>::Iterator::Iterator() {
	curr = NULL;
}

// конструктор с кольцом
template<class T>
Ring<T>::Iterator::Iterator(Ring<T>& _ring) {
	curr = _ring.head;
}

// конструктор с элементом кольца
template<class T>
Ring<T>::Iterator::Iterator(RingNode<T>* _ring_node) {
	curr = _ring_node;
}

// конструктор копирования
template<class T>
Ring<T>::Iterator::Iterator(const Iterator& it) {
	this->curr = it.curr;
}

 //перегрузка оператора присваивания
template<class T>
class Ring<T>::Iterator& Ring<T>::Iterator::operator=(Iterator it) {
	this->curr = it.curr;

	return *this;
}

// перегрузка оператора ++
template<class T>
class Ring<T>::Iterator& Ring<T>::Iterator::operator++(int) {
	this->curr = curr->next;

	return *this;
}

// перегрузка оператора --
template<class T>
class Ring<T>::Iterator& Ring<T>::Iterator::operator--(int) {
	this->curr = curr->prev;

	return *this;
}

// перегрузка операторов сравнения
template<class T>
bool Ring<T>::Iterator::operator==(const Iterator& it) {
	return (this->curr == it.curr);
}
template<class T>
bool Ring<T>::Iterator::operator!=(const Iterator& it) {
	return (this->curr != it.curr);
}
template<class T>
bool Ring<T>::Iterator::check() {
	return this->curr;
}

// получить доступ к текущему элементу кольца
template<class T>
T& Ring<T>::Iterator::operator*() {
	return curr->nodeData;
}

// получить итератор, ссылающийся на голову кольца
template<class T>
class Ring<T>::Iterator Ring<T>::begin() {
	return Iterator(head);
}

// получить итератор, ссылающийся на элемент за головой кольца
template<class T>
class Ring<T>::Iterator Ring<T>::end() {
	return Iterator(head->prev);
}
