
#include <iostream>
#include <conio.h>
#include <string.h>

#include <Windows.h>
#include <Poclass.h>
#include <powrprof.h>
#include <guiddef.h> 
#include <powersetting.h>
#include <setupapi.h>
#include <BatClass.h>
#include <devguid.h>


#define INITGUID

#pragma comment (lib, "setupapi.lib")
#pragma comment (lib, "Powrprof.lib")

using namespace std;

struct TimeStruct {
    int hours;
    int minutes;
    int seconds;
};

void infoPrint();
void powerSchemePrint();
void battPercPrint();
void battTypePrint();
void battEnergyConsumptMode();

void batteryTimeInfoShow();


int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    system("cls");
   
    char choice = 'c';
    while (choice != '4') {
        if (_kbhit()) {
            choice = _getch();
            switch (choice)
            {
            case '1': batteryTimeInfoShow(); break;
            case '2': SetSuspendState(TRUE, TRUE, FALSE); break;
            case '3': SetSuspendState(FALSE, TRUE, FALSE); break;
            case '4': break;
            }
        }
        infoPrint();
        Sleep(50);
    }    

    exit(0);
}

void set_cur(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void infoPrint() {
    //system("cls");
    set_cur(0, 0);
    cout << "Current Power Scheme: ";
    powerSchemePrint();
    cout << "\nBattery Percentage: ";
    battPercPrint();
    cout << "Battery Type: ";
    battTypePrint();
    cout << "\nEnergy Consumption Mode: ";
    battEnergyConsumptMode();
    cout << "Press:\n-- 1 to get Time Info\n-- 2 to set Hibernate Mode\n-- 3 to set Sleep Mode\n-- 4 to Exit\n";

}

void powerSchemePrint() {
    SYSTEM_POWER_STATUS spsPwr;
    if (GetSystemPowerStatus(&spsPwr)) {
        if (spsPwr.ACLineStatus == 0)
            cout << "offline ";
        else cout << "online ";
    }
    else cout << "Getting System Power Status ERROR";
}

void battPercPrint() {
    SYSTEM_POWER_STATUS spsPwr;
    if (GetSystemPowerStatus(&spsPwr)) {
        cout << (int)spsPwr.BatteryLifePercent << "%\n";
    }
    else cout << "Getting System Power Status ERROR";
}

void battTypePrint() {

    //returns a handle to a device information set that contains requested device information elements for a local computer
    HDEVINFO hdev = SetupDiGetClassDevs(&GUID_DEVCLASS_BATTERY, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    //structure defines a device interface in a device information set
    SP_DEVICE_INTERFACE_DATA did = { 0 };
    did.cbSize = sizeof(did);

    // enumerates the device interfaces that are contained in a device information set
    if (SetupDiEnumDeviceInterfaces(hdev, 0, &GUID_DEVCLASS_BATTERY, 0, &did))
    {
        DWORD cbRequired = 0;
        // returns details about a device interface
        SetupDiGetDeviceInterfaceDetail(hdev, &did, 0, 0, &cbRequired, 0);
        PSP_DEVICE_INTERFACE_DETAIL_DATA pdidd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, cbRequired);
        if (pdidd) {
            pdidd->cbSize = sizeof(*pdidd);

            if (SetupDiGetDeviceInterfaceDetail(hdev, &did, pdidd, cbRequired, &cbRequired, 0)) {
                // Ask it for information.
                HANDLE hBattery = CreateFile(pdidd->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

                if (INVALID_HANDLE_VALUE != hBattery)
                {
                    // Ask the battery for its tag.
                    BATTERY_QUERY_INFORMATION bqi = { 0 };

                    DWORD dwWait = 0;
                    DWORD dwOut;
                    if (DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_TAG, &dwWait, sizeof(dwWait), &bqi.BatteryTag, sizeof(bqi.BatteryTag), &dwOut, NULL) && bqi.BatteryTag) {

                        // With the tag, you can query the battery info.
                        BATTERY_INFORMATION bi = { 0 };
                        bqi.InformationLevel = BatteryInformation;

                        DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_INFORMATION, &bqi, sizeof(bqi), &bi, sizeof(bi), &dwOut, NULL);

                        for (int o = 0; o < 4; o++) {
                            cout << bi.Chemistry[o];
                        };

                    }
                    CloseHandle(hBattery);
                }
            }
            LocalFree(pdidd);
        }
    }
    SetupDiDestroyDeviceInfoList(hdev);
}

