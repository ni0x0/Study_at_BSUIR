#include <iostream>
#include <conio.h>

#include <windows.h>
#include <setupapi.h>
#include <initguid.h>
#include <devguid.h>
#include <dbt.h>
#include <Usbiodef.h>
#include <string>

#include <cfgmgr32.h>

#pragma comment(lib, "setupapi")

bool print_usb_devices();
bool print_external_devices();
void menu_print();
int get_devs_num();


LRESULT FAR PASCAL WndMsgsHandle(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
DWORD WINAPI initialisationThread(void*);
void register_device(HDEVINFO hDevInfo, SP_DEVICE_INTERFACE_DATA* devInterfaceData);

int devices_num = 0;
int devil_num = 0;
//int devices_num_aftr = 0;

WNDCLASSEXW wind_cl;
HWND wind_handle = NULL;

bool exitFlag = false;

bool eject_dev(int i);
bool is_ej = false;

void dop_info(int i);


int main() {

    //SetConsoleCP(1251);
    //SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    if (print_usb_devices() == false)
        return 1;
    devices_num = get_devs_num();
    //print_external_devices();

    menu_print();

    HANDLE thread = CreateThread(NULL, 0, initialisationThread, NULL, 0, NULL);
    if (thread == NULL) {
        std::cout << "Cannot create thread." << std::endl;
        return -1;
    }


    char choice = 'c';
    while (choice != 'e') {
        choice = _getche();

        switch (choice)
        {
        case 'e':
        {
            exitFlag = true;
            WaitForSingleObject(thread, INFINITE);
            CloseHandle(thread);
            break;
        }
        case 'm':
        {
            system("cls");
            print_external_devices();
            system("pause");
            system("cls");
            print_usb_devices();
            menu_print();
            break;
        }
        case 'j':
        {
            is_ej = true;
            char jch = ' ';
            jch = _getch();
            system("cls");
            std::cout << "Trying to eject " << jch << " ...";
            if ((int)(jch - '0') > devices_num)
                std::cout << "\nWrong number\n"; 
            else 
                eject_dev((int)(jch - '0') - 1);
            system("pause");
            system("cls");
            print_usb_devices();
            menu_print();
            is_ej = false;
            break;
        }
        case 'd':
        {
            char dch = ' ';
            dch = _getch();
            system("cls");
            std::cout << "Trying to get additional info about " << dch << " ...";
            if ((int)(dch - '0') > devices_num)
                std::cout << "\nWrong number\n";
            else
                dop_info((int)(dch - '0') - 1);
            system("pause");
            system("cls");
            print_usb_devices();
            menu_print();
            break;
        }

        }

    }

    return 0;
}

DWORD WINAPI initialisationThread(void*)
{
    // Стр-ра для описания окна
    ZeroMemory(&wind_cl, sizeof(wind_cl));
    wind_cl.cbSize = sizeof(WNDCLASSEX);
    wind_cl.lpfnWndProc = (WNDPROC)WndMsgsHandle;
    wind_cl.lpszClassName = L"AAAAA";

    // Рег-ем класс окна и создаем его
    if (RegisterClassExW(&wind_cl))
        wind_handle = CreateWindowA("AAAAA", "NotifyDEVS", WS_ICONIC, 0, 0, CW_USEDEFAULT, 0, 0, NULL, GetModuleHandle(nullptr), NULL);

    DEV_BROADCAST_DEVICEINTERFACE_A wind_broadcast;
    wind_broadcast.dbcc_size = sizeof(wind_broadcast);
    wind_broadcast.dbcc_classguid = GUID_DEVINTERFACE_USB_DEVICE;
    wind_broadcast.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    RegisterDeviceNotificationA(wind_handle, &wind_broadcast, DEVICE_NOTIFY_WINDOW_HANDLE);

    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_DEVICE, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (hDevInfo != INVALID_HANDLE_VALUE) {

        SP_DEVICE_INTERFACE_DATA devInterfaceData;
        devInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

        DWORD d_ind = 0;
        while (SetupDiEnumDeviceInterfaces(hDevInfo, 0, &GUID_DEVINTERFACE_USB_DEVICE, d_ind, &devInterfaceData)) {

            register_device(hDevInfo, &devInterfaceData);
            
            d_ind++;
        }
        SetupDiDestroyDeviceInfoList(hDevInfo);  // Освобождаем ресурсы
    }

    MSG msg;
    while (!exitFlag)
    {
        if (PeekMessage(&msg, wind_handle, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
}


LRESULT FAR PASCAL WndMsgsHandle(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    if (message == WM_DEVICECHANGE && is_ej == false) {
        switch (wParam) {
        case DBT_DEVICEARRIVAL:
        {
            // Указатель на инфу о сообщении
            PDEV_BROADCAST_DEVICEINTERFACE broadcast_in = (PDEV_BROADCAST_DEVICEINTERFACE)lParam;
            if (broadcast_in->dbcc_classguid != GUID_DEVINTERFACE_USB_DEVICE)
                break;
            
            HDEVINFO deviceList = SetupDiCreateDeviceInfoList(NULL, NULL);
            SetupDiOpenDeviceInterfaceW(deviceList, broadcast_in->dbcc_name, NULL, NULL);

            SP_DEVICE_INTERFACE_DATA devInterfaceData;
            devInterfaceData.cbSize = sizeof(devInterfaceData);
            SetupDiEnumDeviceInterfaces(deviceList, 0, &GUID_DEVINTERFACE_USB_DEVICE, 0, &devInterfaceData);

            register_device(deviceList, &devInterfaceData);

            system("CLS");
            print_usb_devices();
            std::cout << "PLUS DEVICE\n\n";
            menu_print();
            break;
        }
        case DBT_DEVICEREMOVECOMPLETE:
        {
            system("CLS");            
            print_usb_devices();
            std::cout << "MINUS DEVICE\n\n";
            menu_print();
            break;
        }
        case DBT_DEVICEQUERYREMOVE:
        {
            system("CLS");
            print_usb_devices();
            std::cout << "DEVICE IS EJECTED SAFELY\n";
            menu_print();
            break;
        }
        case DBT_DEVICEQUERYREMOVEFAILED:
        {
            system("CLS");
            print_usb_devices();
            std::cout << "FAILED TO EJECT DEVICE\n\n";
            menu_print();
            break;
        }
        }
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

void register_device(HDEVINFO hDevInfo, SP_DEVICE_INTERFACE_DATA* devInterfaceData) {

    DWORD enaugh_size;
    SetupDiGetDeviceInterfaceDetail(hDevInfo, devInterfaceData, NULL, 0, &enaugh_size, NULL);
    if (enaugh_size == 0)
        return;

    //SP_DEVICE_INTERFACE_DETAIL_DATA devInterfaceDetailData [1];
    SP_DEVICE_INTERFACE_DETAIL_DATA* devInterfaceDetailData = (SP_DEVICE_INTERFACE_DETAIL_DATA*)malloc(enaugh_size);
    if (devInterfaceDetailData == NULL)
        return;
    devInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

    if (SetupDiGetDeviceInterfaceDetail(hDevInfo, devInterfaceData, devInterfaceDetailData, enaugh_size, NULL, NULL)) {

        std::wstring devicePath = devInterfaceDetailData->DevicePath;
        //std::cout << "\nPAAAAATH:";
        //std::wcout << devicePath;

        HANDLE deviceHandle = CreateFile(devicePath.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        DEV_BROADCAST_HANDLE dev_broadcast = { 0 };
        dev_broadcast.dbch_size = sizeof(dev_broadcast);
        dev_broadcast.dbch_devicetype = DBT_DEVTYP_HANDLE;
        dev_broadcast.dbch_handle = deviceHandle;
        RegisterDeviceNotification(wind_handle, &dev_broadcast, DEVICE_NOTIFY_WINDOW_HANDLE);
        CloseHandle(deviceHandle);
    }
    free(devInterfaceDetailData);
}

bool print_usb_devices() {

    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DeviceInfoData;
    ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    std::cout << "==============================================================================================\n" 
        << "=== USB Devices List =========================================================================\n"
        << "==============================================================================================";

    // Get a handle to the device information set for camera devices
    // GUID_DEVCLASS_USB GUID_DEVINTERFACE_USB_DEVICE
    // GUID_DEVCLASS_WPD
    hDevInfo = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_USB_DEVICE, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    //hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_WPD, 0, 0, DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to get device information set." << std::endl;
        return false;
    }

    int deviceIndex = 0;
    while (SetupDiEnumDeviceInfo(hDevInfo, deviceIndex, &DeviceInfoData)) {

        std::cout << "\n--- :" << deviceIndex + 1 << " ---------------------------------------------------------------------------------------\n";

        // Get description of a device device
        WCHAR deviceName[255];
        if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_DEVICEDESC, NULL,
            (PBYTE)deviceName, sizeof(deviceName), NULL)) {
            wprintf(L"\tDescription of a Device: %s\n", deviceName);
        }

        // Get representation the device's type
        WCHAR deviceManufacturer[255]; 
        if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_MFG, NULL,
        //if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_SERVICE, NULL,
            (PBYTE)deviceManufacturer, sizeof(deviceManufacturer), NULL)) {
            wprintf(L"\tDevice Manufacturer: %s\n", deviceManufacturer);
        }
        // Get hardware IDs
        WCHAR hardwareID[255];
        if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_HARDWAREID, NULL,
        //if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_LOCATION_PATHS, NULL,
            (PBYTE)hardwareID, sizeof(hardwareID), NULL)) {
            wprintf(L"\tHardware (vendor and device) IDs: %s\n", hardwareID);
        }

        // Get InstanceId
        WCHAR instanceId[255];
        if (SetupDiGetDeviceInstanceIdW(hDevInfo, &DeviceInfoData, (PWSTR)instanceId, 255, NULL)) {
            wprintf(L"\tInstance Id: %s\n", instanceId);
        }

        /*
        DWORD guides[] = { SPDRP_ADDRESS , SPDRP_BUSNUMBER, SPDRP_BUSTYPEGUID , SPDRP_CAPABILITIES,
        SPDRP_CHARACTERISTICS , SPDRP_CLASS, SPDRP_CLASSGUID, SPDRP_COMPATIBLEIDS, SPDRP_CONFIGFLAGS,
        SPDRP_DEVICE_POWER_DATA, SPDRP_DEVICEDESC, SPDRP_DEVTYPE, SPDRP_DRIVER, SPDRP_ENUMERATOR_NAME, SPDRP_EXCLUSIVE,
        SPDRP_FRIENDLYNAME, SPDRP_HARDWAREID, SPDRP_INSTALL_STATE, SPDRP_LEGACYBUSTYPE, SPDRP_LOCATION_INFORMATION, 
        SPDRP_LOCATION_PATHS, SPDRP_LOWERFILTERS , SPDRP_MFG , SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, SPDRP_REMOVAL_POLICY , 
        SPDRP_REMOVAL_POLICY_HW_DEFAULT, SPDRP_REMOVAL_POLICY_OVERRIDE, SPDRP_SECURITY, SPDRP_SECURITY_SDS, 
        SPDRP_SERVICE, SPDRP_UI_NUMBER, SPDRP_UI_NUMBER_DESC_FORMAT, SPDRP_UPPERFILTERS };
        std::cout << "\n+++++++++++++++++++\n++++++++++++++++++++++++++++++++++++\n";
        for (int i = 0; i < (sizeof(guides)/sizeof(DWORD)); i++) {
            
            WCHAR infoe[255];
            if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, guides[i], NULL,
                //if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_LOCATION_PATHS, NULL,
                (PBYTE)infoe, sizeof(infoe), NULL)) {
                wprintf(L"%d!\t %s\n", i, infoe);
            }
            
        }
        std::cout << "\n+++++++++++++++++++\n+++++++++++++++++++++++++++++++++++++++++\n";
        */ 


        //DWORD properties;
//SetupDiGetDeviceRegistryPropertyA(deviceList, &deviceInfo, SPDRP_CAPABILITIES, NULL, (PBYTE)&properties, sizeof(DWORD), NULL);
//this->ejectable = properties & CM_DEVCAP_REMOVABLE;

        //if (deviceIndex == 0)
          //  CM_Request_Device_EjectW(DeviceInfoData.DevInst, nullptr, nullptr, NULL, NULL);

        deviceIndex++;
    }
    devices_num = deviceIndex;
    
    std::cout << "----------------------------------------------------------------------------------------------\n" 
        << "======================================================================================== " << devices_num << " ===\n\n";
    SetupDiDestroyDeviceInfoList(hDevInfo);

    return true;
}


