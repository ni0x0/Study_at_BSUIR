#include "AuthorWindow.h"

// конструктор объекта класса AuthorWindow
AuthorWindow::AuthorWindow(InformationDepot* dep, QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::AuthorWindowClass())
{
	ui->setupUi(this);                            // создание и настройка пользовательского интерфейса
	setWindowTitle("Authorization Window");       // установка названия окна

	ui->stWidg_auth->setCurrentIndex(0);          // установка текущей страницы
	this->dep_p = dep;                            // привязка хранилища с информацией

	// связь со слотами на странице авторизации
	connect(ui->pB_regest, SIGNAL(clicked()), this, SLOT(go_to_registration()));
	connect(ui->pB_logIn, SIGNAL(clicked()), this, SLOT(authoriz_button_push()));

	// связь со слотами на странице регистрации
	connect(ui->pB_regCancel, SIGNAL(clicked()), this, SLOT(back_to_authorization()));
	connect(ui->pB_regRegistr, SIGNAL(clicked()), this, SLOT(register_account_push()));
}

// деструктор
AuthorWindow::~AuthorWindow()
{
	delete ui;
}

// перейти на страницу регистрации
void AuthorWindow::go_to_registration() {
	ui->stWidg_auth->setCurrentIndex(1);
	setWindowTitle("Registration Window");
}

// перейти на страницу авторизации
void AuthorWindow::back_to_authorization() {
	ui->stWidg_auth->setCurrentIndex(0);
	setWindowTitle("Authorization Window");
}

// обработка нажатия кнопка "зарегестрироваться"
void AuthorWindow::register_account_push() {
	char buff_name[USER_NAME_LEN];      // буффер, куда будет занесено и проверено имя пользователя
	char buff_pass[PASSWORD_LEN];       // буффер, куда будет занесен и проверен пароль 

	try
	{                                                    // считывание введенной информации:
		QString _name = ui->lnEd_regName->text();        // имени
		QString _pass1 = ui->lnEd_repPasWD->text();      // пароля
		QString _pass2 = ui->lnEd_regRepPW->text();      // второго пароля для проверки на совпадение

		if (_name.size() > USER_NAME_LEN - 1)            // проверка размеров введенных полей
			throw InputException(0, "Name_field is too long");
		if (_pass1.size() > PASSWORD_LEN - 1)
			throw InputException(0, "Password_field is too long");
		if (_pass2.size() > PASSWORD_LEN - 1)
			throw InputException(0, "Repeat_Password_field is too long");

		if (_name == "")                                // проверка на на наличие введенной информации
			throw InputException(1, "Name_field is empty");
		if (_pass1 == "")
			throw InputException(1, "Password_field is empty");
		if (_pass2 == "")
			throw InputException(1, "Repeat_Password_field is empty");

		if (_pass1 != _pass2)                          // проверка на совпадение паролей
			throw InputException(2, "Password_field and Repeat_Password_field must mutch");

		QByteArray ba = _name.toLocal8Bit();           // преобразование QString в char[]
		char* ch_buff = ba.data();
		strcpy_s(buff_name, USER_NAME_LEN, ch_buff);

		ba = _pass1.toLocal8Bit();
		ch_buff = ba.data();
		strcpy_s(buff_pass, PASSWORD_LEN, ch_buff);
			                                            // проверка на наличие недопустимых символов
		if (!CheckInputFunctions::check_on_eng_letters_n_digits(buff_name) || !CheckInputFunctions::check_on_eng_letters_n_digits(buff_pass))
			throw InputException(2, "Name_field and Password_field must contain only english letters and digits");

		UserInfo checkUs;
		checkUs.set_name(buff_name);                   // проверка на наличие пользователя с введенным именем
		if ((*dep_p).usersInfo_r.get_nodesNumber())
			if ((Algorithm::find_match<Ring<UserInfo>::Iterator, UserInfo>((*dep_p).usersInfo_r.begin(), (*dep_p).usersInfo_r.end(), checkUs)).check())
				throw InputException(3, "User such name already registered");

		QMessageBox::information(this, "", "Registration is successfull!");
	}
	catch (InputException excp)
	{
		QMessageBox::information(this, "", excp.get_exp_msg());
		return;
	}

	UserInfo tempUs(buff_name, buff_pass);   // добавление нового пользователя
	(*dep_p).usersInfo_r.add(tempUs);

	ui->lnEd_regName->setText("");           // очистка строк
	ui->lnEd_repPasWD->setText("");
	ui->lnEd_regRepPW->setText("");

	back_to_authorization();
}

// обработка нажатия кнопки "выполнить вход"
void AuthorWindow::authoriz_button_push() {
	char buff_name[USER_NAME_LEN];                                            // буффер, куда будет занесено и проверено имя пользователя
	char buff_pass[PASSWORD_LEN];                                             // буффер, куда будет занесен и проверен пароль 
	Ring<UserInfo>::Iterator it_user_point = (*dep_p).usersInfo_r.begin();         // итератор для поиска пользователя с совпадающей информацией

	try
	{
		QString _name = ui->lnEd_name->text();                      // считывание введенных имени и пароля
		QString _pass = ui->lnEd_passwd->text();
		if (_name.size() > USER_NAME_LEN - 1)                       // проверка размеров введенных полей
			throw InputException(0, "Name_field is too long");
		if (_pass.size() > PASSWORD_LEN - 1)
			throw InputException(0, "Password_field is too long");

		QByteArray ba = _name.toLocal8Bit();                        // преобразование QString в char[]
		char* ch_buff = ba.data();
		strcpy_s(buff_name, USER_NAME_LEN, ch_buff);
			
		ba = _pass.toLocal8Bit();
		ch_buff = ba.data();
		strcpy_s(buff_pass, PASSWORD_LEN, ch_buff);

		UserInfo user_to_check;                                    // поиск пользователя с вовпадающей информацией
		user_to_check.set_name(buff_name);
		if ((*dep_p).usersInfo_r.get_nodesNumber()) {
			it_user_point = Algorithm::find_match<Ring<UserInfo>::Iterator, UserInfo>(dep_p->usersInfo_r.begin(), dep_p->usersInfo_r.end(), user_to_check);
			if (it_user_point.check() == false)
				throw ContainerException(1, "User with such name is not found");
			if (strcmp((*(it_user_point)).get_passw(), buff_pass))
				throw InputException(2, "Wrong password");
		}
		else throw InputException(2, "There are not registered users");
	}
	catch (InputException excp)
	{
		QMessageBox::information(this, "", excp.get_exp_msg());
		return;
	}
	catch (ContainerException excp)
	{
		QMessageBox::information(this, "", excp.get_exp_msg());
		return;
	}

	QMessageBox::information(this, "", "Authorization is successfull!");
	(*dep_p).user_is_authed = true;            
	(*dep_p).current_user = it_user_point;

	ui->lnEd_name->setText("");
	ui->lnEd_passwd->setText("");
	close();
}