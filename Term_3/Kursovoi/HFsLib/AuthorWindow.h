#ifndef AUTHORIZ_WIND_H
#define AUTHORIZ_WIND_H

#include <QDialog>
#include "ui_AuthorWindow.h"

#include <QMessageBox>

#include "InformationDepot.h"
#include "InputExceptions.h"
#include "CheckInputFunctions.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AuthorWindowClass; };
QT_END_NAMESPACE

class AuthorWindow : public QDialog      // класс AuthorWindow, унаследованный от QT класса  QDialog -
{                                        // окно для регистрации и авторизации пользователя
	Q_OBJECT                             // макрос, позволяющий работать с макросами QT

public:                                  
	AuthorWindow(InformationDepot* dep, QWidget* parent = nullptr); // конструктор
	~AuthorWindow();                                                // деструктор

private slots:
	// слоты на странице авторизации
	void go_to_registration();           // перейти на страницу регистрации
	void authoriz_button_push();         // обработка нажатия кнопки "авторизоваться"

	// слоты на странице регистрации
	void back_to_authorization();        // перейти на страницу рецистрации
	void register_account_push();        // обработка нажатия кнопки "зарегистрироваться"

private:
	Ui::AuthorWindowClass *ui;           // указатель на объект "User Interface"
	InformationDepot* dep_p;             // указатель на хранилище информаци, обработка которой будет производиться
};

#endif
