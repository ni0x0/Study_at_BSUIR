#include <windows.h>
#include <conio.h>
#include <iostream>
using namespace std;

void ReadCOM();            // ф-я чтения из com'а

HANDLE hSerPortHandler;    // "описатель com порта" 

int main() {

    LPCTSTR serPortName = L"COM2";

    // Открываем порт для чтения
    // имя, режим доступа, режим совместного доступа (д.б. запрещен), атрибут защиты файла (для com нет)б
    // управление режимом автосоздания файлом (для com всегда один и тот же), атрибут файла, описатель файла "шаблона" (для com не используется)
    hSerPortHandler = ::CreateFile(serPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    // Проверка работоспособности
    if (hSerPortHandler == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            cout << "SERIAL PORT DOESN'T EXIST\n";
        }
        cout << "OTHER ERROR\n";
        return 0;
    }
    else cout << "COM2 (RECIEVER) IS FOUND\n\n";


    // Настройка параметров соединения  
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);        // длина структуры
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

    // чтение из порта
    ReadCOM();

    // закрыть порт
    CloseHandle(hSerPortHandler);
    return 0;
}

// Чтение
void ReadCOM()
{
    DWORD iSize;
    char sReceivedChar;
    while (true) {
        BOOL iRet = ReadFile(hSerPortHandler, &sReceivedChar, 1, &iSize, 0);  // получаем 1 байт
        if (iSize > 0 && iRet) {                                              // если что-то принято, выводим
            if (sReceivedChar == '$')
                break;
            cout << sReceivedChar;
        }
    }
}