void battEnergyConsumptMode() {
    SYSTEM_POWER_STATUS spsPwr;
    if (GetSystemPowerStatus(&spsPwr)) {
        if (spsPwr.SystemStatusFlag == 0)
            cout << "Balanced   \n";
        else cout << "Power Saver\n";
    }
    else cout << "Getting System Power Status ERROR";

    GUID* powSchInfo_ptr = NULL;
    PowerGetActiveScheme(NULL, &powSchInfo_ptr);
    cout << "AA";//powSchInfo.Data4;

    UCHAR displayBuffer[1024];
    DWORD* size = new DWORD;
    *size = 1024;
    PowerReadFriendlyName(NULL, powSchInfo_ptr, NULL, NULL, displayBuffer, size);
    wchar_t* utf_display_buffer = (wchar_t*)displayBuffer;

    cout << "AA";//powSchInfo.Data4;
}

TimeStruct secs_to_hours_n_mins(long long seconds) {
    TimeStruct time;

    time.hours = seconds / 3600;  
    time.minutes = (seconds % 3600) / 60; 
    time.seconds = seconds % 60;



    return time;
}

void batteryTimeInfoShow() {
    system("cls");
    SYSTEM_POWER_STATUS spsPwr;
    if (GetSystemPowerStatus(&spsPwr)) {
        TimeStruct time = secs_to_hours_n_mins((long long)spsPwr.BatteryFullLifeTime);
        cout << "Full Battery Life Time: " << time.hours << "h " << time.minutes << "min " << time.seconds << "sec";

        time = secs_to_hours_n_mins((long long)spsPwr.BatteryLifeTime);
        cout << "\nRemaining Battery Life Time: " << time.hours << "h " << time.minutes << "min " << time.seconds << "sec";
    }
    else cout << "Getting System Power Status ERROR";
    
    cout << "\nEnter any key to Return";
    char a = _getch();
    system("cls");
}











































/*
int main(int argc, char* argv[]) {
    cout << "MRAAAAAAk\n";

    //EFFECTIVE_POWER_MODE aa;// = EffectivePowerModeBatterySaver;
    //EFFECTIVE_POWER_MODE_CALLBACK (aa, NULL);

    //callbackFunction(aa, NULL);


    //GUID* asaa = &asa;

    //PowerGetActiveScheme(NULL, &asaa);
    //cout << "MRAAAAAAk\n";
    int a;
    
    cout << "MRAAAAAAk\n";
    cin >> a;
    
    SetSuspendState(FALSE, FALSE, FALSE);
    //SetSuspendState(TRUE, FALSE, FALSE);
    cout << a << "\nNOOO MRAAAAAAk\n";




    exit(0);
}

*/


