/* $Id: ldr.c 51104 2011-03-21 14:16:01Z rharabien $
 *
 * COPYRIGHT: See COPYING in the top level directory
 * PROJECT  : ReactOS user mode libraries
 * MODULE   : kernel32.dll
 * FILE     : reactos/lib/kernel32/misc/ldr.c
 * AUTHOR   : Ariadne
 */

#include <k32.h>

//#define NDEBUG
#include <debug.h>

typedef struct tagLOADPARMS32 {
  LPSTR lpEnvAddress;
  LPSTR lpCmdLine;
  LPSTR lpCmdShow;
  DWORD dwReserved;
} LOADPARMS32;

extern BOOLEAN InWindows;
extern WaitForInputIdleType lpfnGlobalRegisterWaitForInputIdle;

/* FUNCTIONS ****************************************************************/

/**
 * @name GetDllLoadPath
 *
 * Internal function to compute the load path to use for a given dll.
 *
 * @remarks Returned pointer must be freed by caller.
 */

LPWSTR
GetDllLoadPath(LPCWSTR lpModule)
{
	ULONG Pos = 0, Length = 0;
	PWCHAR EnvironmentBufferW = NULL;
	LPCWSTR lpModuleEnd = NULL;
	UNICODE_STRING ModuleName;
	DWORD LastError = GetLastError(); /* GetEnvironmentVariable changes LastError */

	if ((lpModule != NULL) && (wcslen(lpModule) > 2) && (lpModule[1] == ':'))
	{
		lpModuleEnd = lpModule + wcslen(lpModule);
	}
	else
	{
		ModuleName = NtCurrentPeb()->ProcessParameters->ImagePathName;
		lpModule = ModuleName.Buffer;
		lpModuleEnd = lpModule + (ModuleName.Length / sizeof(WCHAR));
	}

	if (lpModule != NULL)
	{
		while (lpModuleEnd > lpModule && *lpModuleEnd != L'/' &&
		       *lpModuleEnd != L'\\' && *lpModuleEnd != L':')
		{
			--lpModuleEnd;
		}
		Length = (lpModuleEnd - lpModule) + 1;
	}

	Length += GetCurrentDirectoryW(0, NULL);
	Length += GetDllDirectoryW(0, NULL);
	Length += GetSystemDirectoryW(NULL, 0);
	Length += GetWindowsDirectoryW(NULL, 0);
	Length += GetEnvironmentVariableW(L"PATH", NULL, 0);

	EnvironmentBufferW = RtlAllocateHeap(RtlGetProcessHeap(), 0,
	                                     Length * sizeof(WCHAR));
	if (EnvironmentBufferW == NULL)
	{
		return NULL;
	}

	if (lpModule)
	{
		RtlCopyMemory(EnvironmentBufferW, lpModule,
		              (lpModuleEnd - lpModule) * sizeof(WCHAR));
		Pos += lpModuleEnd - lpModule;
		EnvironmentBufferW[Pos++] = L';';
	}

	Pos += GetCurrentDirectoryW(Length, EnvironmentBufferW + Pos);
	EnvironmentBufferW[Pos++] = L';';
	Pos += GetDllDirectoryW(Length - Pos, EnvironmentBufferW + Pos);
	EnvironmentBufferW[Pos++] = L';';
	Pos += GetSystemDirectoryW(EnvironmentBufferW + Pos, Length - Pos);
	EnvironmentBufferW[Pos++] = L';';
	Pos += GetWindowsDirectoryW(EnvironmentBufferW + Pos, Length - Pos);
	EnvironmentBufferW[Pos++] = L';';
	Pos += GetEnvironmentVariableW(L"PATH", EnvironmentBufferW + Pos, Length - Pos);

	SetLastError(LastError);
	return EnvironmentBufferW;
}

/*
 * @implemented
 */
BOOL
WINAPI
DisableThreadLibraryCalls(
    IN HMODULE hLibModule)
{
    NTSTATUS Status;

    Status = LdrDisableThreadCalloutsForDll((PVOID)hLibModule);
    if (!NT_SUCCESS(Status))
    {
        BaseSetLastNTError(Status);
        return FALSE;
    }
    return TRUE;
}


