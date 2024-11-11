#ifndef RING_TEMPLATE_H
#define RING_TEMPLATE_H

#include <iostream>
using namespace std;

// шаблон класса списка-кольца
template <class T>
class Ring
{
	template <class T>             // шаблон cтруктуры узла кольца
	struct RingNode
	{
		T nodeData = T();
		RingNode<T>* next = NULL;
		RingNode<T>* prev = NULL;
	};

	RingNode<T>* head;             // указатель на голову кольца
	int nodesNumber;               // количество элементов в кольце

public:

	Ring();                                    // конструктор
	Ring(const Ring& optRing);                 // конструктор копирования
	~Ring();                                   // деструктор
	Ring& operator = (const Ring& other);      // присваивание значений другого обьекта

	void add(T&);                                                         // добавить элемент 
	int delete_all_match_elems(T node_to_del);                            // удалить все элементы, совпадающие по заданному значению

	Ring<T> find_all_match_elems(T);                     // получить кольцо со всеми элементами, совпадающими по заданному значению
	T* get_access_to_elem_data(T, bool);                 // получить cсылку на nodeData первого (справа или слева от головы) элемента, совпадающего по заданному значению

	void free_ring();                      // очистить содержимое
	int get_nodesNumber();                 // получить количество элементов в кольце

	// класс итератора для кольца
	class Iterator {
		RingNode<T>* curr;                                 // указатель на рассматриваемый элемент кольца
	public:
		Iterator();                                        // конструктор по умолчанию
		Iterator(Ring<T>& _ring);                          // конструктор с кольцом
		Iterator(RingNode<T>* _ring_node);                 // конструктор с элементом кольца
		Iterator(const Iterator& it);                      // конструктор копирования

		Iterator& operator = (Iterator it);
		Iterator& operator ++ (int);
		Iterator& operator -- (int);

		bool operator== (const Iterator& it);              // перегрузка операторов сравнения
		bool operator!= (const Iterator& it);
		bool check();

		T& operator*();                                    // получить доступ к текущему элементу кольца
	};

	Iterator begin();              // получить итератор, ссылающийся на голову кольца
	Iterator end();                // получить итератор, ссылающийся на элемент за головой кольца

};

#endif 
























































































































