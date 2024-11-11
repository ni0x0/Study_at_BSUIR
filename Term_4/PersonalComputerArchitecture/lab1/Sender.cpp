#include <windows.h>
#include <iostream>
using namespace std;

int main() {

    HANDLE hSerPortHandler;                 // "описатель com порта" 
    LPCTSTR sPortName = L"COM1";
    //LPCTSTR sPortName = L"COM2";

    // Открываем ком порт для записи
    // имя, режим доступа, режим совместного доступа (д.б. запрещен), атрибут защиты файла (для com нет)б
    // управление режимом автосоздания файлом (для com всегда один и тот же), атрибут файла, описатель файла "шаблона" (для com не используется)
    hSerPortHandler = ::CreateFile(sPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    

    // Проверка работоспособности
    if (hSerPortHandler == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            cout << "SERIAL PORT DOESN'T EXIST\n";
        }
        cout << "OTHER ERROR\n";
        return 0;
    }
    else cout << "COM (SENDER) IS FOUND\n\n";


    // Настройка параметров соединения
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);  // длина структуры
    if (!GetCommState(hSerPortHandler, &dcbSerialParams)) {
        cout << "GET STATE ERROR\n";
        return 0;
    }
    dcbSerialParams.BaudRate = CBR_9600;                  // скорость в бит/сек
    dcbSerialParams.ByteSize = 8;                         // количество бит в символе 
    dcbSerialParams.StopBits = ONESTOPBIT;                // длина стопового бита 
    dcbSerialParams.Parity = NOPARITY;                    // режим паритета
    if (!SetCommState(hSerPortHandler, &dcbSerialParams)) {
        cout << "SET STATE ERROR\n";
        return 0;
    }

    // отсылка данных
    while (1) {
        char data_to_send[10] = {'\0'};              // строка для передачи
        cin >> data_to_send;

        DWORD dwSize = sizeof(data_to_send);         // размер этой строки
        DWORD dwBytesWritten;                        // количество байт, что будет передано
        // отправка строки
        BOOL iRet = WriteFile(hSerPortHandler, data_to_send, dwSize, &dwBytesWritten, NULL);
        //cout << dwSize << " Bytes in string. " << dwBytesWritten << " Bytes sended. " << endl;

        if (data_to_send[0] == '$')
            break;
    }

    // закрыть порт
    CloseHandle(hSerPortHandler);
    return 0;
}