bool print_external_devices() {

    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DeviceInfoData;
    ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    std::cout << "==============================================================================================\n"
        << "=== External Memory Devices List =============================================================\n"
        << "==============================================================================================";

    // Get a handle to the device information set for camera devices
    hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_WPD, 0, 0, DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to get device information set." << std::endl;
        return false;
    }

    
    int deviceIndex = 0;
    while (SetupDiEnumDeviceInfo(hDevInfo, deviceIndex, &DeviceInfoData)) {

        std::cout << "\n--- :" << deviceIndex + 1 << " ---------------------------------------------------------------------------------------\n";

        // Get description of a device device
        WCHAR deviceName[255];
        if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_DEVICEDESC, NULL,
            (PBYTE)deviceName, sizeof(deviceName), NULL)) {
            wprintf(L"\tDescription of a Device: %s\n", deviceName);
        }

        // Get device name (friendly name)
        WCHAR deviceFriendlyName[255];
        if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_FRIENDLYNAME, NULL,
            (PBYTE)deviceFriendlyName, sizeof(deviceFriendlyName), NULL)) {
            wprintf(L"\tDevice Name: %s\n", deviceFriendlyName);
        }

        // Get representation the device's type
        WCHAR deviceDescription[255];
        if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_DEVTYPE, NULL,
            (PBYTE)deviceDescription, sizeof(deviceDescription), NULL)) {
            wprintf(L"\tDevice Type: %s\n", deviceDescription);
        }

        // Get representation the device's type
        WCHAR deviceManufacturer[255];
        if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_MFG, NULL,
        //if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_SERVICE, NULL,
            (PBYTE)deviceManufacturer, sizeof(deviceManufacturer), NULL)) {
            wprintf(L"\tDevice Manufacturer: %s\n", deviceManufacturer);
        }

        // Get hardware IDs
        WCHAR hardwareID[255];
        if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_HARDWAREID, NULL,
        //if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_LOCATION_PATHS, NULL,
            (PBYTE)hardwareID, sizeof(hardwareID), NULL)) {
            wprintf(L"\tHardware (vendor and device) IDs: %s\n", hardwareID);
        }

        // Get InstanceId
        WCHAR instanceId[255];
        if (SetupDiGetDeviceInstanceIdW(hDevInfo, &DeviceInfoData, (PWSTR)instanceId, 255, NULL)) {
            wprintf(L"\tInstance Id: %s\n", instanceId);
        }

        /*
        DWORD guides[] = { SPDRP_ADDRESS , SPDRP_BUSNUMBER, SPDRP_BUSTYPEGUID , SPDRP_CAPABILITIES,
SPDRP_CHARACTERISTICS , SPDRP_CLASS, SPDRP_CLASSGUID, SPDRP_COMPATIBLEIDS, SPDRP_CONFIGFLAGS,
SPDRP_DEVICE_POWER_DATA, SPDRP_DEVICEDESC, SPDRP_DEVTYPE, SPDRP_DRIVER, SPDRP_ENUMERATOR_NAME, SPDRP_EXCLUSIVE,
SPDRP_FRIENDLYNAME, SPDRP_HARDWAREID, SPDRP_INSTALL_STATE, SPDRP_LEGACYBUSTYPE, SPDRP_LOCATION_INFORMATION,
SPDRP_LOCATION_PATHS, SPDRP_LOWERFILTERS , SPDRP_MFG , SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, SPDRP_REMOVAL_POLICY ,
SPDRP_REMOVAL_POLICY_HW_DEFAULT, SPDRP_REMOVAL_POLICY_OVERRIDE, SPDRP_SECURITY, SPDRP_SECURITY_SDS,
SPDRP_SERVICE, SPDRP_UI_NUMBER, SPDRP_UI_NUMBER_DESC_FORMAT, SPDRP_UPPERFILTERS };
        std::cout << "\n+++++++++++++++++++\n++++++++++++++++++++++++++++++++++++\n";
        for (int i = 0; i < (sizeof(guides) / sizeof(DWORD)); i++) {

            WCHAR infoe[255];
            if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, guides[i], NULL,
                //if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_LOCATION_PATHS, NULL,
                (PBYTE)infoe, sizeof(infoe), NULL)) {
                wprintf(L"%d!\t %s\n", i, infoe);
            }

        }
        std::cout << "\n+++++++++++++++++++\n+++++++++++++++++++++++++++++++++++++++++\n";
        */

        deviceIndex++;
    }


    std::cout << "----------------------------------------------------------------------------------------------\n"
        << "==============================================================================================\n\n";
    SetupDiDestroyDeviceInfoList(hDevInfo);

    return true;
}


