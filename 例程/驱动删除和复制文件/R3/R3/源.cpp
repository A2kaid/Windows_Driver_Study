#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <tchar.h>

#define IOCTRL_BASE 0x800

#define MYIOCTRL_CODE(i) \
	CTL_CODE(FILE_DEVICE_UNKNOWN,IOCTRL_BASE+i,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define CTL_HELLO MYIOCTRL_CODE(0)
#define CTL_PRINT MYIOCTRL_CODE(1)
#define CTL_BYE MYIOCTRL_CODE(2)
#define CTL_DEL MYIOCTRL_CODE(3)
#define CTL_COPY MYIOCTRL_CODE(4)
#define CTL_KILL MYIOCTRL_CODE(5)

int killprocess(HANDLE hDevice) {
	DWORD len = 0;
	UCHAR buffer[20];
	memset(buffer, 0x00, 20);

	long pid = 0;

	// 接收用户输入的pid
	printf("Please Enter the Process ID : ");
	scanf_s("%d", &pid);

	// 发送pid给驱动
	BOOL status = DeviceIoControl(hDevice, CTL_KILL, &pid, 4, buffer, 20, &len, NULL);
	return 0;
}

int deletefile(HANDLE hDevice) {
	WCHAR wchar[50] = { 0 };
	DWORD len = 0;
	UCHAR buffer[20];
	memset(buffer, 0x00, 20);

	long pid = 0;

	// 接收用户输入的pid
	printf("Please Enter the filepath : ");
	wscanf_s(L"%ls", wchar, (unsigned)_countof(wchar));
	wprintf(L"filepath is = %s\n", wchar);

	// 发送pid给驱动
	BOOL status = DeviceIoControl(hDevice, CTL_DEL, wchar, 50*sizeof(WCHAR), buffer, 20, &len, NULL);
	return 0;
}


int main() {
	HANDLE hDevice = NULL;
	wchar_t readbuffer[50] = { 0 };
	CHAR buffer[50] = { 0 };
	DWORD bRead = 0;

	hDevice = CreateFile(_T("\\\\.\\ntdriver"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("open device failed\n");
		system("pause");
		return 0;
	}
	else
	{
		printf("open success\n");
		system("pause");
	}
	ReadFile(hDevice, (PVOID)readbuffer, 50, &bRead, NULL);
	
	wprintf(L"%s", readbuffer);
	system("pause");
	WriteFile(hDevice, "R3 Write message", strlen("R3 Write message"), &bRead, NULL);
	system("pause");

	printf("DeviceIo!\n");
	DeviceIoControl(hDevice, CTL_HELLO, &buffer, 50, &buffer, 50, &bRead, NULL);
	system("pause");

	//printf("kill process test\n");
	//killprocess(hDevice);
	
	printf("delete file test\n");
	deletefile(hDevice);

	system("pause");

	CloseHandle(hDevice);
	return 0;
}