/*
 * @implemented
 */
//HINSTANCE
//WINAPI
//LoadLibraryA (
//	LPCSTR	lpLibFileName
//	)
//{
//	return LoadLibraryExA (lpLibFileName, 0, 0);
//}
//
//
///*
// * @implemented
// */
//HINSTANCE
//WINAPI
//LoadLibraryExA(
//    LPCSTR lpLibFileName,
//    HANDLE hFile,
//    DWORD dwFlags)
//{
//   PUNICODE_STRING FileNameW;
//
//    if (!(FileNameW = Basep8BitStringToStaticUnicodeString(lpLibFileName)))
//        return NULL;
//
//    return LoadLibraryExW(FileNameW->Buffer, hFile, dwFlags);
//}
//
//
///*
// * @implemented
// */
//HINSTANCE
//WINAPI
//LoadLibraryW (
//	LPCWSTR	lpLibFileName
//	)
//{
//	return LoadLibraryExW (lpLibFileName, 0, 0);
//}
//
//
//static
//NTSTATUS
//LoadLibraryAsDatafile(PWSTR path, LPCWSTR name, HMODULE* hmod)
//{
//    static const WCHAR dotDLL[] = {'.','d','l','l',0};
//
//    WCHAR filenameW[MAX_PATH];
//    HANDLE hFile = INVALID_HANDLE_VALUE;
//    HANDLE mapping;
//    HMODULE module;
//
//    *hmod = 0;
//
//    if (!SearchPathW( path, name, dotDLL, sizeof(filenameW) / sizeof(filenameW[0]),
//                     filenameW, NULL ))
//    {
//        return NtCurrentTeb()->LastStatusValue;
//    }
//
//    hFile = CreateFileW( filenameW, GENERIC_READ, FILE_SHARE_READ,
//                         NULL, OPEN_EXISTING, 0, 0 );
//
//    if (hFile == INVALID_HANDLE_VALUE) return NtCurrentTeb()->LastStatusValue;
//
//    mapping = CreateFileMappingW( hFile, NULL, PAGE_READONLY, 0, 0, NULL );
//    CloseHandle( hFile );
//    if (!mapping) return NtCurrentTeb()->LastStatusValue;
//
//    module = MapViewOfFile( mapping, FILE_MAP_READ, 0, 0, 0 );
//    CloseHandle( mapping );
//    if (!module) return NtCurrentTeb()->LastStatusValue;
//
//    /* make sure it's a valid PE file */
//    if (!RtlImageNtHeader(module))
//    {
//        UnmapViewOfFile( module );
//        return STATUS_INVALID_IMAGE_FORMAT;
//    }
//    *hmod = (HMODULE)((char *)module + 1);  /* set low bit of handle to indicate datafile module */
//    return STATUS_SUCCESS;
//}
//
//
///*
// * @implemented
// */
//HINSTANCE
//WINAPI
//LoadLibraryExW (
//	LPCWSTR	lpLibFileName,
//	HANDLE	hFile,
//	DWORD	dwFlags
//	)
//{
//	UNICODE_STRING DllName;
//	HINSTANCE hInst;
//	NTSTATUS Status;
//	PWSTR SearchPath;
//    ULONG DllCharacteristics;
//	BOOL FreeString = FALSE;
//
//        (void)hFile;
//
//	if ( lpLibFileName == NULL )
//		return NULL;
//
//    /* Check for any flags LdrLoadDll might be interested in */
//    if (dwFlags & DONT_RESOLVE_DLL_REFERENCES)
//    {
//        /* Tell LDR to treat it as an EXE */
//        DllCharacteristics = IMAGE_FILE_EXECUTABLE_IMAGE;
//    }
//
//	dwFlags &=
//	  DONT_RESOLVE_DLL_REFERENCES |
//	  LOAD_LIBRARY_AS_DATAFILE |
//	  LOAD_WITH_ALTERED_SEARCH_PATH;
//
//	SearchPath = GetDllLoadPath(
//	  dwFlags & LOAD_WITH_ALTERED_SEARCH_PATH ? lpLibFileName : NULL);
//
//	RtlInitUnicodeString(&DllName, (LPWSTR)lpLibFileName);
//
//	if (DllName.Buffer[DllName.Length/sizeof(WCHAR) - 1] == L' ')
//	{
//		RtlCreateUnicodeString(&DllName, (LPWSTR)lpLibFileName);
//		while (DllName.Length > sizeof(WCHAR) &&
//				DllName.Buffer[DllName.Length/sizeof(WCHAR) - 1] == L' ')
//		{
//			DllName.Length -= sizeof(WCHAR);
//		}
//		DllName.Buffer[DllName.Length/sizeof(WCHAR)] = UNICODE_NULL;
//		FreeString = TRUE;
//	}
//
//    if (dwFlags & LOAD_LIBRARY_AS_DATAFILE)
//    {
//        Status = LdrGetDllHandle(SearchPath, NULL, &DllName, (PVOID*)&hInst);
//        if (!NT_SUCCESS(Status))
//        {
//            /* The method in load_library_as_datafile allows searching for the
//             * 'native' libraries only
//             */
//            Status = LoadLibraryAsDatafile(SearchPath, DllName.Buffer, &hInst);
//            goto done;
//        }
//    }
//
//    /* HACK!!! FIXME */
//    if (InWindows)
//    {
//        /* Call the API Properly */
//        Status = LdrLoadDll(SearchPath,
//                            &DllCharacteristics,
//                            &DllName,
//                            (PVOID*)&hInst);
//    }
//    else
//    {
//        /* Call the ROS API. NOTE: Don't fix this, I have a patch to merge later. */
//        Status = LdrLoadDll(SearchPath, &dwFlags, &DllName, (PVOID*)&hInst);
//    }
//
//done:
//	RtlFreeHeap(RtlGetProcessHeap(), 0, SearchPath);
//	if (FreeString)
//		RtlFreeUnicodeString(&DllName);
//	if ( !NT_SUCCESS(Status))
//	{
//		SetLastErrorByStatus (Status);
//		return NULL;
//	}
//
//	return hInst;
//}
//
//
///*
// * @implemented
// */
//FARPROC
//WINAPI
//GetProcAddress( HMODULE hModule, LPCSTR lpProcName )
//{
//	ANSI_STRING ProcedureName;
//	FARPROC fnExp = NULL;
//	NTSTATUS Status;
//
//	if (!hModule)
//	{
//		SetLastError(ERROR_PROC_NOT_FOUND);
//		return NULL;
//	}
//
//	if (HIWORD(lpProcName) != 0)
//	{
//		RtlInitAnsiString (&ProcedureName,
//		                   (LPSTR)lpProcName);
//		Status = LdrGetProcedureAddress ((PVOID)hModule,
//		                        &ProcedureName,
//		                        0,
//		                        (PVOID*)&fnExp);
//	}
//	else
//	{
//		Status = LdrGetProcedureAddress ((PVOID)hModule,
//		                        NULL,
//		                        (ULONG)lpProcName,
//		                        (PVOID*)&fnExp);
//	}
//
//	if (!NT_SUCCESS(Status))
//	{
//		SetLastErrorByStatus(Status);
//		fnExp = NULL;
//	}
//
//	return fnExp;
//}
//
//
///*
// * @implemented
// */
//BOOL WINAPI FreeLibrary(HINSTANCE hLibModule)
//{
//    NTSTATUS Status;
//
//    if (!hLibModule)
//    {
//        SetLastError(ERROR_INVALID_HANDLE);
//        return FALSE;
//    }
//
//    if ((ULONG_PTR)hLibModule & 1)
//    {
//        /* this is a LOAD_LIBRARY_AS_DATAFILE module */
//        char *ptr = (char *)hLibModule - 1;
//        return UnmapViewOfFile(ptr);
//    }
//
//    Status = LdrUnloadDll(hLibModule);
//    if (!NT_SUCCESS(Status))
//    {
//        SetLastErrorByStatus(Status);
//        return FALSE;
//    }
//
//    return TRUE;
//}
//
//
///*
// * @implemented
// */
////VOID
////WINAPI
////FreeLibraryAndExitThread (
////	HMODULE	hLibModule,
////	DWORD	dwExitCode
////	)
////{
////    FreeLibrary(hLibModule);
////    ExitThread(dwExitCode);
////}
//
//
///*
// * @implemented
// */
//DWORD
//WINAPI
//GetModuleFileNameA (
//	HINSTANCE	hModule,
//	LPSTR		lpFilename,
//	DWORD		nSize
//	)
//{
//	ANSI_STRING FileName;
//	PLIST_ENTRY ModuleListHead;
//	PLIST_ENTRY Entry;
//	PLDR_DATA_TABLE_ENTRY Module;
//	PPEB Peb;
//	ULONG Length = 0;
//
//	Peb = NtCurrentPeb ();
//	RtlEnterCriticalSection (Peb->LoaderLock);
//
//	if (hModule == NULL)
//		hModule = Peb->ImageBaseAddress;
//
//	ModuleListHead = &Peb->Ldr->InLoadOrderModuleList;
//	Entry = ModuleListHead->Flink;
//
//	while (Entry != ModuleListHead)
//	{
//		Module = CONTAINING_RECORD(Entry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
//		if (Module->DllBase == (PVOID)hModule)
//		{
//			Length = min(nSize, Module->FullDllName.Length / sizeof(WCHAR));
//			FileName.Length = 0;
//			FileName.MaximumLength = (USHORT)Length * sizeof(WCHAR);
//			FileName.Buffer = lpFilename;
//
//			/* convert unicode string to ansi (or oem) */
//			if (bIsFileApiAnsi)
//				RtlUnicodeStringToAnsiString (&FileName,
//				                              &Module->FullDllName,
//				                              FALSE);
//			else
//				RtlUnicodeStringToOemString (&FileName,
//				                             &Module->FullDllName,
//				                             FALSE);
//
//			if (Length < nSize)
//				lpFilename[Length] = '\0';
//			else
//				SetLastErrorByStatus (STATUS_BUFFER_TOO_SMALL);
//
//			RtlLeaveCriticalSection (Peb->LoaderLock);
//			return Length;
//		}
//
//		Entry = Entry->Flink;
//	}
//
//	SetLastErrorByStatus (STATUS_DLL_NOT_FOUND);
//	RtlLeaveCriticalSection (Peb->LoaderLock);
//
//	return 0;
//}
//
//
///*
// * @implemented
// */
//DWORD
//WINAPI
//GetModuleFileNameW (
//	HINSTANCE	hModule,
//	LPWSTR		lpFilename,
//	DWORD		nSize
//	)
//{
//	UNICODE_STRING FileName;
//	PLIST_ENTRY ModuleListHead;
//	PLIST_ENTRY Entry;
//	PLDR_DATA_TABLE_ENTRY Module;
//	PPEB Peb;
//	ULONG Length = 0;
//
//	Peb = NtCurrentPeb ();
//	RtlEnterCriticalSection (Peb->LoaderLock);
//
//	if (hModule == NULL)
//		hModule = Peb->ImageBaseAddress;
//
//	ModuleListHead = &Peb->Ldr->InLoadOrderModuleList;
//	Entry = ModuleListHead->Flink;
//	while (Entry != ModuleListHead)
//	{
//		Module = CONTAINING_RECORD(Entry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
//
//		if (Module->DllBase == (PVOID)hModule)
//		{
//			Length = min(nSize, Module->FullDllName.Length / sizeof(WCHAR));
//			FileName.Length = 0;
//			FileName.MaximumLength = (USHORT) Length * sizeof(WCHAR);
//			FileName.Buffer = lpFilename;
//
//			RtlCopyUnicodeString (&FileName,
//			                      &Module->FullDllName);
//			if (Length < nSize)
//				lpFilename[Length] = L'\0';
//			else
//				SetLastErrorByStatus (STATUS_BUFFER_TOO_SMALL);
//
//			RtlLeaveCriticalSection (Peb->LoaderLock);
//
//			return Length;
//		}
//
//		Entry = Entry->Flink;
//	}
//
//	SetLastErrorByStatus (STATUS_DLL_NOT_FOUND);
//	RtlLeaveCriticalSection (Peb->LoaderLock);
//
//	return 0;
//}
//
//
///*
// * @implemented
// */
//HMODULE
//WINAPI
//GetModuleHandleA ( LPCSTR lpModuleName )
//{
//	ANSI_STRING ModuleName;
//	NTSTATUS Status;
//	PTEB pTeb = NtCurrentTeb();
//
//	if (lpModuleName == NULL)
//	{
//		return ((HMODULE)pTeb->ProcessEnvironmentBlock->ImageBaseAddress);
//	}
//
//	RtlInitAnsiString(&ModuleName, lpModuleName);
//
//	Status = RtlAnsiStringToUnicodeString(&pTeb->StaticUnicodeString,
//	                                      &ModuleName,
//	                                      FALSE);
//
//	if (NT_SUCCESS(Status))
//	{
//		return GetModuleHandleW(pTeb->StaticUnicodeString.Buffer);
//	}
//
//	SetLastErrorByStatus(Status);
//	return FALSE;
//}
//
//
///*
// * @implemented
// */
//HMODULE
//WINAPI
//GetModuleHandleW (LPCWSTR lpModuleName)
//{
//	UNICODE_STRING ModuleName;
//	PVOID BaseAddress;
//	NTSTATUS Status;
//
//	if (lpModuleName == NULL)
//		return ((HMODULE)NtCurrentPeb()->ImageBaseAddress);
//
//	RtlInitUnicodeString (&ModuleName,
//			      (LPWSTR)lpModuleName);
//
//	Status = LdrGetDllHandle (0,
//				  0,
//				  &ModuleName,
//				  &BaseAddress);
//	if (!NT_SUCCESS(Status))
//	{
//		SetLastErrorByStatus (Status);
//		return NULL;
//	}
//
//	return ((HMODULE)BaseAddress);
//}
//
//
///*
// * @implemented
// */
//BOOL
//WINAPI
//GetModuleHandleExW(IN DWORD dwFlags,
//                   IN LPCWSTR lpModuleName  OPTIONAL,
//                   OUT HMODULE* phModule)
//{
//    HMODULE hModule;
//    NTSTATUS Status;
//    BOOL Ret = FALSE;
//
//    if (phModule == NULL ||
//        ((dwFlags & (GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT)) ==
//         (GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT)))
//    {
//        SetLastError(ERROR_INVALID_PARAMETER);
//        return FALSE;
//    }
//
//    if (lpModuleName == NULL)
//    {
//        hModule = NtCurrentPeb()->ImageBaseAddress;
//    }
//    else
//    {
//        if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)
//        {
//            hModule = (HMODULE)RtlPcToFileHeader((PVOID)lpModuleName,
//                                                 (PVOID*)&hModule);
//            if (hModule == NULL)
//            {
//                SetLastErrorByStatus(STATUS_DLL_NOT_FOUND);
//            }
//        }
//        else
//        {
//            hModule = GetModuleHandleW(lpModuleName);
//        }
//    }
//
//    if (hModule != NULL)
//    {
//        if (!(dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT))
//        {
//            Status = LdrAddRefDll((dwFlags & GET_MODULE_HANDLE_EX_FLAG_PIN) ? LDR_PIN_MODULE : 0,
//                                  hModule);
//
//            if (NT_SUCCESS(Status))
//            {
//                Ret = TRUE;
//            }
//            else
//            {
//                SetLastErrorByStatus(Status);
//                hModule = NULL;
//            }
//        }
//        else
//            Ret = TRUE;
//    }
//
//    *phModule = hModule;
//    return Ret;
//}
//
///*
// * @implemented
// */
//BOOL
//WINAPI
//GetModuleHandleExA(IN DWORD dwFlags,
//                   IN LPCSTR lpModuleName  OPTIONAL,
//                   OUT HMODULE* phModule)
//{
//    ANSI_STRING ModuleName;
//    LPCWSTR lpModuleNameW;
//    NTSTATUS Status;
//    BOOL Ret;
//
//    PTEB pTeb = NtCurrentTeb();
//
//    if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)
//    {
//        lpModuleNameW = (LPCWSTR)lpModuleName;
//    }
//    else
//    {
//        RtlInitAnsiString(&ModuleName, lpModuleName);
//
//        Status = RtlAnsiStringToUnicodeString(&pTeb->StaticUnicodeString,
//                                              &ModuleName,
//                                              FALSE);
//
//        if (!NT_SUCCESS(Status))
//        {
//            SetLastErrorByStatus(Status);
//            return FALSE;
//        }
//
//        lpModuleNameW = pTeb->StaticUnicodeString.Buffer;
//    }
//
//    Ret = GetModuleHandleExW(dwFlags,
//                             lpModuleNameW,
//                             phModule);
//
//    return Ret;
//}

