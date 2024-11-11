#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>

#include <iostream>

#include <filesystem>
#include <winsock2.h>
#include <conio.h>
#include <ws2bth.h>
#include <vector>
#include <thread>
#include <mmsystem.h>


#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "opencv_world470")
#pragma comment(lib, "opencv_world470d")


#define ALIAS "random_str"
#define FILDIR ".\\files\\"

static const GUID MY_SERVICE =
{ 0x6afcbbcb, 0x995a, 0x425d, { 0xa0, 0x95, 0x9f, 0x6e, 0xac, 0x4f, 0x8e, 0x74 } };

using namespace std;
namespace fs = std::filesystem;
using namespace cv;

struct BlutoothDevice {
    wstring name;
    BTH_ADDR addr{};
    SOCKADDR sock_addr{};
};


vector<BlutoothDevice> bluetooth_devs = vector<BlutoothDevice>();

SOCKADDR_BTH sockAddr;
SOCKET blut_socket{};

bool socket_connected = false;
int connect_ind{};

bool end_flag = false;
DWORD err = 0;

bool end_of_connection = false;
bool recv_thread_is_over = true;

bool thread_is = false;

thread recieve_thread;


INT get_service_port(LPSOCKADDR addr);
BOOL get_devices_list();
void print_menu();
void print_bluetooth_decvs();
void connect_to_device(int dev_num);
void disconnect_from_serv();
void send_file(string f_name);
string choose_file_from_dir();
void recieve_procc(SOCKET client);
void play_audio(string file_name);
void record_file(string fname);
INT make_photo(string photo_n);
INT show_png(string photo_n);


int main()
{
    setlocale(LC_ALL, "rus");
   // utils::logging::setLogLevel(utils::logging::LOG_LEVEL_SILENT);


    WSADATA wsaData;
    int error;

    error = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (error != 0) {
        printf("WSAStartup failed with error: %d\n", error);
        return 1;
    }

    char choice = 'c';
    print_menu();

    while (choice != '8') {

        choice = _getche();


        switch (choice)
        {
        case '1':
        {
            system("cls");

            cout << "Finding devices...";
            get_devices_list();

            print_bluetooth_decvs();
            system("pause");

            system("cls");
            print_menu();

            break;
        }
        case '2':
        {
            if (socket_connected == 1) {
                system("cls");
                cout << "Disconnecting from server...\n";
                disconnect_from_serv();
                cout << "Disconnected\n\n";
                //recv_thread_is_over = 1;

            }
            else {
                system("cls");
                print_bluetooth_decvs();

                cout << "Choose Device Number to connect: ";
                int devNum;
                cin >> devNum;

                if (recv_thread_is_over == 0) {
                    recieve_thread.join();
                    recv_thread_is_over = 1;
                }

                connect_to_device(devNum);
                if (socket_connected == false)
                    cout << "\n...Connection failed :( \n";
                else {
                    recieve_thread = thread(recieve_procc, blut_socket);
                }
            }

            system("pause");
            system("cls");
            print_menu();
            break;
        }
        case '3':
        {
            system("cls");

            cout << "Trying to send file...\n";
            string fname;
            fname = choose_file_from_dir();

            if (fname == "") {
                cout << "\nFile is not chosen :(\n";
            }
            else {
                send_file(fname);
                cout << "\nYour file has been sent\n\n";
            }


            system("pause");
            system("cls");
            print_menu();
            break;
        }
        case '4':
        {
            system("cls");

            cout << "Recording Aydio...\n";
            string fname;
            cout << "Enter file name:";
            cin >> fname;
            fname = FILDIR + fname + ".wav";

            cout << "Wait a few secs...\n";
            record_file(fname);

            system("pause");
            system("cls");
            print_menu();
            break;
        }
        case '5':
        {
            system("cls");

            cout << "Playing Audio File...\n";
            string fname;
            fname = choose_file_from_dir();

            if (fname == "") {
                cout << "\n File is not chosen\n";
            }
            else {
                cout << endl << fname << " ";
                play_audio(fname);
            }

            system("pause");
            system("cls");
            print_menu();
            break;
        }
        case '6': {

            system("cls");

            cout << "Taking photo...\n";
            string fname;
            cout << "Enter file name:";
            cin >> fname;
            fname = FILDIR + fname + ".png";

            cout << "\nSay cheeeeese!";
            make_photo(fname);
            cout << "\nGood" << fname << " one :)\n\n";

            system("pause");
            system("cls");
            print_menu();
            break;
        }
        case '7': {

            system("cls");

            cout << "Opening photo...\n";
            string fname;
            fname = choose_file_from_dir();

            if (fname == "") {
                cout << "\n File is not chosen\n";
            }
            else {
                //cout << endl << fname << " ";
                if (show_png(fname) == 0)
                    cout << "\nNice foto :) \n\n";
            }

            system("pause");
            system("cls");
            print_menu();
            break;
        }
        }

    }

    if (recv_thread_is_over == 0) {
        recieve_thread.join();
        recv_thread_is_over = 1;
    }


    WSACleanup();

    return 0;
}



