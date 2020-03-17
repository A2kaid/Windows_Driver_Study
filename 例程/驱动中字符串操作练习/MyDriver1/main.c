#include <ntddk.h>

#define DEVICE_NAME L"\\device\\ntdriver"
#define LINK_NAME L"\\dosdevices\\ntdriver"

#define IOCTRL_BASE 0x800

#define MYIOCTRL_CODE(i) \
	CTL_CODE(FILE_DEVICE_UNKNOWN,IOCTRL_BASE+i,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define CTL_HELLO MYIOCTRL_CODE(0)
#define CTL_PRINT MYIOCTRL_CODE(1)
#define CTL_BYE MYIOCTRL_CODE(2)


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

	DbgPrint("String test begin!\n");

	//在内核中一般使用UNICODE_STRING字符串，使用字符常量初始化UNICODE_STRING如下所示，使用完无需释放
	/************************************************************************
	* 函数名称:RtlInitUnicodeString
	* 功能描述:初始化UNICODE_STRING字符串
	* 参数列表:
		DestinationString:要初始化的UNICODE_STRING字符
		SourceString:字符串的内容
	* 返回 值:VOID
	*************************************************************************/
	UNICODE_STRING src = { 0 };
	RtlInitUnicodeString(&src, L"Hello My First Driver..");
	DbgPrint("src = %wZ",&src);

	//使用一个缓冲区初始化字符串
	UNICODE_STRING str = RTL_CONSTANT_STRING(L"hello");
	UNICODE_STRING str1 = { 0 };
	WCHAR str1_buf[256];

	//复制字符串
	/************************************************************************
	* 函数名称:RtlCopyUnicodeString
	* 功能描述:UNICODE_STRING字符串复制
	* 参数列表:
		DestinationString:目的字符串
		SourceString:源字符串
		* 返回 值:VOID
	*************************************************************************/
	RtlInitEmptyUnicodeString(&str1, str1_buf, sizeof(WCHAR) * 256);
	RtlCopyUnicodeString(&str1, &str);
	UNICODE_STRING str2;

	//拼接字符串，需要注意长度
	RtlInitUnicodeString(&str2, L" world!!!");
	NTSTATUS res = RtlAppendUnicodeStringToString(&str1, &str2);
	if (res == STATUS_BUFFER_TOO_SMALL)
		DbgPrint("string too long");
	else
		DbgPrint("string = %wZ", &str1);

	//字符串比较
	/************************************************************************
	* 函数名称:RtlCompareUnicodeString
	* 功能描述:UNICODE_STRING字符串比较
	* 参数列表:
		String1:要比较的第一个字符串
		String2:要比较的第二个字符串
		CaseInSensitive:是否对大小写敏感
	* 返回 值:比较结果
		如果函数返回值为0，表示两个字符串相等
		如果小于0，则表示第一个字符串小于第二个字符串
		如果大于0，则表示第一个字符串大于第二个字符串
	*************************************************************************/
	UNICODE_STRING Filename1 = { 0 }, Filename2 = { 0 };
	RtlInitUnicodeString(&Filename1, L"C:\\aaa\\bbb\\ccc\\1.doc");
	RtlInitUnicodeString(&Filename2, L"C:\\aaa\\bbb\\ccc\\1.DOC");

	if (RtlCompareUnicodeString(&Filename1, &Filename2, FALSE))
	{
		DbgPrint(("not equal"));
	}
	else
	{
		DbgPrint("equal");
	}

	//UnicodeString转换为AnsiString
	UNICODE_STRING US1;
	RtlInitUnicodeString(&US1, L"Hello World");

	ANSI_STRING AnsiString1;
	NTSTATUS nStatus = RtlUnicodeStringToAnsiString(&AnsiString1, &US1, TRUE);
	if (NT_SUCCESS(nStatus))
	{
		KdPrint(("Conver successfully!\n"));
		KdPrint(("Result:%Z\n", &AnsiString1));
	}
	else
	{
		KdPrint(("Conver unsuccessfully\n"));
	}

	//销毁AnsiString1
	RtlFreeAnsiString(&AnsiString1);


	////将小写转为大写
	///************************************************************************
	//* 函数名称:RtlUpperString
	//* 功能描述:UNICODE_STRING字符串转化成大写
	//* 参数列表:
	//	DestinationString:目的字符串
	//	SourceString:源字符串
	//	AllocateDesttinationString:是否为目的字符串分配内存
	//* 返回 值:VOID
	//*************************************************************************/
	//初始化 UnicodeString1
	UNICODE_STRING UnicodeString1;
	UNICODE_STRING UnicodeString2;
	RtlInitUnicodeString(&UnicodeString1, L"Hello World");
	//变化前
	DbgPrint("UnicodeString1 = %wZ\n", &UnicodeString1);
	//变大写
	RtlUpcaseUnicodeString(&UnicodeString2, &UnicodeString1, TRUE);
	//变化后
	DbgPrint("UnicodeString2 = %wZ\n", &UnicodeString2);
	//销毁 UnicodeString2（UnicodeString1 不用销毁）
	RtlFreeUnicodeString(&UnicodeString2);

	////动态分配内存储存字符串，在内核中防止内存泄漏，使用内置函数分配内存并释放
	UNICODE_STRING usAllocMem = {0};
	usAllocMem.MaximumLength = 0x1000;
	usAllocMem.Buffer = (PWSTR)ExAllocatePool(PagedPool, 0x1000);//申请内存
	UNICODE_STRING wideString = RTL_CONSTANT_STRING(L"wideString...");
	usAllocMem.Length = (wcslen(L"wideString...") + 1) * sizeof(WCHAR);
	ASSERT(usAllocMem.MaximumLength > usAllocMem.Length); 
	RtlCopyUnicodeString(&usAllocMem, &wideString);

	DbgPrint("usAllocMem = %wZ", &usAllocMem);

	ExFreePool(usAllocMem.Buffer); //释放内存
	usAllocMem.Buffer = NULL;
	usAllocMem.Length = usAllocMem.MaximumLength = 0;


	RtlInitUnicodeString(&uDeviceName, DEVICE_NAME);
	RtlInitUnicodeString(&uLinkName, LINK_NAME);

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