//
///*
// * @implemented
// */
//DWORD
//WINAPI
//LoadModule (
//    LPCSTR  lpModuleName,
//    LPVOID  lpParameterBlock
//    )
//{
//  STARTUPINFOA StartupInfo;
//  PROCESS_INFORMATION ProcessInformation;
//  LOADPARMS32 *LoadParams;
//  char FileName[MAX_PATH];
//  char *CommandLine, *t;
//  BYTE Length;
//
//  LoadParams = (LOADPARMS32*)lpParameterBlock;
//  if(!lpModuleName || !LoadParams || (((WORD*)LoadParams->lpCmdShow)[0] != 2))
//  {
//    /* windows doesn't check parameters, we do */
//    SetLastError(ERROR_INVALID_PARAMETER);
//    return 0;
//  }
//
//  if(!SearchPathA(NULL, lpModuleName, ".exe", MAX_PATH, FileName, NULL) &&
//     !SearchPathA(NULL, lpModuleName, NULL, MAX_PATH, FileName, NULL))
//  {
//    return ERROR_FILE_NOT_FOUND;
//  }
//
//  Length = (BYTE)LoadParams->lpCmdLine[0];
//  if(!(CommandLine = RtlAllocateHeap(RtlGetProcessHeap(), HEAP_ZERO_MEMORY,
//                               strlen(lpModuleName) + Length + 2)))
//  {
//    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
//    return 0;
//  }
//
//  /* Create command line string */
//  strcpy(CommandLine, lpModuleName);
//  t = CommandLine + strlen(CommandLine);
//  *(t++) = ' ';
//  memcpy(t, LoadParams->lpCmdLine + 1, Length);
//
//  /* Build StartupInfo */
//  RtlZeroMemory(&StartupInfo, sizeof(STARTUPINFOA));
//  StartupInfo.cb = sizeof(STARTUPINFOA);
//  StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
//  StartupInfo.wShowWindow = ((WORD*)LoadParams->lpCmdShow)[1];
//
//  if(!CreateProcessA(FileName, CommandLine, NULL, NULL, FALSE, 0, LoadParams->lpEnvAddress,
//                     NULL, &StartupInfo, &ProcessInformation))
//  {
//    DWORD Error;
//
//    RtlFreeHeap(RtlGetProcessHeap(), 0, CommandLine);
//    /* return the right value */
//    Error = GetLastError();
//    switch(Error)
//    {
//      case ERROR_BAD_EXE_FORMAT:
//      {
//        return ERROR_BAD_FORMAT;
//      }
//      case ERROR_FILE_NOT_FOUND:
//      case ERROR_PATH_NOT_FOUND:
//      {
//        return Error;
//      }
//    }
//    return 0;
//  }
//
//  RtlFreeHeap(RtlGetProcessHeap(), 0, CommandLine);
//
//  /* Wait up to 15 seconds for the process to become idle */
//  if (NULL != lpfnGlobalRegisterWaitForInputIdle)
//  {
//    lpfnGlobalRegisterWaitForInputIdle(ProcessInformation.hProcess, 15000);
//  }
//
//  NtClose(ProcessInformation.hThread);
//  NtClose(ProcessInformation.hProcess);
//
//  return 33;
//}

/* EOF */