INT get_service_port(LPSOCKADDR addr)
{
    WSAQUERYSET wsaq;
    ZeroMemory(&wsaq, sizeof(WSAQUERYSET));
    wsaq.dwSize = sizeof(WSAQUERYSET);
    wsaq.dwNameSpace = NS_BTH;

    DWORD addressLength = 128;
    wchar_t* addressString = new wchar_t[addressLength];

    int result = WSAAddressToStringW(addr,
        sizeof(SOCKADDR_BTH),
        NULL,
        addressString,
        &addressLength);

    GUID guid = MY_SERVICE;
    wsaq.lpServiceClassId = &guid;
    wsaq.lpszContext = addressString;

    HANDLE hLookup;
    int iRet = WSALookupServiceBegin(&wsaq, LUP_FLUSHCACHE, &hLookup);
    if (WSAGetLastError() != 0)
    {
        cout << "Bluetooth problems" << endl;
        system("pause");
        return false;
    }

    while (1)
    {
        DWORD dw_size = 0;
        CHAR* buf = new CHAR[4096];
        LPWSAQUERYSET pwsa_result = (LPWSAQUERYSET)buf;
        ZeroMemory(pwsa_result, sizeof(WSAQUERYSET));

        pwsa_result->dwSize = sizeof(WSAQUERYSET);
        pwsa_result->dwNameSpace = NS_BTH;
        pwsa_result->lpBlob = NULL;

        dw_size = 4096;
        iRet = WSALookupServiceNext(hLookup, LUP_RETURN_NAME | LUP_RETURN_ADDR | LUP_FLUSHCACHE, &dw_size, pwsa_result);
        if (WSAGetLastError() != 0)
        {
            delete[] buf;
            break;
        }

        INT res = ((SOCKADDR_BTH*)pwsa_result->lpcsaBuffer->RemoteAddr.lpSockaddr)->port;

        delete[] buf;
        return res;
    }

    WSALookupServiceEnd(hLookup);

    return -1;

}

BOOL get_devices_list()
{
    bluetooth_devs.clear();

    WSAQUERYSET wsaq;
    ZeroMemory(&wsaq, sizeof(WSAQUERYSET));
    wsaq.dwSize = sizeof(WSAQUERYSET);
    wsaq.dwNameSpace = NS_BTH;

    HANDLE hLookup;
    int iRet = WSALookupServiceBegin(&wsaq, LUP_CONTAINERS | LUP_RETURN_ADDR | LUP_RETURN_NAME | LUP_FLUSHCACHE, &hLookup);
    if (WSAGetLastError() != 0)
    {
        cout << "\nBluetooth problems :(\n";
        return false;
    }

    while (1)
    {
        DWORD dw_size = 0;
        CHAR* buf = new CHAR[4096];
        LPWSAQUERYSET pwsa_result = (LPWSAQUERYSET)buf;
        ZeroMemory(pwsa_result, sizeof(WSAQUERYSET));

        pwsa_result->dwSize = sizeof(WSAQUERYSET);
        pwsa_result->dwNameSpace = NS_BTH;
        pwsa_result->lpBlob = NULL;

        dw_size = 4096;
        iRet = WSALookupServiceNext(hLookup, LUP_CONTAINERS | LUP_RETURN_NAME | LUP_RETURN_ADDR | LUP_FLUSHCACHE, &dw_size, pwsa_result);
        if (WSAGetLastError() != 0)
        {
            delete[] buf;
            break;
        }
        BlutoothDevice instance;
        instance.addr = 0;
        instance.name = pwsa_result->lpszServiceInstanceName;
        CHAR* p = (CHAR*)pwsa_result->lpcsaBuffer->RemoteAddr.lpSockaddr;

        instance.sock_addr = *(pwsa_result->lpcsaBuffer->RemoteAddr.lpSockaddr);

        memcpy(&instance.addr, &p[2], 8);

        bluetooth_devs.push_back(instance);
        delete[] buf;
    }

    WSALookupServiceEnd(hLookup);
    return true;
}