//int main(int argc, char* argv[]) {



    //SYSTEM_POWER_STATUS spsPwr;
    //if (GetSystemPowerStatus(&spsPwr)) {
    //    cout << "\nAC Status : " << static_cast<double>(spsPwr.ACLineStatus)
    //        << "\nBattery Status : " << static_cast<double>(spsPwr.BatteryFlag)
    //        << "\nBattery Life % : " << (int)spsPwr.BatteryLifePercent    //static_cast<double>(spsPwr.BatteryLifePercent)
    //        << endl;
    //    //return 0;
    //}
    //else return 1;

    //_BATTERY_INFORMATION batt_info;
    


    /*

    //HDEVINFO DeviceInfoSet;
    //DeviceInfoSet = SetupDiGetClassDevs(&GUID_DEVCLASS_BATTERY, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    HDEVINFO hdev = SetupDiGetClassDevs(&GUID_DEVCLASS_BATTERY, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    //SP_DEVICE_INTERFACE_DATA DeviceInterfaceData = { 0 };
    //ZeroMemory(&DeviceInterfaceData, sizeof(SP_DEVINFO_DATA));
    //DeviceInterfaceData.cbSize = sizeof(SP_DEVINFO_DATA);
    SP_DEVICE_INTERFACE_DATA did = { 0 };
    did.cbSize = sizeof(did);

    //SetupDiEnumDeviceInterfaces(DeviceInfoSet, NULL, &GUID_DEVCLASS_BATTERY, 0, &DeviceInterfaceData);
    //DWORD cbRequired = 0;
    if (SetupDiEnumDeviceInterfaces(hdev, 0, &GUID_DEVCLASS_BATTERY, 0, &did))
    {
        DWORD cbRequired = 0;

        //SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, &DeviceInterfaceData, NULL, NULL, &cbRequired, NULL);
        //PSP_DEVICE_INTERFACE_DETAIL_DATA pdidd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, cbRequired);
        //pdidd->cbSize = sizeof(*pdidd);

        SetupDiGetDeviceInterfaceDetail(hdev, &did, 0, 0, &cbRequired, 0);
        PSP_DEVICE_INTERFACE_DETAIL_DATA pdidd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, cbRequired);
        if (pdidd) {
            pdidd->cbSize = sizeof(*pdidd);

            //SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, &DeviceInterfaceData, pdidd, cbRequired, &cbRequired, NULL);
            //HANDLE hBattery = CreateFile(pdidd->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (SetupDiGetDeviceInterfaceDetail(hdev, &did, pdidd, cbRequired, &cbRequired, 0)) {
                // Ask it for information.
                HANDLE hBattery = CreateFile(pdidd->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

                //BATTERY_QUERY_INFORMATION BatteryQueryInformation = { 0 };
                //DWORD bytesWait = 0;
                //DWORD bytesReturned = 0;
                //DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_TAG, &bytesWait, sizeof(bytesWait), &BatteryQueryInformation.BatteryTag,
                //    sizeof(BatteryQueryInformation.BatteryTag), &bytesReturned, NULL) && BatteryQueryInformation.BatteryTag;
                if (INVALID_HANDLE_VALUE != hBattery)
                {
                    // Ask the battery for its tag.
                    BATTERY_QUERY_INFORMATION bqi = { 0 };

                    DWORD dwWait = 0;
                    DWORD dwOut;
                    if (DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_TAG, &dwWait, sizeof(dwWait), &bqi.BatteryTag, sizeof(bqi.BatteryTag), &dwOut, NULL) && bqi.BatteryTag) {

                        //BATTERY_INFORMATION BatteryInfo = { 0 };
                        //BatteryQueryInformation.InformationLevel = BatteryInformation;
                        // With the tag, you can query the battery info.
                        BATTERY_INFORMATION bi = { 0 };
                        bqi.InformationLevel = BatteryInformation;

                        //DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_INFORMATION, &BatteryQueryInformation, sizeof(BatteryQueryInformation),
                            //&BatteryInfo, sizeof(BatteryInfo), &bytesReturned, NULL);
                        DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_INFORMATION, &bqi, sizeof(bqi), &bi, sizeof(bi), &dwOut, NULL);

                        for (int b = 0; b < 4; b++)
                        {
                            cout << bi.Chemistry[b];
                        };

                    }
                    CloseHandle(hBattery);
                }
            }
            LocalFree(pdidd);
        }
    }
    SetupDiDestroyDeviceInfoList(hdev);

    */
//    exit(0);
//}


