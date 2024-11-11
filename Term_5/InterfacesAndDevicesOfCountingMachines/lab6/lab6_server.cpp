#include <iostream>

#include <winsock2.h>
#include <ws2bth.h>
#include <vector>
#include <thread>
#include <filesystem>
#include <mmsystem.h>
#include <conio.h>

#include "opencv2/opencv.hpp"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Winmm.lib")

#define ALIAS "random_str"
#define FILDIR ".\\files"


using namespace cv;
using std::endl, std::cout, std::thread, std::string, std::cerr, std::cin, std::vector, std::wstring;
namespace fs = std::filesystem;


static const GUID MY_SERVICE =
{ 0x6afcbbcb, 0x995a, 0x425d, { 0xa0, 0x95, 0x9f, 0x6e, 0xac, 0x4f, 0x8e, 0x74 } };

SOCKET blut_socket;
int main_cycle_end = 0;
int end_of_connection = 0;
DWORD err = 0;


void init_sock_addr(SOCKADDR_BTH* sockaddr);
void init_query_set(WSAQUERYSET* set, WCHAR* name_of_server, GUID* guid);
void init_csaddr(CSADDR_INFO* csAddrInfo);
void print_menu();
std::string choose_file_from_dir();
void send_file(string f_name, SOCKET client);
void recieve_procc(SOCKET client);
void play_audio(string file_name);
INT show_png(string photo_n);


int main() {

    setlocale(LC_ALL, "rus");

    WSADATA wsaData;
    int error;

    error = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (error != 0) {
        printf("WSA ERROR\n");
        return -1;
    }

    thread my_thread{};

    SOCKADDR_BTH sockAddr;
    init_sock_addr(&sockAddr);

    WCHAR name_of_server[] = L"Server";
    GUID guid = MY_SERVICE;

    WSAQUERYSETW query_set;
    init_query_set(&query_set, name_of_server, &guid);

    CSADDR_INFO csAddrInfo;
    init_csaddr(&csAddrInfo);

    blut_socket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    if (blut_socket == SOCKET_ERROR) {

        cout << "SOCKET ERROR" << endl;
        return -1;
    }

    sockaddr* addr = (sockaddr*)&sockAddr;
    error = bind(blut_socket, addr, sizeof(sockAddr));

    if (error != 0) {

        cout << "BIND ERROR" << endl;
        return -1;
    }

    int len = sizeof(sockaddr);
    getsockname(blut_socket, addr, &len);
    csAddrInfo.LocalAddr.lpSockaddr = addr;

    query_set.lpcsaBuffer = &csAddrInfo;
    error = WSASetService(&query_set, RNRSERVICE_REGISTER, 0);
    if (error != 0) {

        cout << "WSASetService ERROR" << endl;
        return -1;
    }

    error = listen(blut_socket, 2);
    if (error != 0) {

        cout << "Listen error" << endl;
        return -1;
    }

    int bytes_read = 0;
    int err = 0;

    char choice = ' ';

    while (!main_cycle_end) {

        system("CLS");
        SOCKET client{};

        cout << "WAITING FOR CLIENT CONNECTION... " << endl;
        client = accept(blut_socket, NULL, NULL);

        if (client == INVALID_SOCKET) 
            break;
        my_thread = thread(recieve_procc, client);


        system("CLS");
        print_menu();
        while (1) {

            choice = _getche();

            if (end_of_connection == 1)  {

                //my_thread.join();
                cout << "Connection was broken\n";
                end_of_connection = 0;
                system("pause");
                break;
            }
            
            switch (choice) {
            case '1': {
                system("cls");

                cout << "Trying to send file...\n";
                string fname;
                fname = choose_file_from_dir();

                if (fname == "") {
                    cout << "\nFile is not chosen :(\n";
                }
                else {
                    send_file(fname, client);
                    cout << "\nYour file has been sent\n\n";
                }

                system("pause");
                system("cls");
                print_menu();
                break;
            }
            case '2':
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
            case '3': {

                system("cls");

                cout << "Opening photo...\n";
                string fname;
                fname = choose_file_from_dir();

                if (fname == "") {
                    cout << "\n File is not chosen\n";
                }
                else {
                    if (show_png(fname) == 0)
                        cout << "\nNice foto :) \n\n";
                }

                system("pause");
                system("cls");
                print_menu();
                break;
            }
            case '4': {
                int command_to_send = 2;
                send(client, (char*)&command_to_send, 4, 0);

                end_of_connection = 1;
                closesocket(client);
                my_thread.join();
                break;
            }
            }
        }
    }
    WSACleanup();

    return 0;
}