int get_devs_num() {
    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DeviceInfoData;
    ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    hDevInfo = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_USB_DEVICE, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    //hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_WPD, 0, 0, DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to get device information set." << std::endl;
        return false;
    }

    int deviceIndex = 0;
    while (SetupDiEnumDeviceInfo(hDevInfo, deviceIndex, &DeviceInfoData)) {
        deviceIndex++;
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);

    return deviceIndex;
}


void menu_print() {
    std::cout << "Choose option: " << devil_num << "\n\t e - to exit;\n"
        << "\t m - external memory devices list;\n" 
        << "\t j0-j9 - eject device\n"
        << "\t d0-d9 - dop info\n";
    /*
    char choice = 'c';
    while (choice != 'e') {
        if (_kbhit()) {
            choice = _getch();
            switch (choice)
            {
            case '1': std::cout << 100 << std::endl;
            case 'e': break;
            }
        }
        Sleep(50);
    }
    */
}

bool eject_dev(int i) {

    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DeviceInfoData;
    ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    hDevInfo = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_USB_DEVICE, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to get device information set." << std::endl;
        return false;
    }

    if (SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData)) {

        // Get description of a device device
        WCHAR deviceName[255];
        if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_DEVICEDESC, NULL,
            (PBYTE)deviceName, sizeof(deviceName), NULL)) {
            wprintf(L"\n\tDevice %s is ", deviceName);
        }

        DWORD props;
        SetupDiGetDeviceRegistryPropertyA(hDevInfo, &DeviceInfoData, SPDRP_CAPABILITIES, NULL, (PBYTE)&props, sizeof(DWORD), NULL);
        if (props & CM_DEVCAP_REMOVABLE) {
            std::cout << "removable\n\n";
            if (CM_Request_Device_EjectW(DeviceInfoData.DevInst, nullptr, nullptr, NULL, NULL)) {
                std::cout << "Ejecting is failed (Device is busy)\n";
            }
            else std::cout << "Ejecting is successfull\n";
        }
        else
            std::cout << "not removable\n\n";
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    return true;
}

