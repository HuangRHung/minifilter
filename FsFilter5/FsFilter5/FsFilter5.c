/*++

Module Name:

	FsFilter2.c

Abstract:

	This is the main module of the FsFilter2 miniFilter driver.

Environment:

	Kernel mode

--*/

#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#include <ctype.h>


//PFLT_PORT port = NULL;
//PFLT_PORT ClientPort = NULL;
PFLT_FILTER FilterHandle = NULL;
NTSTATUS MiniUnlo(FLT_FILTER_UNLOAD_FLAGS Flags);
FLT_POSTOP_CALLBACK_STATUS MiniPostCreate(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext, FLT_POST_OPERATION_FLAGS Flags);
FLT_PREOP_CALLBACK_STATUS MiniPreCreate(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext);
FLT_PREOP_CALLBACK_STATUS MiniPreWrite(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext);
FLT_PREOP_CALLBACK_STATUS MiniPreSet(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext);
FLT_PREOP_CALLBACK_STATUS MiniPreQuery(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext);
FLT_POSTOP_CALLBACK_STATUS MiniPostSet(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext, FLT_POST_OPERATION_FLAGS Flags);
//PFLT_INSTANCE_QUERY_TEARDOWN_CALLBACK Miniteardown(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags);

NTSTATUS anCondition(WCHAR buffer[]);



const FLT_OPERATION_REGISTRATION Callbacks[] = {

	//{IRP_MJ_CREATE,0,MiniPreCreate,MiniPostCreate},
	{IRP_MJ_SET_INFORMATION,FLTFL_OPERATION_REGISTRATION_SKIP_PAGING_IO,MiniPreSet,NULL},
	//{IRP_MJ_QUERY_INFORMATION,FLTFL_OPERATION_REGISTRATION_SKIP_PAGING_IO,MiniPreQuery,NULL},
	//{IRP_MJ_READ,0,MiniPreRead,NULL},
	//{IRP_MJ_WRITE,0,MiniPreWrite,NULL},
	{ IRP_MJ_OPERATION_END }
};

const FLT_REGISTRATION FilterRegistration = {
	sizeof(FLT_REGISTRATION),				//size
	FLT_REGISTRATION_VERSION,				//version
	0,										//flags
	NULL,									//ContextRegistrarion
	Callbacks,								//Operation callbacks
	MiniUnlo,								//FilterUnload
	NULL,									//InstanceSetup
	NULL,									//InstanceQueryTeardown
	NULL,									//InstanceTeardownStart
	NULL,									//InstanceTeardownComplete
	NULL,									//GenerateFileName
	NULL,									//  GenerateDestinationFileName
	NULL,									//  NormalizeNameComponent

};


NTSTATUS MiniUnlo(FLT_FILTER_UNLOAD_FLAGS Flags)
{

	KdPrint(("driver unload \r\n"));
	//FltCloseCommunicationPort(port);
	FltUnregisterFilter(FilterHandle);

	return STATUS_SUCCESS;
}
FLT_POSTOP_CALLBACK_STATUS MiniPostCreate(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext, FLT_POST_OPERATION_FLAGS Flags)
{
	PFLT_FILE_NAME_INFORMATION FileNameInfo;
	int file_num;
	int i, j = 0;
	NTSTATUS status;
	NTSTATUS rule_check;
	WCHAR Name[200] = { 0 };
	int filetype;
	status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &FileNameInfo);


	if (NT_SUCCESS(status)) {

		status = FltParseFileNameInformation(FileNameInfo); ///////////////將FileNameInfo指向一個unicode string
		if (NT_SUCCESS(status)) {
			if (wcsstr(FileNameInfo->Name.Buffer, L".txt") != NULL) {
				if (FileNameInfo->Name.MaximumLength < 260)
				{
					RtlCopyMemory(Name, FileNameInfo->Name.Buffer, FileNameInfo->Name.MaximumLength);
					KdPrint(("MiniPostCreate : %ws \r\n", Name));
					//FltReleaseFileNameInformation(FileNameInfo);
					//return FLT_PREOP_COMPLETE;

				}
			}
		}
		FltReleaseFileNameInformation(FileNameInfo);
	}
	return FLT_POSTOP_FINISHED_PROCESSING;
}
FLT_POSTOP_CALLBACK_STATUS MiniPostSet(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext, FLT_POST_OPERATION_FLAGS Flags)
{
	KdPrint(("post set is runnung \r\n"));
	return FLT_POSTOP_FINISHED_PROCESSING;
}
/*PFLT_INSTANCE_QUERY_TEARDOWN_CALLBACK Miniteardown(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags)
{
	NTSTATUS status;
	
}*/
FLT_PREOP_CALLBACK_STATUS MiniPreCreate(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext)
{


	PFLT_FILE_NAME_INFORMATION FileNameInfo;
	int file_num;
	int i, j = 0;
	NTSTATUS status;
	NTSTATUS rule_check;
	WCHAR Name[200] = { 0 };
	int filetype;
	status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &FileNameInfo);


	if (NT_SUCCESS(status)) {

		status = FltParseFileNameInformation(FileNameInfo); ///////////////將FileNameInfo指向一個unicode string
		if (NT_SUCCESS(status)) {
			if (wcsstr(FileNameInfo->Name.Buffer, L".txt") != NULL) {
				if (FileNameInfo->Name.MaximumLength < 260)
				{
					RtlCopyMemory(Name, FileNameInfo->Name.Buffer, FileNameInfo->Name.MaximumLength);
					KdPrint(("MiniPreCreate : %ws \r\n", Name));
					//FltReleaseFileNameInformation(FileNameInfo);
					//return FLT_PREOP_COMPLETE;

				}
			}
		}
		FltReleaseFileNameInformation(FileNameInfo);
	}
	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}