//int main(int argc, char* argv[]) {



    //SYSTEM_POWER_STATUS spsPwr;
    //if (GetSystemPowerStatus(&spsPwr)) {
    //    cout << "\nAC Status : " << static_cast<double>(spsPwr.ACLineStatus)
    //        << "\nBattery Status : " << static_cast<double>(spsPwr.BatteryFlag)
    //        << "\nBattery Life % : " << (int)spsPwr.BatteryLifePercent    //static_cast<double>(spsPwr.BatteryLifePercent)
    //        << endl;
    //    //return 0;
    //}
    //else return 1;

    //_BATTERY_INFORMATION batt_info;
    


    /*

    //HDEVINFO DeviceInfoSet;
    //DeviceInfoSet = SetupDiGetClassDevs(&GUID_DEVCLASS_BATTERY, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    HDEVINFO hdev = SetupDiGetClassDevs(&GUID_DEVCLASS_BATTERY, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    //SP_DEVICE_INTERFACE_DATA DeviceInterfaceData = { 0 };
    //ZeroMemory(&DeviceInterfaceData, sizeof(SP_DEVINFO_DATA));
    //DeviceInterfaceData.cbSize = sizeof(SP_DEVINFO_DATA);
    SP_DEVICE_INTERFACE_DATA did = { 0 };
    did.cbSize = sizeof(did);

    //SetupDiEnumDeviceInterfaces(DeviceInfoSet, NULL, &GUID_DEVCLASS_BATTERY, 0, &DeviceInterfaceData);
    //DWORD cbRequired = 0;
    if (SetupDiEnumDeviceInterfaces(hdev, 0, &GUID_DEVCLASS_BATTERY, 0, &did))
    {
        DWORD cbRequired = 0;

        //SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, &DeviceInterfaceData, NULL, NULL, &cbRequired, NULL);
        //PSP_DEVICE_INTERFACE_DETAIL_DATA pdidd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, cbRequired);
        //pdidd->cbSize = sizeof(*pdidd);

        SetupDiGetDeviceInterfaceDetail(hdev, &did, 0, 0, &cbRequired, 0);
        PSP_DEVICE_INTERFACE_DETAIL_DATA pdidd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, cbRequired);
        if (pdidd) {
            pdidd->cbSize = sizeof(*pdidd);

            //SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, &DeviceInterfaceData, pdidd, cbRequired, &cbRequired, NULL);
            //HANDLE hBattery = CreateFile(pdidd->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (SetupDiGetDeviceInterfaceDetail(hdev, &did, pdidd, cbRequired, &cbRequired, 0)) {
                // Ask it for information.
                HANDLE hBattery = CreateFile(pdidd->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

                //BATTERY_QUERY_INFORMATION BatteryQueryInformation = { 0 };
                //DWORD bytesWait = 0;
                //DWORD bytesReturned = 0;
                //DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_TAG, &bytesWait, sizeof(bytesWait), &BatteryQueryInformation.BatteryTag,
                //    sizeof(BatteryQueryInformation.BatteryTag), &bytesReturned, NULL) && BatteryQueryInformation.BatteryTag;
                if (INVALID_HANDLE_VALUE != hBattery)
                {
                    // Ask the battery for its tag.
                    BATTERY_QUERY_INFORMATION bqi = { 0 };

                    DWORD dwWait = 0;
                    DWORD dwOut;
                    if (DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_TAG, &dwWait, sizeof(dwWait), &bqi.BatteryTag, sizeof(bqi.BatteryTag), &dwOut, NULL) && bqi.BatteryTag) {

                        //BATTERY_INFORMATION BatteryInfo = { 0 };
                        //BatteryQueryInformation.InformationLevel = BatteryInformation;
                        // With the tag, you can query the battery info.
                        BATTERY_INFORMATION bi = { 0 };
                        bqi.InformationLevel = BatteryInformation;

                        //DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_INFORMATION, &BatteryQueryInformation, sizeof(BatteryQueryInformation),
                            //&BatteryInfo, sizeof(BatteryInfo), &bytesReturned, NULL);
                        DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_INFORMATION, &bqi, sizeof(bqi), &bi, sizeof(bi), &dwOut, NULL);

                        for (int b = 0; b < 4; b++)
                        {
                            cout << bi.Chemistry[b];
                        };

                    }
                    CloseHandle(hBattery);
                }
            }
            LocalFree(pdidd);
        }
    }
    SetupDiDestroyDeviceInfoList(hdev);

    */
//    exit(0);
//}


//#include <windows.h>


//int main() {
//    SYSTEM_POWER_STATUS sps = { 0 };
//    if (GetSystemPowerStatus(&sps)) {
//        //check the structure
//        cout << sps.BatteryLifeTime;
//    }
//    return 0;
//}