void init_sock_addr(SOCKADDR_BTH* sockaddr) {

    ZeroMemory(sockaddr, sizeof(SOCKADDR_BTH));
    sockaddr->addressFamily = AF_BTH;
    sockaddr->serviceClassId = RFCOMM_PROTOCOL_UUID;
    sockaddr->port = 27;
    sockaddr->btAddr = 0;
}


void init_query_set(WSAQUERYSET* set, WCHAR* name_of_server, GUID* guid) {

    ZeroMemory(set, sizeof(WSAQUERYSET));

    set->dwSize = sizeof(WSAQUERYSET);
    set->lpszServiceInstanceName = name_of_server;
    set->lpServiceClassId = guid;
    set->dwNameSpace = NS_BTH;
    set->dwNumberOfCsAddrs = 1;
}

void init_csaddr(CSADDR_INFO* csAddrInfo) {

    memset(csAddrInfo, 0, sizeof(CSADDR_INFO));
    csAddrInfo->LocalAddr.iSockaddrLength = sizeof(SOCKADDR_BTH);
    csAddrInfo->iSocketType = SOCK_STREAM;
    csAddrInfo->iProtocol = BTHPROTO_RFCOMM;
}

void print_menu() {
    cout << " |--------------------------------|\n"
        << " |----Connection is accepted :)---|\n"
        << " |----Actions List:---------------|\n"
        << " |--------------------------------|\n"
        << " |-1.-|--Send file----------------|\n"
        << " |-2.-|--Play sound---------------|\n"
        << " |-3.-|--Take a look on photo-----|\n"
        << " |-4.-|--Kick client ))0)---------|\n"
        << " |--------------------------------|\n";
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

void send_file(string f_name, SOCKET client) {

    int com = 1;
    send(client, (char*)&com, 4, 0);
    int file_len = f_name.length();
    send(client, (char*)&file_len, 4, 0);
    send(client, f_name.c_str(), f_name.length(), 0);

    FILE* file;
    fopen_s(&file, f_name.c_str(), "rb");
    if (!file) return;
    char end_flag = 0;
    while (1)
    {
        char buf[512];
        int bytes_read = fread(buf, 1, 512, file);

        err = send(client, buf, bytes_read, 0);
        err = WSAGetLastError();

        if (feof(file)) end_flag = 1;
        err = send(client, &end_flag, 1, 0);
        err = WSAGetLastError();

        if (end_flag == 1) break;

    }
    fclose(file);

}


void recieve_procc(SOCKET client) {

    end_of_connection = 0;

    cout << "HERE IS A CLIENT !!!" << endl;
    int bytes_read = 0;

    char end_flag = 0;
    while (!end_of_connection)
    {
        int command = 0;
        bytes_read = recv(client, (char*)&command, 4, 0);

        if (command == 1)
        {
            char name[512];
            int len{};
            recv(client, (char*)&len, 4, 0);
            bytes_read = recv(client, name, len, 0);
            name[len] = '\0';

            FILE* file;
            fopen_s(&file, name, "wb");
            if (!file) continue;

            while (1)
            {
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
            end_of_connection = 1;
            closesocket(client);
            break;
        }
    }
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
                system("CLS");
                cout << "Choose file for sending: \n";

                string path = FILESDIR;
                vector<fs::directory_entry> files = vector<fs::directory_entry>();

                for (const auto& entry : fs::directory_iterator(path))
                    files.push_back(entry);

                for (int i = 0; i < files.size(); i++)
                {
                    cout << i + 1 << ". " << files[i].path().u8string() << '\n';
                }
                cout << files.size() + 1 << ". Return to menu" << "\n";

                string fname;
                int f_ch{};
                cin >> f_ch;

                if (f_ch == files.size() + 1) continue;

                fname = files[f_ch - 1].path().u8string();
                */