FLT_PREOP_CALLBACK_STATUS MiniPreSet(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext)
{
	NTSTATUS status;
	PFLT_FILE_NAME_INFORMATION pOutReNameinfo;

	if (Data->Iopb->Parameters.SetFileInformation.FileInformationClass == FileRenameInformation)
	{
		WCHAR Name[200] = { 0 };
		PFILE_RENAME_INFORMATION renameInfo = Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
		status = FltGetDestinationFileNameInformation(FltObjects->Instance, Data->Iopb->TargetFileObject, renameInfo->RootDirectory, renameInfo->FileName, renameInfo->FileNameLength, FLT_FILE_NAME_NORMALIZED, &pOutReNameinfo);
		if (!NT_SUCCESS(status))
		{
			DbgPrint("FltGetDestinationFileNameInformation is faild! 0x%x", status);
			FltReleaseFileNameInformation(pOutReNameinfo);
			return FLT_PREOP_COMPLETE;

		}
		status = FltParseFileNameInformation(pOutReNameinfo);
		if (NT_SUCCESS(status)) {
			RtlCopyMemory(Name, pOutReNameinfo->Name.Buffer, pOutReNameinfo->Name.MaximumLength);
			KdPrint(("MiniPreSet : %ws \r\n", Name));

		}

		FltReleaseFileNameInformation(pOutReNameinfo);
	}


	return FLT_PREOP_SUCCESS_NO_CALLBACK;


}
FLT_PREOP_CALLBACK_STATUS MiniPreQuery(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext)
{
	NTSTATUS status;
	PFLT_FILE_NAME_INFORMATION pOutReNameinfo;

	if (Data->Iopb->Parameters.SetFileInformation.FileInformationClass == FileRenameInformation)
	{
		WCHAR Name[200] = { 0 };
		PFILE_RENAME_INFORMATION renameInfo = Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
		status = FltGetDestinationFileNameInformation(FltObjects->Instance, Data->Iopb->TargetFileObject, renameInfo->RootDirectory, renameInfo->FileName, renameInfo->FileNameLength, FLT_FILE_NAME_NORMALIZED, &pOutReNameinfo);
		if (!NT_SUCCESS(status))
		{
			DbgPrint("FltGetDestinationFileNameInformation is faild! 0x%x", status);
			FltReleaseFileNameInformation(pOutReNameinfo);
			return FLT_PREOP_COMPLETE;

		}
		status = FltParseFileNameInformation(pOutReNameinfo);
		if (NT_SUCCESS(status)) {
			RtlCopyMemory(Name, pOutReNameinfo->Name.Buffer, pOutReNameinfo->Name.MaximumLength);
			KdPrint(("MiniPreQuery : %ws \r\n", Name));

		}

		FltReleaseFileNameInformation(pOutReNameinfo);
	}


	return FLT_PREOP_SUCCESS_NO_CALLBACK;


}
NTSTATUS anCondition(WCHAR buffer[])
{
	NTSTATUS status;
	if (buffer == NULL)
	{
		status = FALSE;
		return status;
	}
	status = TRUE;
	return status;
}
FLT_PREOP_CALLBACK_STATUS MiniPreWrite(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext)
{
	PFLT_FILE_NAME_INFORMATION FileNameInfo;
	NTSTATUS status;
	WCHAR Name[200] = { 0 };
	status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &FileNameInfo);

	if (NT_SUCCESS(status)) {

		status = FltParseFileNameInformation(FileNameInfo);

		if (NT_SUCCESS(status)) {

			if (FileNameInfo->Name.MaximumLength < 260) {

				RtlCopyMemory(Name, FileNameInfo->Name.Buffer, FileNameInfo->Name.MaximumLength);
				_wcsupr(Name);
				if (wcsstr(Name, L"OPENME.TXT") != NULL) {
					KdPrint(("write file: %ws blocked \r\n", Name));
					Data->IoStatus.Status = STATUS_INVALID_PARAMETER;
					Data->IoStatus.Information = 0;
					FltReleaseFileNameInformation(FileNameInfo);
					return FLT_PREOP_COMPLETE;
				}
				KdPrint(("create file: %ws \r\n", Name));

			}
		}
		FltReleaseFileNameInformation(FileNameInfo);
	}
	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}