/*
    DWORD GetBatteryState() {
#define GBS_HASBATTERY 0x1
#define GBS_ONBATTERY  0x2
        // Returned value includes GBS_HASBATTERY if the system has a 
        // non-UPS battery, and GBS_ONBATTERY if the system is running on 
        // a battery.
        //
        // dwResult & GBS_ONBATTERY means we have not yet found AC power.
        // dwResult & GBS_HASBATTERY means we have found a non-UPS battery.

        DWORD dwResult = GBS_ONBATTERY;

        // IOCTL_BATTERY_QUERY_INFORMATION,
        // enumerate the batteries and ask each one for information.

        HDEVINFO hdev =
            SetupDiGetClassDevs(&GUID_DEVCLASS_BATTERY,
                0,
                0,
                DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
        if (INVALID_HANDLE_VALUE != hdev)
        {
            // Limit search to 100 batteries max
            for (int idev = 0; idev < 100; idev++)
            {
                SP_DEVICE_INTERFACE_DATA did = { 0 };
                did.cbSize = sizeof(did);

                if (SetupDiEnumDeviceInterfaces(hdev,
                    0,
                    &GUID_DEVCLASS_BATTERY,
                    idev,
                    &did))
                {
                    DWORD cbRequired = 0;

                    SetupDiGetDeviceInterfaceDetail(hdev,
                        &did,
                        0,
                        0,
                        &cbRequired,
                        0);
                    if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
                    {
                        PSP_DEVICE_INTERFACE_DETAIL_DATA pdidd =
                            (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR,
                                cbRequired);
                        if (pdidd)
                        {
                            pdidd->cbSize = sizeof(*pdidd);
                            if (SetupDiGetDeviceInterfaceDetail(hdev,
                                &did,
                                pdidd,
                                cbRequired,
                                &cbRequired,
                                0))
                            {
                                // Enumerated a battery.  Ask it for information.
                                HANDLE hBattery =
                                    CreateFile(pdidd->DevicePath,
                                        GENERIC_READ | GENERIC_WRITE,
                                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                                        NULL,
                                        OPEN_EXISTING,
                                        FILE_ATTRIBUTE_NORMAL,
                                        NULL);
                                if (INVALID_HANDLE_VALUE != hBattery)
                                {
                                    // Ask the battery for its tag.
                                    BATTERY_QUERY_INFORMATION bqi = { 0 };

                                    DWORD dwWait = 0;
                                    DWORD dwOut;

                                    if (DeviceIoControl(hBattery,
                                        IOCTL_BATTERY_QUERY_TAG,
                                        &dwWait,
                                        sizeof(dwWait),
                                        &bqi.BatteryTag,
                                        sizeof(bqi.BatteryTag),
                                        &dwOut,
                                        NULL)
                                        && bqi.BatteryTag)
                                    {
                                        // With the tag, you can query the battery info.
                                        BATTERY_INFORMATION bi = { 0 };
                                        bqi.InformationLevel = BatteryInformation;

                                        if (DeviceIoControl(hBattery,
                                            IOCTL_BATTERY_QUERY_INFORMATION,
                                            &bqi,
                                            sizeof(bqi),
                                            &bi,
                                            sizeof(bi),
                                            &dwOut,
                                            NULL))
                                        {
                                            // Only non-UPS system batteries count
                                            if (bi.Capabilities & BATTERY_SYSTEM_BATTERY)
                                            {
                                                if (!(bi.Capabilities & BATTERY_IS_SHORT_TERM))
                                                {
                                                    dwResult |= GBS_HASBATTERY;
                                                }

                                                // Query the battery status.
                                                BATTERY_WAIT_STATUS bws = { 0 };
                                                bws.BatteryTag = bqi.BatteryTag;

                                                BATTERY_STATUS bs;
                                                if (DeviceIoControl(hBattery,
                                                    IOCTL_BATTERY_QUERY_STATUS,
                                                    &bws,
                                                    sizeof(bws),
                                                    &bs,
                                                    sizeof(bs),
                                                    &dwOut,
                                                    NULL))
                                                {
                                                    if (bs.PowerState & BATTERY_POWER_ON_LINE)
                                                    {
                                                        dwResult &= ~GBS_ONBATTERY;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    CloseHandle(hBattery);
                                }
                            }
                            LocalFree(pdidd);
                        }
                    }
                }
                else  if (ERROR_NO_MORE_ITEMS == GetLastError())
                {
                    break;  // Enumeration failed - perhaps we're out of items
                }
            }
            SetupDiDestroyDeviceInfoList(hdev);
        }

        //  Final cleanup:  If we didn't find a battery, then presume that we
        //  are on AC power.

        if (!(dwResult & GBS_HASBATTERY))
            dwResult &= ~GBS_ONBATTERY;

        return dwResult;
    }
    */