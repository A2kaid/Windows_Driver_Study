#include <ntifs.h>

#define DEVICE_NAME L"\\device\\ntdriver"
#define LINK_NAME L"\\dosdevices\\ntdriver"

#define IOCTRL_BASE 0x800

#define MYIOCTRL_CODE(i) \
	CTL_CODE(FILE_DEVICE_UNKNOWN,IOCTRL_BASE+i,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define CTL_HELLO MYIOCTRL_CODE(0)
#define CTL_PRINT MYIOCTRL_CODE(1)
#define CTL_BYE MYIOCTRL_CODE(2)
#define CTL_DEL MYIOCTRL_CODE(3)
#define CTL_COPY MYIOCTRL_CODE(4)

NTSTATUS kernelDeleteFile(PWCHAR pFilePath)
{
	UNICODE_STRING uFilePath = { 0 };
	OBJECT_ATTRIBUTES Obj = { 0 };
	RtlInitUnicodeString(&uFilePath, pFilePath);
	DbgPrint("file = %wS\n", pFilePath);
	DbgPrint("file = %wZ\n", &uFilePath);
	DbgPrint("============================");
	InitializeObjectAttributes(&Obj, &uFilePath, OBJ_CASE_INSENSITIVE, NULL, NULL);
	NTSTATUS Status = ZwDeleteFile(&Obj);

	if (!NT_SUCCESS(Status))
	{
		DbgPrint("Delete file failed! %x", Status);
		return Status;
	}
	return Status;
}

NTSTATUS kernelCopyFile(PWCHAR pFilePath, PWCHAR pSourcePath)
{
	HANDLE hFile = NULL;
	UNICODE_STRING uSourcePath = { 0 };
	UNICODE_STRING uFilePath = { 0 };
	OBJECT_ATTRIBUTES Obj = { 0 };
	IO_STATUS_BLOCK IoStack = { 0 };
	RtlInitUnicodeString(&uSourcePath, pSourcePath);
	InitializeObjectAttributes(&Obj, &uSourcePath, OBJ_CASE_INSENSITIVE, NULL, NULL);
	NTSTATUS Status = ZwOpenFile(&hFile, GENERIC_ALL, &Obj,&IoStack, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_SYNCHRONOUS_IO_NONALERT);
	if (!NT_SUCCESS(Status))
	{
		DbgPrint("Open soucefile failed!");
		return Status;
	}
	FILE_STANDARD_INFORMATION filea = { 0 };
	Status = ZwQueryInformationFile(hFile, &IoStack, &filea, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
	if (!NT_SUCCESS(Status))
	{
		DbgPrint("QueryInformation soucefile failed!");
		ZwClose(hFile);
		return Status;
	}
	PVOID filebuffer = NULL;
	filebuffer = ExAllocatePool(NonPagedPool, filea.EndOfFile.QuadPart);
	if (!filebuffer)
	{
		DbgPrint("Alloc failed!");
		ZwClose(hFile);
		return Status;
	}
	RtlZeroMemory(filebuffer, filea.EndOfFile.QuadPart);
	LARGE_INTEGER readoffset = { 0 };
	readoffset.QuadPart = 0;
	Status = ZwReadFile(hFile, NULL, NULL, NULL, &IoStack, filebuffer, filea.EndOfFile.QuadPart, &readoffset, NULL);
	if (!filebuffer)
	{
		DbgPrint("read failed!");
		ZwClose(hFile);
		ExFreePool(filebuffer);
		return Status;
	}
	DbgPrint("Ioinfo = %d", IoStack.Information);
	ZwClose(hFile);

	HANDLE hFile2 = NULL;
	UNICODE_STRING pdFilePath = { 0 };
	OBJECT_ATTRIBUTES Obj2 = { 0 };
	IO_STATUS_BLOCK IoStack2 = { 0 };
	RtlInitUnicodeString(&pdFilePath, pFilePath);
	InitializeObjectAttributes(&Obj2, &pdFilePath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	Status = ZwCreateFile(&hFile2, &pdFilePath, &Obj2, &IoStack2, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_WRITE, FILE_SUPERSEDE, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
	if (!NT_SUCCESS(Status))
	{
		DbgPrint("Creat file failed!");
		ExFreePool(filebuffer);
		return Status;
	}
	LARGE_INTEGER writeoffset = { 0 };
	writeoffset.QuadPart = 0;
	Status = ZwWriteFile(hFile2, NULL, NULL,NULL,&IoStack2, filebuffer, filea.EndOfFile.QuadPart, &writeoffset, NULL);
	if (!NT_SUCCESS(Status))
	{
		DbgPrint("Write file failed!");
		ExFreePool(filebuffer);
		ZwClose(hFile2);
		return Status;
	}
	DbgPrint("wirte num = %d", IoStack2.Information);
	ExFreePool(filebuffer);
	ZwClose(hFile2);

	return Status;
}

NTSTATUS DispatchCommon(PDEVICE_OBJECT pObject, PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS;//��¼״̬�����ص�R3
	pIrp->IoStatus.Information = 0;//��¼�ر���Ϣ������ʵ�ʶ���д�ĳ���

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DispatchCreate(PDEVICE_OBJECT pObject, PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS;//��¼״̬�����ص�R3
	pIrp->IoStatus.Information = 0;//��¼�ر���Ϣ������ʵ�ʶ���д�ĳ���

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DispatchClose(PDEVICE_OBJECT pObject, PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS;//��¼״̬�����ص�R3
	pIrp->IoStatus.Information = 0;//��¼�ر���Ϣ������ʵ�ʶ���д�ĳ���

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DispatchClean(PDEVICE_OBJECT pObject, PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS;//��¼״̬�����ص�R3
	pIrp->IoStatus.Information = 0;//��¼�ر���Ϣ������ʵ�ʶ���д�ĳ���

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DispatchRead(PDEVICE_OBJECT pObject, PIRP pIrp)
{
	PVOID pReadBuffer = NULL;
	ULONG uReadLength = 0;
	PIO_STACK_LOCATION pStack = NULL;
	ULONG uMin = 0;
	ULONG uHellostr = 0;

	uHellostr = (wcslen(L"Hello world") + 1) * sizeof(WCHAR);

	pReadBuffer = pIrp->AssociatedIrp.SystemBuffer;
	pStack = IoGetCurrentIrpStackLocation(pIrp); //��IRP��ջ�ϻ��

	uReadLength = pStack->Parameters.Read.Length;//��ȡunion�����������ֵ
	uMin = uReadLength > uHellostr ? uHellostr : uReadLength;

	RtlCopyMemory(pReadBuffer, L"Hello world", uMin);

	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = uMin;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DispatchWrite(PDEVICE_OBJECT pObject, PIRP pIrp)
{
	PVOID pWriteBuff = NULL;
	ULONG uWriteLength = 0;
	PIO_STACK_LOCATION pStack = NULL;

	PVOID pBuffer = NULL;

	pWriteBuff = pIrp->AssociatedIrp.SystemBuffer;

	pStack = IoGetCurrentIrpStackLocation(pIrp); //ָ��IRP��ջ��
	uWriteLength = pStack->Parameters.Write.Length;

	pBuffer = ExAllocatePoolWithTag(PagedPool, uWriteLength, 'TSET'); //���ں�������ڴ�

	if (pBuffer == NULL)
	{
		pIrp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
		pIrp->IoStatus.Information = 0;
		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	memset(pBuffer, 0, uWriteLength);

	RtlCopyMemory(pBuffer, pWriteBuff, uWriteLength);
	DbgPrint("%s\n", pBuffer);
	ExFreePool(pBuffer);
	pBuffer = NULL;

	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = uWriteLength;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DispatchIoctrl(PDEVICE_OBJECT pObject, PIRP pIrp)
{
	NTSTATUS status = 0;
	ULONG uIoctrlCode = 0;
	PVOID pInputBuff = NULL;
	PVOID pOutputBuff = NULL;
	WCHAR filepath = { 0 };
	ULONG uInputLength = 0;
	ULONG uOutputLength = 0;
	PIO_STACK_LOCATION pStack = NULL;

	pInputBuff = pOutputBuff = pIrp->AssociatedIrp.SystemBuffer;

	pStack = IoGetCurrentIrpStackLocation(pIrp);
	uInputLength = pStack->Parameters.DeviceIoControl.InputBufferLength;
	uOutputLength = pStack->Parameters.DeviceIoControl.OutputBufferLength;

	uIoctrlCode = pStack->Parameters.DeviceIoControl.IoControlCode;

	switch (uIoctrlCode)
	{
	case CTL_HELLO:
		DbgPrint("Hello controls\n");
		break;
	case CTL_PRINT:
		DbgPrint("%ws\n", pInputBuff);
		break;
	case CTL_BYE:
		DbgPrint("Goodbye controls\n");
		break;
	case CTL_DEL:
		pInputBuff = pIrp->AssociatedIrp.SystemBuffer;
		DbgPrint("start delete");
		DbgPrint("filepath = %wS\n", pInputBuff);
		kernelDeleteFile(pInputBuff);
		break;
	case CTL_COPY:
		DbgPrint("Goodbye controls\n");
		break;
	default:
		DbgPrint("Unknown controls\n");
	}

	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

VOID DriverUnload(PDRIVER_OBJECT pDriverObject)
{
	UNICODE_STRING uLinkName = { 0 };
	RtlInitUnicodeString(&uLinkName, LINK_NAME);
	IoDeleteSymbolicLink(&uLinkName);

	IoDeleteDevice(pDriverObject->DeviceObject);

	DbgPrint("Driver unload success!\n");
}


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegPath)
{
	UNICODE_STRING uDeviceName = { 0 };
	UNICODE_STRING uLinkName = { 0 };
	NTSTATUS ntStatus = 0;
	PDEVICE_OBJECT pDeviceObject = NULL;
	ULONG i = 0;

	//kernelDeleteFile(L"\\??\\C:\\123.txt");
	//kernelCopyFile(L"\\??\\C:\\888.txt",L"\\??\\C:\\666.txt");

	RtlInitUnicodeString(&uDeviceName, DEVICE_NAME);
	RtlInitUnicodeString(&uLinkName, LINK_NAME);

	ntStatus = IoCreateDevice(pDriverObject, 0, &uDeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &pDeviceObject);

	if (!NT_SUCCESS(ntStatus))              //�ж��豸�����Ƿ񴴽��ɹ�,�ɹ�Ϊ0
	{
		DbgPrint("IocreateDevice failed:%x\n", ntStatus);
		return ntStatus;
	}

	pDeviceObject->Flags |= DO_BUFFERED_IO; //R3��R0��ͨѶ��ʽΪbufferIO

	ntStatus = IoCreateSymbolicLink(&uLinkName, &uDeviceName);//�����������ӣ�����R3�Ľ���
	if (!NT_SUCCESS(ntStatus))
	{
		IoDeleteDevice(pDeviceObject);
		DbgPrint("IocreateSybolicLink failed:%x\n", ntStatus);
		return ntStatus;
	}

	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		pDriverObject->MajorFunction[i] = DispatchCommon;
	}

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
	pDriverObject->MajorFunction[IRP_MJ_READ] = DispatchRead;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = DispatchWrite;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIoctrl;
	pDriverObject->MajorFunction[IRP_MJ_CLEANUP] = DispatchClean;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;

	pDriverObject->DriverUnload = DriverUnload;

	return STATUS_SUCCESS;
}