/*FLT_PREOP_CALLBACK_STATUS MiniPreRead(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext)
{

}*/
/*NTSTATUS MiniConnect(PFLT_PORT clientport, PVOID serverportcookie, PVOID Context, ULONG size, PVOID Connectioncookie)
{
	ClientPort = clientport;
	KdPrint(("connect \r \n"));
	return STATUS_SUCCESS;
}*/

/*VOID MiniDisconnect(PVOID connectioncookie)
{
	KdPrint(("disconnect \r \n"));
	FltCloseClientPort(FilterHandle, &ClientPort);
}*/

/*NTSTATUS MiniSendRec(PVOID portcookie, PVOID InputBuffer, ULONG InputBufferLength, PVOID OutputBuffer, ULONG OutputBufferLength, PULONG RetLength)
{
	PCHAR msg = "kernel msg";
	KdPrint(("user msg is : %s \r \n", (PCHAR)InputBuffer));

	strcpy((PCHAR)OutputBuffer, msg);

	return STATUS_SUCCESS;
}*/
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;
	//PSECURITY_DESCRIPTOR sd;
	//OBJECT_ATTRIBUTES oa = { 0 };
	//UNICODE_STRING name = RTL_CONSTANT_STRING(L"\\mf");
	/*
	status = FltRegisterFilter(DriverObject, &FilterRegistration, &FilterHandle);

	if (!NT_SUCCESS(status)) {
		return status;
	}
	status = FltBuildDefaultSecurityDescriptor(&sd, FLT_PORT_ALL_ACCESS);

	if (NT_SUCCESS(status)) {

		InitializeObjectAttributes(&oa, &name, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, sd);
		status = FltCreateCommunicationPort(FilterHandle, &port, &oa, NULL, MiniConnect, MiniDisconnect, MiniSendRec, 1);

		FltFreeSecurityDescriptor(sd);
		if (NT_SUCCESS(status)) {
			status = FltStartFiltering(FilterHandle);

			if (NT_SUCCESS(status)) {
				return status;

			}
			FltCloseCommunicationPort(port);
		}

		FltUnregisterFilter(FilterHandle);
	}


	return status;
	*/
	status = FltRegisterFilter(DriverObject, &FilterRegistration, &FilterHandle);


	if (NT_SUCCESS(status)) {

		status = FltStartFiltering(FilterHandle);

		if (!NT_SUCCESS(status)) {

			FltUnregisterFilter(FilterHandle);
		}

	}
	return status;

}

NTSTATUS checkfilenamerule(WCHAR NAMEFILE[], int filenumber)	/////////////////filename 是否符合規定
{
	int i;
	NTSTATUS status;
	for (i = 0; i < filenumber; i++)   ////////先線暫定 前5碼為英文字母且大寫後5碼數字
	{
		if (i >= 0 && i < 5) //前五
		{
			if (NAMEFILE[i] >= L'A'&&NAMEFILE[i] <= L'Z')
			{
				continue;
			}
			else  //前五非服合規定
			{
				status = FALSE;
				return status;
			}
		}
		else
		{
			if (NAMEFILE[i] >= L'0'&&NAMEFILE[i] <= L'9')
			{
				continue;
			}
			else
			{
				status = FALSE;
				return status;
			}
		}
	}
	status = TRUE;
	return status;
}