bool find_rev_in_wpd(std::string rev_str);

void dop_info(int i) {
    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DeviceInfoData;
    ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    hDevInfo = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_USB_DEVICE, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to get device information set." << std::endl;
        return;
    }

    if (SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData)) {

        // Get description of a device device
        WCHAR deviceName[255];
        if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_DEVICEDESC, NULL,
            (PBYTE)deviceName, sizeof(deviceName), NULL)) {
            wprintf(L"\n\tDevice: %s\n", deviceName);
        }

        std::string rev_str = "REV_";
        size_t rev_in = 0;
        // Get hardware IDs
        CHAR hardwareID[255];
        if (SetupDiGetDeviceRegistryPropertyA(hDevInfo, &DeviceInfoData, SPDRP_HARDWAREID, NULL,
            //if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_LOCATION_PATHS, NULL,
            (PBYTE)hardwareID, sizeof(hardwareID), NULL)) {
            //printf("\tHardware (vendor and device) IDs: %s\n", hardwareID);

            std::string hardwareIDstr (hardwareID);
            rev_in = hardwareIDstr.find(rev_str);
            if (rev_in != std::string::npos) {
                rev_str = hardwareIDstr.substr(rev_in + 4, 4);
                std::cout << "\tREV --" << rev_str << "--\n";

                if (find_rev_in_wpd(rev_str) == false)
                    std::cout << "\n No Windows Portable Device information was found :(\n\n";
            }
        }
    }
}

