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

NTSTATUS kernelCopyFile(PWCHAR pFilePath, PWCHAR pSourcePath)
{
	HANDLE hFile = NULL;
	UNICODE_STRING uSourcePath = { 0 };
	UNICODE_STRING uFilePath = { 0 };
	OBJECT_ATTRIBUTES Obj = { 0 };
	IO_STATUS_BLOCK IoStack = { 0 };
	RtlInitUnicodeString(&uSourcePath, pSourcePath);
	InitializeObjectAttributes(&Obj, &uSourcePath, OBJ_CASE_INSENSITIVE, NULL, NULL);
	DbgPrint("source = %wZ", &uSourcePath);
	NTSTATUS Status = ZwOpenFile(&hFile, STANDARD_RIGHTS_ALL, &Obj,&IoStack, FILE_SHARE_VALID_FLAGS, FILE_SYNCHRONOUS_IO_NONALERT);
	if (!NT_SUCCESS(Status))
	{
		DbgPrint("Open soucefile failed! %x", Status);
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
	pIrp->IoStatus.Status = STATUS_SUCCESS;//记录状态，返回到R3
	pIrp->IoStatus.Information = 0;//记录特别信息，比如实际读或写的长度

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DispatchCreate(PDEVICE_OBJECT pObject, PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS;//记录状态，返回到R3
	pIrp->IoStatus.Information = 0;//记录特别信息，比如实际读或写的长度

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DispatchClose(PDEVICE_OBJECT pObject, PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS;//记录状态，返回到R3
	pIrp->IoStatus.Information = 0;//记录特别信息，比如实际读或写的长度

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DispatchClean(PDEVICE_OBJECT pObject, PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS;//记录状态，返回到R3
	pIrp->IoStatus.Information = 0;//记录特别信息，比如实际读或写的长度

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
	pStack = IoGetCurrentIrpStackLocation(pIrp); //从IRP的栈上获得

	uReadLength = pStack->Parameters.Read.Length;//读取union联合体里面的值
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

	pStack = IoGetCurrentIrpStackLocation(pIrp); //指向IRP的栈上
	uWriteLength = pStack->Parameters.Write.Length;

	pBuffer = ExAllocatePoolWithTag(PagedPool, uWriteLength, 'TSET'); //在内核里分配内存

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

	RtlInitUnicodeString(&uDeviceName, DEVICE_NAME);
	RtlInitUnicodeString(&uLinkName, LINK_NAME);

	HANDLE keyhandle = NULL;
	OBJECT_ATTRIBUTES obja = { 0 };
	ULONG keyop = 0;
	PVOID keyinfo = NULL;

	DbgPrint("reg addr = %wZ", pRegPath);
	InitializeObjectAttributes(&obja, pRegPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	//ntStatus = ZwCreateKey(&keyhandle, KEY_ALL_ACCESS, &obja, 0, NULL, REG_OPTION_NON_VOLATILE, &keyop);
	
	//ntStatus = RtlCheckRegistryKey(RTL_REGISTRY_SERVICES, L"123456");
	//if (!NT_SUCCESS(ntStatus))
	//{
	//	DbgPrint("not found %x", ntStatus);
	//	RtlCreateRegistryKey(RTL_REGISTRY_SERVICES, L"123456");
	//}
	//else
	//{
	//	DbgPrint("found it %x", ntStatus);
	//}

	
	ntStatus = ZwOpenKey(&keyhandle, KEY_ALL_ACCESS, &obja);
	if (NT_SUCCESS(ntStatus))
	{
		keyinfo = ExAllocatePool(NonPagedPool, 0x1000);
		if (!keyinfo)
		{
			DbgPrint("alloc failed");
			return 0;
		}
		RtlZeroMemory(keyinfo, 0x1000);
		UNICODE_STRING name = { 0 };
		RtlInitUnicodeString(&name, L"ImagePath");
		ntStatus = ZwQueryValueKey(keyhandle, &name, KeyValuePartialInformation, keyinfo, 0x1000, &keyop);
		if (!NT_SUCCESS(ntStatus))
		{
			if (ntStatus == STATUS_BUFFER_TOO_SMALL)
			{
				DbgPrint("too small");
			}
			ZwClose(keyhandle);
			ExFreePool(keyinfo);
			return 0;
		}

		PKEY_VALUE_PARTIAL_INFORMATION tempinfo = (PKEY_VALUE_PARTIAL_INFORMATION)keyinfo;
		PWCHAR imagepath = (PWCHAR)(tempinfo->Data);
		DbgPrint("ImagePath = %wS", imagepath);

		UNICODE_STRING head1 = { 0 };
		RtlInitUnicodeString(&head1, L"\\SystemRoot\\");
		UNICODE_STRING head2= { 0 };
		RtlInitUnicodeString(&head2, imagepath);

		DbgPrint("head1 = %wZ head2 = %wZ", &head1, &head2);

		//判断注册表是否已经被修改
		if (RtlPrefixUnicodeString(&head1, &head2, FALSE))
		{

			DbgPrint("head is equal!");
		}
		else
		{
			kernelCopyFile(L"\\??\\C:\\Windows\\System32\\drivers\\test.sys", imagepath);
			//change reg path
			PWCHAR rootpath = L"\\SystemRoot\\System32\\drivers\\test.sys";
			ntStatus = ZwSetValueKey(keyhandle, &name, 0, REG_EXPAND_SZ, rootpath, wcslen(rootpath) * 2 + 2);
			if (!NT_SUCCESS(ntStatus))
			{
				DbgPrint("Error Code %x", ntStatus);
			}

			ZwClose(keyhandle);
			ExFreePool(keyinfo);

			//使用另外一种方式写入注册表，完成开机自启项修改
			ULONG tempstart = 1;
			RtlWriteRegistryValue(RTL_REGISTRY_ABSOLUTE, pRegPath->Buffer, L"Start", REG_DWORD, &tempstart, 4);
		}

	}
	

	ntStatus = IoCreateDevice(pDriverObject, 0, &uDeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &pDeviceObject);

	if (!NT_SUCCESS(ntStatus))              //判断设备对象是否创建成功,成功为0
	{
		DbgPrint("IocreateDevice failed:%x\n", ntStatus);
		return ntStatus;
	}

	pDeviceObject->Flags |= DO_BUFFERED_IO; //R3与R0间通讯方式为bufferIO

	ntStatus = IoCreateSymbolicLink(&uLinkName, &uDeviceName);//创建符号连接，用于R3的交互
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