#include <opencv2/opencv.hpp>
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <string>
#include <SetupAPI.h>
#include <initguid.h>
#include <devguid.h>

#pragma comment (lib, "setupapi.lib")


bool show_camera_info();
bool make_photo(std::string photo_n);
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
//DWORD WINAPI threadFunction(LPVOID lpParam);
//bool create_camera_thread();


#define millsecs 200 
std::string photo_name = "photo_.jpg";
bool threadFlagExit = false;
HHOOK hook;
HWND window_handle;
//HANDLE threads[1];
//int threadIds[1];




int main() {

	//SetConsoleCP(1251);
	//SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "Russian");

	if (show_camera_info() == false)
		return 1;
	system("pause");

	window_handle = GetForegroundWindow();
	ShowWindow(window_handle, SW_HIDE);

	hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);

	MSG msg;
	int ind = 0;
	while (!threadFlagExit) {
		PeekMessage(&msg, NULL, 0, 0, 1);

		std::string resultString = photo_name;
		resultString.insert(resultString.find('_') + 1, std::to_string(ind));
		make_photo(resultString);
		ind++;
		
		cv::waitKey(millsecs);
	}
	
	//while (GetMessage(&msg, NULL, 0, 0)) {
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	//}
	UnhookWindowsHookEx(hook);

	return 0;
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION) {
		if (wParam == WM_KEYDOWN) {
			KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
			if (kbdStruct->vkCode == VK_ESCAPE) {
				//ShowWindow(window_handle, SW_SHOW); 
				threadFlagExit = true;
				//MessageBox(NULL, L"Клавиша Esc была нажата", L"Сообщение", MB_ICONINFORMATION);
				//PostQuitMessage(0); // Завершить приложение
			}
		}
	}
	return CallNextHookEx(hook, nCode, wParam, lParam);
}

bool make_photo(std::string photo_n) {

	cv::VideoCapture cap(0, cv::CAP_DSHOW); // Открыть камеру (0 - индекс камеры, если несколько)

	if (!cap.isOpened()) {
		std::cerr << "Ошибка при открытии камеры." << std::endl;
		return false;
	}

	cv::Mat frame;
	cap >> frame; // Захватываем кадр с камеры.

	if (frame.empty()) {
		std::cerr << "Пустой кадр." << std::endl;
		return false;
	}

	// Сохраняем кадр в файл.
	cv::imwrite(photo_n, frame);
	cap.release(); // Освобождаем ресурсы камеры.

	return true;
}


bool show_camera_info() {


	//HDEVINFO handle;
	//handle = SetupDiGetClassDevsA(NULL, NULL, NULL, DIGCF_ALLCLASSES);

	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA DeviceInfoData;
	ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	// Get a handle to the device information set for camera devices
	hDevInfo = SetupDiGetClassDevsW(&GUID_DEVCLASS_CAMERA, 0, 0, DIGCF_PRESENT);
	if (hDevInfo == INVALID_HANDLE_VALUE) {
		std::cerr << "Failed to get device information set." << std::endl;
		return false;
	}

	int deviceIndex = 0;
	while (SetupDiEnumDeviceInfo(hDevInfo, deviceIndex, &DeviceInfoData)) {

		// Get device name (friendly name)
		WCHAR deviceFriendlyName[255];
		if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_FRIENDLYNAME, NULL,
			(PBYTE)deviceFriendlyName, sizeof(deviceFriendlyName), NULL)) {
			wprintf(L"Device Name: %s\n", deviceFriendlyName);
		}

		// Get description of a device device
		WCHAR deviceName[255];
		if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_DEVICEDESC, NULL,
			(PBYTE)deviceName, sizeof(deviceName), NULL)) {
			wprintf(L"Description of a Device: %s\n", deviceName);
		}

		// Get representation the device's type
		WCHAR deviceDescription[255];
		if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_DEVTYPE, NULL,
			(PBYTE)deviceDescription, sizeof(deviceDescription), NULL)) {
			wprintf(L"Device Type: %s\n", deviceDescription);
		}

		// Get representation the device's type
		WCHAR deviceManufacturer[255];
		if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_MFG, NULL,
			(PBYTE)deviceManufacturer, sizeof(deviceManufacturer), NULL)) {
			wprintf(L"Device Manufacturer: %s\n", deviceManufacturer);
		}

		// Get hardware IDs
		WCHAR hardwareID[255];
		if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_HARDWAREID, NULL,
			(PBYTE)hardwareID, sizeof(hardwareID), NULL)) {
			wprintf(L"\nHardware (vendor and device) IDs: %s\n", hardwareID);
		}

		// Get InstanceId
		WCHAR instanceId[255];
		if (SetupDiGetDeviceInstanceIdW(hDevInfo, &DeviceInfoData, (PWSTR)instanceId, 255, NULL)) {
			wprintf(L"Instance Id: %s\n", instanceId);
		}


		wprintf(L"===========================================================================================\n\n");

		deviceIndex++;
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return true;
}