bool find_rev_in_wpd(std::string rev_str) {

    bool info_is_f = false;

    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DeviceInfoData;
    ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    // Get a handle to the device information set for camera devices
    hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_WPD, 0, 0, DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to get device information set." << std::endl;
        return false;
    }

    int deviceIndex = 0;
    while (SetupDiEnumDeviceInfo(hDevInfo, deviceIndex, &DeviceInfoData)) {

        // Get InstanceId
        CHAR instanceId[255];
        if (SetupDiGetDeviceInstanceIdA(hDevInfo, &DeviceInfoData, (PSTR)instanceId, 255, NULL)) {
            //printf("\n\tInstance Id: %s\n", instanceId);
            
            std::string instIDstr(instanceId);
            size_t rev_in = instIDstr.find("REV_");
            if (rev_in != std::string::npos) {
                std::string rev_str2 = instIDstr.substr(rev_in + 4, 4);
                //std::cout << "\tREV --" << rev_str << "--\n";
                if (rev_str == rev_str2) {
                    info_is_f = true;
                    //std::cout << "same";
                    std::cout << "\n--- :" << deviceIndex + 1 << " ---------------------------------------------------------------------------------------\n";

                    // Get description of a device device
                    WCHAR deviceName[255];
                    if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_DEVICEDESC, NULL,
                        (PBYTE)deviceName, sizeof(deviceName), NULL)) {
                        wprintf(L"\tDescription of a Device: %s\n", deviceName);
                    }

                    // Get device name (friendly name)
                    WCHAR deviceFriendlyName[255];
                    if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_FRIENDLYNAME, NULL,
                        (PBYTE)deviceFriendlyName, sizeof(deviceFriendlyName), NULL)) {
                        wprintf(L"\tDevice Name: %s\n", deviceFriendlyName);
                    }

                    // Get representation the device's type
                    WCHAR deviceDescription[255];
                    if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_DEVTYPE, NULL,
                        (PBYTE)deviceDescription, sizeof(deviceDescription), NULL)) {
                        wprintf(L"\tDevice Type: %s\n", deviceDescription);
                    }

                    // Get representation the device's type
                    WCHAR deviceManufacturer[255];
                    if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_MFG, NULL,
                        //if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_SERVICE, NULL,
                        (PBYTE)deviceManufacturer, sizeof(deviceManufacturer), NULL)) {
                        wprintf(L"\tDevice Manufacturer: %s\n", deviceManufacturer);
                    }

                    // Get hardware IDs
                    WCHAR hardwareID[255];
                    if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_HARDWAREID, NULL,
                        //if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_LOCATION_PATHS, NULL,
                        (PBYTE)hardwareID, sizeof(hardwareID), NULL)) {
                        wprintf(L"\tHardware (vendor and device) IDs: %s\n", hardwareID);
                    }
                    
                }
                //else std::cout << "not same";
            }
            //else std::cout << "ref is not found";
        }

        deviceIndex++;
    }


    if (info_is_f)
        std::cout << "----------------------------------------------------------------------------------------------\n\n";
    SetupDiDestroyDeviceInfoList(hDevInfo);

    return info_is_f;
}
