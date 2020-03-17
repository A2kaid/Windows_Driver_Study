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

	//���ں���һ��ʹ��UNICODE_STRING�ַ�����ʹ���ַ�������ʼ��UNICODE_STRING������ʾ��ʹ���������ͷ�
	/************************************************************************
	* ��������:RtlInitUnicodeString
	* ��������:��ʼ��UNICODE_STRING�ַ���
	* �����б�:
		DestinationString:Ҫ��ʼ����UNICODE_STRING�ַ�
		SourceString:�ַ���������
	* ���� ֵ:VOID
	*************************************************************************/
	UNICODE_STRING src = { 0 };
	RtlInitUnicodeString(&src, L"Hello My First Driver..");
	DbgPrint("src = %wZ",&src);

	//ʹ��һ����������ʼ���ַ���
	UNICODE_STRING str = RTL_CONSTANT_STRING(L"hello");
	UNICODE_STRING str1 = { 0 };
	WCHAR str1_buf[256];

	//�����ַ���
	/************************************************************************
	* ��������:RtlCopyUnicodeString
	* ��������:UNICODE_STRING�ַ�������
	* �����б�:
		DestinationString:Ŀ���ַ���
		SourceString:Դ�ַ���
		* ���� ֵ:VOID
	*************************************************************************/
	RtlInitEmptyUnicodeString(&str1, str1_buf, sizeof(WCHAR) * 256);
	RtlCopyUnicodeString(&str1, &str);
	UNICODE_STRING str2;

	//ƴ���ַ�������Ҫע�ⳤ��
	RtlInitUnicodeString(&str2, L" world!!!");
	NTSTATUS res = RtlAppendUnicodeStringToString(&str1, &str2);
	if (res == STATUS_BUFFER_TOO_SMALL)
		DbgPrint("string too long");
	else
		DbgPrint("string = %wZ", &str1);

	//�ַ����Ƚ�
	/************************************************************************
	* ��������:RtlCompareUnicodeString
	* ��������:UNICODE_STRING�ַ����Ƚ�
	* �����б�:
		String1:Ҫ�Ƚϵĵ�һ���ַ���
		String2:Ҫ�Ƚϵĵڶ����ַ���
		CaseInSensitive:�Ƿ�Դ�Сд����
	* ���� ֵ:�ȽϽ��
		�����������ֵΪ0����ʾ�����ַ������
		���С��0�����ʾ��һ���ַ���С�ڵڶ����ַ���
		�������0�����ʾ��һ���ַ������ڵڶ����ַ���
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

	//UnicodeStringת��ΪAnsiString
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

	//����AnsiString1
	RtlFreeAnsiString(&AnsiString1);


	////��СдתΪ��д
	///************************************************************************
	//* ��������:RtlUpperString
	//* ��������:UNICODE_STRING�ַ���ת���ɴ�д
	//* �����б�:
	//	DestinationString:Ŀ���ַ���
	//	SourceString:Դ�ַ���
	//	AllocateDesttinationString:�Ƿ�ΪĿ���ַ��������ڴ�
	//* ���� ֵ:VOID
	//*************************************************************************/
	//��ʼ�� UnicodeString1
	UNICODE_STRING UnicodeString1;
	UNICODE_STRING UnicodeString2;
	RtlInitUnicodeString(&UnicodeString1, L"Hello World");
	//�仯ǰ
	DbgPrint("UnicodeString1 = %wZ\n", &UnicodeString1);
	//���д
	RtlUpcaseUnicodeString(&UnicodeString2, &UnicodeString1, TRUE);
	//�仯��
	DbgPrint("UnicodeString2 = %wZ\n", &UnicodeString2);
	//���� UnicodeString2��UnicodeString1 �������٣�
	RtlFreeUnicodeString(&UnicodeString2);

	////��̬�����ڴ洢���ַ��������ں��з�ֹ�ڴ�й©��ʹ�����ú��������ڴ沢�ͷ�
	UNICODE_STRING usAllocMem = {0};
	usAllocMem.MaximumLength = 0x1000;
	usAllocMem.Buffer = (PWSTR)ExAllocatePool(PagedPool, 0x1000);//�����ڴ�
	UNICODE_STRING wideString = RTL_CONSTANT_STRING(L"wideString...");
	usAllocMem.Length = (wcslen(L"wideString...") + 1) * sizeof(WCHAR);
	ASSERT(usAllocMem.MaximumLength > usAllocMem.Length); 
	RtlCopyUnicodeString(&usAllocMem, &wideString);

	DbgPrint("usAllocMem = %wZ", &usAllocMem);

	ExFreePool(usAllocMem.Buffer); //�ͷ��ڴ�
	usAllocMem.Buffer = NULL;
	usAllocMem.Length = usAllocMem.MaximumLength = 0;


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