void print_menu() {
    cout << " |--------------------------------|\n"
        << " |----Actions List:---------------|\n"
        << " |--------------------------------|\n"
        << " |-1.-|--Get devices list---------|\n";

    if (socket_connected == 0)
        cout << " |-2.-|--Connect to device--------|\n";
    else
        cout << " |-2.-|--Disconnect---------------|\n";

    cout << " |-3.-|--Send file----------------|\n"
        << " |-4.-|--Make audio file----------|\n"
        << " |-5.-|--Play audio file----------|\n"
        << " |-6.-|--Make a photo-------------|\n"
        << " |-7.-|--Take a look on photo-----|\n"
        << " |-8.-|--Exit from program--------|\n"
        << " |--------------------------------|\n";
}

void print_bluetooth_decvs() {
    system("cls");
    if (bluetooth_devs.size() == 0)
        return;

    cout << " |----------------------------------------------|\n"
        << " |--Bluetooth Devices List:---------------------|\n"
        << " |----------------------------------------------|\n"
        << " |---|------------------------------------------|";

    for (int i = 0; i < bluetooth_devs.size(); i++)
    {
        if (bluetooth_devs[i].name.size() == 0) continue;

        cout << "\n |-" << i + 1 << ".|--";
        wcout << left << setw(40) << setfill(L'-') << bluetooth_devs[i].name << L"|";

        DWORD addressLength = 128;
        wchar_t* addressString = new wchar_t[addressLength];

        WSAAddressToStringW(&bluetooth_devs[i].sock_addr, sizeof(SOCKADDR_BTH), NULL, addressString, &addressLength);
        wcout << endl << " |---|----" << left << setw(38) << setfill(L'-') << addressString << L"|\n";

        cout << " |---|------------------------------------------|";
    }
    cout << "\n |----------------------------------------------|\n\n";
}