//// конструктор Ring
//template <class T>
//Ring<T>::Ring() {
//	head = NULL;
//	nodesNumber = 0;
//}
//
//// конструктор копирования класса 
//template <class T>
//Ring<T>::Ring(const Ring<T>& optRn) {
//	this->head = NULL;
//	this->nodesNumber = 0;
//
//	if (optRn.head == NULL) return;
//	RingNode<T>* currentNode = optRn.head;
//	do {
//		this->add(currentNode->nodeData);
//		currentNode = currentNode->next;
//	} while (currentNode != optRn.head);
//}
//
//// деструктор Ring
//template <class T>
//Ring<T>::~Ring() {
//	free_ring();
//}
//
//// перегрузка оператора присваивания
//template <class T>
//Ring<T>& Ring<T>:: operator = (const Ring<T>& other) {
//	this->free_ring();
//	if (other.head == NULL) return *this;
//
//	RingNode<T>* currentNode = other.head;
//	do {
//		this->add(currentNode->nodeData);
//		currentNode = currentNode->next;
//	} while (currentNode != other.head);
//
//	return *this;
//}
//
//// добавить элемент в кольцо
//template <class T>
//void Ring<T>::add(T& node_to_add) {
//	RingNode<T>* newNode = new RingNode<T>;                     // указатель на новозданный элемент
//	newNode->nodeData = node_to_add;                            // копирование значений
//
//	if (head == NULL) {                                  // если кольцо еще не создано (т.е. не содержит ни одного элемента) 
//		newNode->next = newNode;                         // "замыкаем" единственный элемент на себя 
//		newNode->prev = newNode;
//		head = newNode;                                  // указатель на кольцо пока с единственным эл-том
//	}
//	else {                                               // иначе, добавляем новый элемент за голову (слева если рассматривать почасовой обход)
//		newNode->next = head;                            // следующим после нового элемента становится голова
//		newNode->prev = head->prev;                      // а предыдущим то, что стояло перед головой
//		head->prev->next = newNode;                      // следующим после того теперь будет новый элемент
//		head->prev = newNode;                            // а тот будет предыдущим перед головой
//	}
//	nodesNumber++;
//}
//
//// вывести содержимое кольца
//template <class T>
//void Ring<T>::show_all() {
//	if (head == NULL)
//		cout << endl << "RING IS EMPTY" << endl;
//	else {
//		RingNode<T>* currentNode = head;                // ук-ль для прохода по кольцу
//		do {
//			cout << currentNode->nodeData << endl;
//			currentNode = currentNode->next;            // ук-ль указывает на след. элемент в кольце
//		} while (currentNode != head);                  // цикл идет до тех пор, пока не мы вернулись к эл-ту, следующему за головой  
//	}
//}
//
//// удалить элемент из кольца
//template <class T>
//void Ring<T>::delete_elem(T node_to_del) {
//	if (head == NULL) return;
//	RingNode<T>* currentNode = head;                                  // ук-ль для прохода по кольцу
//
//	bool we_have_new_head = false;                                    // проверка на установку новой головы
//	bool head_is_checked = true;                                      // проверка нп то, проверена ли голова
//
//	do {
//		if (head->next == head && head->nodeData == node_to_del) {    // если в кольце 1 эл-т и он искомый
//			delete head;                                              // удаляем
//			head = NULL;
//			nodesNumber--;
//			return;
//		}
//		else {                                                        // иначе:            
//			if (currentNode->nodeData == node_to_del) {               // если текущий элемент надо удалить
//				if (currentNode == head) {                            // если искомым оказалась голова
//					head = head->next;                                // новой головой становится cледующий элемент
//					if (we_have_new_head == false)                    // если до этого голова не менялась, появляется нужда в том,
//						head_is_checked = false;                      // чтобы проверить новую
//					else head_is_checked = true;                      // иначе и новая голова проверена
//					we_have_new_head = true;
//				}
//
//				currentNode->prev->next = currentNode->next;          // замена указателей
//				currentNode->next->prev = currentNode->prev;
//
//
//				RingNode<T>* removed_node = currentNode;              // удаление искомого
//				currentNode = currentNode->next;
//				delete removed_node;
//				nodesNumber--;
//			}
//			else if (currentNode == head && we_have_new_head == true) // если мы вернулись на новую голову, то она проверена
//				head_is_checked = true;
//
//			currentNode = currentNode->prev;                          // продвижение по кольцу влево
//		}
//	} while (currentNode != head || head_is_checked == false);        // цикл пока не вернулись к проверенной голове
//}
//
//// очистить содержимое кольца
//template <class T>
//void Ring<T>::free_ring() {
//	if (head == NULL) return;
//
//	if (head == head->next) {                                 // если в кольце 1 эл-т
//		delete head; head = NULL;                             // очищаем память и возвращаем нулевое значение
//		nodesNumber = 0;
//		return;
//	}
//	else {
//		RingNode<T>* pointer1 = head->next;                   // ук-ль для прохода по кольцу
//		RingNode<T>* pointer2 = pointer1;                     // ук-ль на удаляемый эл-т кольца
//		do {
//			pointer2 = pointer1;                      // запоминаем текущий рассматриваемый эл-т кольца
//			pointer1 = pointer1->next;                // запоминаем следующий
//			delete pointer2;                          // удаляем текущий
//		} while (pointer1 != head);                   // цикл идет до тех пор, пока мы не вернулись в голове кольца
//	}
//	delete head; head = NULL;                                 // очищаем память и возвращаем нулевое значение
//	nodesNumber = 0;
//}
//
//// найти элементы, соответствующие параметрам
//template <class T>
//Ring<T> Ring<T>::find_elem(T filter_node) {
//	Ring<T> neededDataRing;
//	if (head == NULL)
//		return neededDataRing;
//	else {
//		RingNode<T>* currentNode = head;                      // ук-ль для прохода по кольцу
//		do {
//			if (currentNode->nodeData == filter_node)
//				neededDataRing.add(currentNode->nodeData);
//			currentNode = currentNode->next;                  // ук-ль указывает на след. элемент в кольце
//		} while (currentNode != head);                        // цикл идет до тех пор, пока не мы вернулись к голове
//	}
//	return neededDataRing;
//}
//
//// получить доступ к первому элементу по запросу
//template <class T>
//T* Ring<T>::get_access_to_elem_data(T filter_node) {
//	T* pointer_to_return = NULL;
//	if (head == NULL)
//		return NULL;
//	else {
//		RingNode<T>* currentNode = head;                      // ук-ль для прохода по кольцу
//		do {
//			if (currentNode->nodeData == filter_node) {
//				pointer_to_return = &(currentNode->nodeData);
//				return pointer_to_return;
//			}
//			currentNode = currentNode->next;                  // ук-ль указывает на след. элемент в кольце
//		} while (currentNode != head);                        // цикл идет до тех пор, пока не мы вернулись к голове
//	}
//	return pointer_to_return;
//}
//
//// получить количество элементов в кольце
//template <class T>
//int Ring<T>::get_nodesNumber() {
//	return nodesNumber;
//}