void connect_to_device(int dev_num) {
    if (dev_num >= 1 && dev_num <= bluetooth_devs.size())
    {
        INT port = get_service_port(&bluetooth_devs[dev_num - 1].sock_addr);

        blut_socket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
        ZeroMemory(&sockAddr, sizeof(sockAddr));

        sockAddr.addressFamily = AF_BTH;

        sockAddr.serviceClassId = MY_SERVICE; //MY_SERVICE; // GUID_NULL;
        sockAddr.port = port;
        sockAddr.btAddr = bluetooth_devs[dev_num - 1].addr;

        int error;
        for (int i = 0; i < 2; i++) {

            error = connect(blut_socket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
            if (error == 0) {

                wcout << L"Connected with " << bluetooth_devs[dev_num - 1].name << L" =)\n\n";
                err = 0;
                socket_connected = true;
                connect_ind = dev_num - 1;

                break;
            }
            else {

                err = WSAGetLastError();
            }
        }
    }

}

void disconnect_from_serv() {
    int com = 2;
    send(blut_socket, (char*)&com, 4, 0);
    end_of_connection = true;
    closesocket(blut_socket);
    socket_connected = false;
}


void send_file(string f_name) {

    int com = 1;
    send(blut_socket, (char*)&com, 4, 0);

    //char f_name[512] = "./random.wav\0";
    int file_len = f_name.length();
    send(blut_socket, (char*)&file_len, 4, 0);

    send(blut_socket, f_name.c_str(), f_name.length(), 0);

    FILE* file;
    fopen_s(&file, f_name.c_str(), "rb");
    if (!file) return;
    char end_flag = 0;
    while (1)
    {
        char buf[512];
        int bytes_read = fread(buf, 1, 512, file);

        err = send(blut_socket, buf, bytes_read, 0);
        err = WSAGetLastError();

        if (feof(file)) end_flag = 1;
        err = send(blut_socket, &end_flag, 1, 0);
        err = WSAGetLastError();

        if (end_flag == 1) break;

    }
    fclose(file);

}

string choose_file_from_dir() {

    string path = FILDIR;
    vector<fs::directory_entry> files = vector<fs::directory_entry>();

    for (const auto& entry : fs::directory_iterator(path))
        files.push_back(entry);

    for (int i = 0; i < files.size(); i++)
        cout << i + 1 << ". " << files[i].path().u8string() << '\n';

    cout << "Choose file: ";
    string fname = "";
    int f_ch{};
    cin >> f_ch;

    if (f_ch > files.size())
        return "";

    fname = files[f_ch - 1].path().u8string();
    return fname;
}


void recieve_procc(SOCKET client) {

    recv_thread_is_over = 0;
    end_of_connection = false;
    thread_is = true;

    cout << "\nMessage from Server!!!\n";
    int bytes_read = 0;

    char end_flag = 0;
    while (!end_of_connection) {

        int command = 0;
        bytes_read = recv(client, (char*)&command, 4, 0);

        if (command == 1) {

            char name[512];
            int len{};
            recv(client, (char*)&len, 4, 0);
            bytes_read = recv(client, name, len, 0);
            name[len] = '\0';

            FILE* file;
            fopen_s(&file, name, "wb");
            if (!file) continue;

            while (1) {

                char buf[512];
                bytes_read = recv(client, buf, 512, 0);

                fwrite(buf, 1, bytes_read, file);
                bytes_read = recv(client, &end_flag, 1, 0);

                if (end_flag)
                {
                    cout << "File is fully accepted :)\n" << "Check " << FILDIR << endl;
                    break;
                }
            }
            fclose(file);
        }
        if (command == 2)
        {
            cout << "Server has kicked you :(\n";
            closesocket(client);
            end_of_connection = true;
            socket_connected = false;

            break;
        }
    }
    cout << "\nThread end\n";
}


void play_audio(string file_name) {

    wstring filePath(file_name.begin(), file_name.end());
    if (mciSendString((L"open \"" + filePath + L"\" type mpegvideo alias mp3").c_str(), NULL, 0, NULL) != 0) {
        cerr << "\nFILE OPEN ERROR\n";
        return;
    }

    if (mciSendString(L"play mp3", NULL, 0, NULL) != 0) {
        cerr << "PLAY ERROR\n";
        mciSendString(L"close mp3", NULL, 0, NULL);
        return;
    }

    printf("now playing...\nTo stop -- ");
    system("pause");

    if (mciSendString(L"close mp3", NULL, 0, NULL) != 0) {
        cerr << "CLOSE ERROR\n";
        return;
    }

    cout << "Good. Isn't it?\n";
}





void record_file(string fname) {

    int mci_error;

    mci_error = mciSendStringA("open new type waveaudio alias wavee", NULL, 0, NULL);
    mci_error = mciSendStringA("set wavee time format ms", NULL, 0, NULL);
    mci_error = mciSendStringA("record wavee notify", NULL, 0, NULL);

    cout << "\nRecording has been started\nTo stop record --- ";
    system("pause");

    mci_error = mciSendStringA("stop wavee", NULL, 0, NULL);

    mci_error = mciSendStringA(("save wavee " + fname).c_str(), NULL, 0, NULL);
    mci_error = mciSendStringA("close wavee", NULL, 0, NULL);
    cout << "Recording has been ended\nFile is saved as " << fname << endl << endl;
}


INT make_photo(string photo_n) {

    VideoCapture cap(0, CAP_DSHOW); // Открыть камеру (0 - индекс камеры, если несколько)

    if (!cap.isOpened()) {
        cerr << "OPEN ERROR" << endl;
        return -1;
    }

    Mat frame;
    cap >> frame;  // Захватываем кадр с камеры.

    if (frame.empty()) {
        cerr << "EMPTY FRAME" << endl;
        return -1;
    }

    // Сохраняем кадр в файл.
    imwrite(photo_n, frame);
    cap.release(); // Освобождаем ресурсы камеры.

    return 0;
}


INT show_png(string photo_n) {

    Mat image = imread(photo_n);

    if (image.empty()) {
        cout << "Picture problems" << endl;
        return -1;
    }

    namedWindow("Image", WINDOW_NORMAL);
    imshow("Image", image);
    waitKey(0);
    destroyAllWindows();

    return 0;
}








































/*

SOCKADDR_BTH sockAddr;
SOCKET btSocket{};

bool socket_connected = false;
bool end_flag = false;
DWORD err = 0;

refresh_blutooth_list();

while (!end_flag)
{
    system("CLS");

    if (err != 0)
    {
        cout << "Error while connecting with code = " << err << endl;
        err = 0;
    }


    if (socket_connected)
    {
        cout << "Connection with device established" << endl;
    }

    cout << "List of bluetooth devices: " << endl;
    for (int i = 0; i < blutooth_list.size(); i++)
    {
        if (blutooth_list[i].name.size() == 0) continue;

        cout << i + 1 << ". ";

        wcout << blutooth_list[i].name << endl;
    }
    cout << blutooth_list.size() + 1 << ". " << "Refresh list" << endl;

    if (socket_connected)
    {
        cout << blutooth_list.size() + 2 << ". " << "Choose file to send" << endl;
        cout << blutooth_list.size() + 3 << ". " << "Disconnect" << endl;
    }


    int ch;
    cin >> ch;
    if (cin.fail())
    {
        cin.clear();
        cin.ignore(100000);
    }


    if (ch >= 1 && ch <= blutooth_list.size())
    {
        INT port = get_service_port(&blutooth_list[ch-1].sock_addr);

        btSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
        ZeroMemory(&sockAddr, sizeof(sockAddr));

        sockAddr.addressFamily = AF_BTH;
        sockAddr.serviceClassId = MY_SERVICE;
        sockAddr.port = port;
        sockAddr.btAddr = blutooth_list[ch - 1].address;

        for (int i = 0; i < 2; i++)
        {
            error = connect(btSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
            if (error == 0)
            {
                wcout << L"Connection with " << blutooth_list[ch - 1].name << L" established" << endl;
                err = 0;
                socket_connected = true;

                break;
            }
            else
            {
                //socket_connected = false;
                err = WSAGetLastError();
            }
        }
    }

    if (ch == blutooth_list.size() + 1)
    {
        cout << "Refresh in progress" << endl;
        refresh_blutooth_list();
    }

    if (ch == blutooth_list.size() + 2)   {
        int com = 1;
        send(btSocket, (char*)&com, 4, 0);

        char f_name[512] = "./random.wav\0";
        send(btSocket, f_name, strlen(f_name) + 1, 0);

        FILE* file;
        fopen_s(&file, f_name, "rb");
        if (!file) continue;
        char end_flag = 0;
        while(1)
        {
            char buf[1000];
            int bytes_read = fread(buf, 1, 1000, file);

            //cout << "--" << buf << "--              " << bytes_read << "--" << endl;

            err = send(btSocket, buf, bytes_read, 0);
            err = WSAGetLastError();

            if (feof(file)) end_flag = 1;
            err = send(btSocket, &end_flag, 1, 0);
            err = WSAGetLastError();

            if (end_flag == 1) break;

        }
        fclose(file);

    }

    if (ch == blutooth_list.size() + 3)
    {
        int com = 2;
        send(btSocket, (char*)&com, 4, 0);

        closesocket(btSocket);
        socket_connected = false;
    }
}
*/
