/*++

Copyright (c) Alex Ionescu.  All rights reserved.
Copyright (c) 2011 amdf.

    THIS CODE AND INFORMATION IS PROVIDED UNDER THE LESSER GNU PUBLIC LICENSE.
    PLEASE READ THE FILE "COPYING" IN THE TOP LEVEL DIRECTORY.

Module Name:

    main.c

Abstract:

    The Native Command Line Interface (NCLI) is the command shell for the
    TinyKRNL OS.
    This module handles the main command line interface and command parsing.

Environment:

    Native mode

Revision History:

    Alex Ionescu - Started Implementation - 01-Mar-06
    Alex Ionescu - Reworked architecture - 23-Mar-06
    amdf         - Added process launch command - 25-Jan-11
    amdf         - Added move command - 20-Feb-11

--*/
#include "precomp.h"
#include "mspack.h"
#include <error.h>
HANDLE hKeyboard;
HANDLE hHeap;
HANDLE hKey;

#define __NCLI_VER__ "0.12 x86"
void CabinetExpand(WCHAR* source, char* files,unsigned int flag , char* target);
void CabinetTest();
WCHAR *helpstr[] =
{
    {
        L"\n"
        L"cd X     - Change directory to X    md X     - Make directory X\n"
        L"copy X Y - Copy file X to Y         poweroff - Power off PC\n"
        L"dir      - Show directory contents  pwd      - Print working directory\n"
        L"del X    - Delete file X            reboot   - Reboot PC\n"
        L"devtree  - Dump device tree         shutdown - Shutdown PC\n"
        L"\x0000"
    },
    {
        L"exit     - Exit shell               sysinfo  - Dump system information\n"
        L"lm       - List modules             vid      - Test screen output\n"
        L"lp       - List processes           move X Y - Move file X to Y\n"
        L"\n"
        L"If a command is not in the list, it is treated as an executable name\n"
        L"\n"
        L"\x0000"
    }
};
/*++
 * @name RtlClipProcessMessage
 *
 * The RtlClipProcessMessage routine
 *
 * @param Command
 *        FILLMEIN
 *
 * @return None.
 *
 * @remarks Documentation for this routine needs to be completed.
 *
 *--*/
VOID
RtlClipProcessMessage(PCHAR Command)
{
    WCHAR CurrentDirectory[MAX_PATH];
    WCHAR buf1[MAX_PATH];
    WCHAR buf2[MAX_PATH];
    //char buff1[MAX_PATH];
    //char buff2[MAX_PATH];
    char *p, *p1, *p2;
    unsigned int flag;
    UNICODE_STRING CurrentDirectoryString;
    CHAR CommandBuf[BUFFER_SIZE];
    //unsigned int k;
    //
    // Copy command line and break it to arguments
    //
    // if xargc = 3, then xargv[1], xargv[2], xargv[3] are available
    // xargv[1] is a command name, xargv[2] is the first parameter

    memset(CommandBuf, 0x00, BUFFER_SIZE);
    strncpy(CommandBuf, Command, strnlen(Command, BUFFER_SIZE));
    StringToArguments(CommandBuf);
    xargc--;//xargc is big than the length of xargv
    //RtlCliDisplayString("Command: %s\n", Command);
    //RtlCliDisplayString("Args: %d\n", xargc);
    //for(k=1;k<=xargc;k++)
    //{
        //RtlCliDisplayString("Arg %d:->%s<-\n", k, xargv[k]);
    //}
    //
    // We'll call the handler for each command
    //
    if (!_strnicmp(Command, "exit", 4))
    {
        //
        // Exit from shell
        //
        DeinitHeapMemory( hHeap );
        NtTerminateProcess(NtCurrentProcess(), 0);
    }
    else if (!_strnicmp(Command, "test", 4))
    {
        UINT i = 0;

        RtlCliDisplayString("Args: %d\n", xargc);
        for (i = 1; i < xargc; i++)
        {
            RtlCliDisplayString("Arg %d: %s\n", i, xargv[i]);
        }
    }
    else if (!_strnicmp(Command, "help", 4))
    {
        RtlCliDisplayString("%S", helpstr[0]);
        RtlCliDisplayString("%S", helpstr[1]);
    }
    else if (!_strnicmp(Command, "lm", 2))
    {
        //
        // List Modules (!lm)
        //
        RtlCliListDrivers();
    }
    else if (!_strnicmp(Command, "lp", 2))
    {
        //
        // List Processes (!lp)
        //
        RtlCliListProcesses();
    }
    else if (!_strnicmp(Command, "sysinfo", 7))
    {
        //
        // Dump System Information (sysinfo)
        //
        RtlCliDumpSysInfo();
    }
    else if (!_strnicmp(Command, "cd", 2))
    {
        //
        // Set the current directory
        //
        RtlCliSetCurrentDirectory(xargv[2]);
    }
    else if (!_strnicmp(Command, "locale", 6))
    {
        //
        // Set the current directory
        //

        NtSetDefaultLocale(TRUE, 1049);
    }
    else if (!_strnicmp(Command, "pwd", 3))
    {
        //
        // Get the current directory
        //
        RtlCliGetCurrentDirectory(CurrentDirectory);

        //
        // Display it
        //
        RtlInitUnicodeString(&CurrentDirectoryString, CurrentDirectory);
        RtlCliPrintString(&CurrentDirectoryString);

        RtlFreeUnicodeString(&CurrentDirectoryString);
    }
    else if (!_strnicmp(Command, "dir", 3))
    {
        //
        // List the current directory
        //
        RtlCliListDirectory();
    }
    else if (!_strnicmp(Command, "devtree", 7))
    {
        //
        // Dump hardware tree
        //
        RtlCliListHardwareTree();
    }
    else if (!_strnicmp(Command, "shutdown", 8))
    {
        RtlCliShutdown();
    }
    else if (!_strnicmp(Command, "reboot", 6))
    {
        RtlCliReboot();
    }
    else if (!_strnicmp(Command, "poweroff", 6))
    {
        RtlCliPowerOff();
    }
    else if (!_strnicmp(Command, "vid", 6))
    {
        UINT j;
        WCHAR i, w;
        UNICODE_STRING us;

        LARGE_INTEGER delay;
        memset(&delay, 0x00, sizeof(LARGE_INTEGER));
        delay.LowPart = 100000000;


        RtlInitUnicodeString(&us, L" ");

        //75x23
        RtlCliDisplayString("\nVid mode is 75x23\n\nCharacter test:");

        j = 0;
        for (w = L'A'; w < 0xFFFF; w++)
        {
            j++;
            NtDelayExecution(FALSE, &delay);
            //w = i;
            if (w != L'\n' && w != L'\r')
            {
                RtlCliPutChar(w);
            }
            else
            {
                RtlCliPutChar(L' ');
            }
            if (j > 70)
            {
                j = 0;
                RtlCliPutChar(L'\n');
            }
        }
    }
    else if (!_strnicmp(Command, "copy", 4))
    {
        // Copy file
        if (xargc > 2)
        {
            GetFullPath(xargv[2], buf1, FALSE);
            GetFullPath(xargv[3], buf2, FALSE);
            RtlCliDisplayString("\nCopy %S to %S\n", buf1, buf2);
            if (FileExists(buf1))
            {
                if (!NtFileCopyFile(buf1, buf2))
                {
                    RtlCliDisplayString("Failed.\n");
                }
            }
            else
            {
                RtlCliDisplayString("File does not exist.\n");
            }
        }
        else
        {
            RtlCliDisplayString("Not enough arguments.\n");
        }
    }
    else if (!_strnicmp(Command, "move", 4))
    {
        // Move/rename file
        if (xargc > 2)
        {
            GetFullPath(xargv[2], buf1, FALSE);
            GetFullPath(xargv[3], buf2, FALSE);
            RtlCliDisplayString("\nMove %S to %S\n", buf1, buf2);
            if (FileExists(buf1))
            {
                if (!NtFileMoveFile(buf1, buf2, FALSE))
                {
                    RtlCliDisplayString("Failed.\n");
                }
            }
            else
            {
                RtlCliDisplayString("File does not exist.\n");
            }
        }
        else
        {
            RtlCliDisplayString("Not enough arguments.\n");
        }
    }
    else if (!_strnicmp(Command, "del", 3))
    {
        // Delete file
        if (xargc > 1)
        {
            GetFullPath(xargv[2], buf1, FALSE);
            if (FileExists(buf1))
            {
                RtlCliDisplayString("\nDelete %S\n", buf1);

                if (!NtFileDeleteFile(buf1))
                {
                    RtlCliDisplayString("Failed.\n");
                }
            }
            else
            {
                RtlCliDisplayString("File does not exist.\n");
            }
        }
        else
        {
            RtlCliDisplayString("Not enough arguments.\n");
        }
    }
    else if (!_strnicmp(Command, "md", 2))
    {
        // Make directory
        if (xargc > 1)
        {
            GetFullPath(xargv[2], buf1, FALSE);

            RtlCliDisplayString("\nCreate directory %S\n", buf1);

            if (!NtFileCreateDirectory(buf1))
            {
                RtlCliDisplayString("Failed.\n");
            }
        }
        else
        {
            RtlCliDisplayString("Not enough arguments.\n");
        }
    }
    else if (!_strnicmp(Command, "expand", 6))
    {
            __asm
            {
                int 3;
            }
        // Expand .cab file
        if (xargc > 2 && xargc <5)
        {
            //GetFullPath(xargv[2], buf1, FALSE);
            //GetFullPath(xargv[3], buf2, FALSE);
            //wcstombs(buff1, buf1, MAX_PATH);
            //wcstombs(buff2, buf2, MAX_PATH);
            //RtlCliDisplayString("\nexpand %S\n", buf1);

            //if (!CabinetExpand(buff1, buff2))
            //{
            //    RtlCliDisplayString("Failed.\n");
            //}
            flag = 1;
            p = NULL;
            p2 = NULL;
            wcscpy(buf1, L"");
            //wcscpy(buf2, L"");
            if(!_strnicmp(xargv[2],"-d",2)){
                flag = 0;
                GetFullPath(xargv[3], buf1, FALSE);
                if(xargc==4 && !_strnicmp(xargv[4],"-f:",3)){
                    p = xargv[4] + 3;
                }
            }else if(!_strnicmp(xargv[2],"-r",2)){
                GetFullPath(xargv[3], buf1, FALSE);
                //GetFullPath((xargc==4)?xargv[4]:"", buf2, FALSE);
                p2 = (xargc==4)?xargv[4]:NULL;
            }else if(xargc==4 && !_strnicmp(xargv[3],"-f:",3)){
                GetFullPath(xargv[2], buf1, FALSE);
                //GetFullPath(xargv[4], buf2, FALSE);
                p2 = xargv[4];
                p = xargv[3] + 3;
            }else if(xargc==3){
                GetFullPath(xargv[2], buf1, FALSE);
                //GetFullPath(xargv[3], buf2, FALSE);
                p2 = xargv[3];
            }else{
                RtlCliDisplayString("Invalid arguments.\n");
            }
            CabinetExpand(buf1, p, flag, p2);
        }
        else if(xargc >5){
            RtlCliDisplayString("Too more arguments.\n");
        }
        else
        {
            RtlCliDisplayString("Not enough arguments.\n");
        }
    }
    else if (!_strnicmp(Command, "cab", 7))
    {
        CabinetTest();
    }
    else
    {
        //
        // Unknown command, try to find an executable and run it.
        // Executable name should be with an .exe extension.
        //
        WCHAR filename[MAX_PATH];
        ANSI_STRING as;
        UNICODE_STRING us;
        HANDLE hProcess;

        GetFullPath(IN xargv[1], OUT filename, FALSE);

        if (FileExists(filename))
        {
            RtlInitAnsiString(&as, Command);
            RtlAnsiStringToUnicodeString(&us, &as, TRUE);

            NtClose(hKeyboard);
            //RtlCliDisplayString("Keyboard is closed\n");

            CreateNativeProcess(filename, us.Buffer, &hProcess);

            RtlFreeAnsiString(&as);
            RtlFreeUnicodeString(&us);

            //RtlCliDisplayString("Waiting for process terminations\n");
            NtWaitForSingleObject(hProcess, FALSE, NULL);

            RtlCliOpenInputDevice(&hKeyboard, KeyboardType);
            //RtlCliDisplayString("Keyboard restored\n");
        }
        else
        {
            RtlCliDisplayString("%s not recognized\n", Command);
        }
    }
}

/*++
 * @name RtlClipDisplayPrompt
 *
 * The RtlClipDisplayPrompt routine
 *
 * @param None.
 *
 * @return None.
 *
 * @remarks Documentation for this routine needs to be completed.
 *
 *--*/
VOID
RtlClipDisplayPrompt(VOID)
{
    WCHAR CurrentDirectory[MAX_PATH];
    ULONG DirSize;
    UNICODE_STRING DirString;

    //
    // Get the current directory
    //
    DirSize = RtlCliGetCurrentDirectory(CurrentDirectory) / sizeof(WCHAR);

    //
    // Display it
    //
    CurrentDirectory[DirSize] = L'>';
    CurrentDirectory[DirSize + 1] = UNICODE_NULL;
    RtlInitUnicodeString(&DirString, CurrentDirectory);
    RtlCliPrintString(&DirString);
}

/*++
 * @name main
 *
 * The main routine
 *
 * @param argc
 *        FILLMEIN
 *
 * @param argv[]
 *        FILLMEIN
 *
 * @param envp[]
 *        FILLMEIN
 *
 * @param DebugFlag
 *        FILLMEIN
 *
 * @return NTSTATUS
 *
 * @remarks Documentation for this routine needs to be completed.
 *
 *--*/
unsigned int test_count = 0;
#define TEST(x) do {                                                            \
  test_count++;                                                                 \
    if (!(x)) {                                                                 \
        /*RtlCliDisplayString("%s:%d FAILED %s\n",__FUNCTION__,__LINE__,#x); */ \
        DbgPrint("\n!!!%s:%d -> %s\n",__FUNCTION__,__LINE__,#x);     \
    } else{                                                                     \
        /*DbgPrint("\n>>>%s:%d -> %s\n",__FUNCTION__,__LINE__,#x);     */\
    }                                                                           \
} while (0)

/* open where cab file doesn't exist */
void cabd_open_test_01()
{
    struct mscab_decompressor *cabd;
    struct mscabd_cabinet *cab;

    cabd = mspack_create_cab_decompressor(NULL);
    TEST(cabd != NULL);

    cab = cabd->open(cabd, "!!!FILE_WHICH_DOES_NOT_EXIST");
    TEST(cab == NULL);
    TEST(cabd->last_error(cabd) == MSPACK_ERR_OPEN);

    mspack_destroy_cab_decompressor(cabd);
}

/* normal cab file with 2 files and one folder.
 * check ALL headers are read correctly */
void cabd_open_test_02()
{
    struct mscab_decompressor *cabd;
    struct mscabd_cabinet *cab;
    struct mscabd_folder *folder;
    struct mscabd_file *file;

    cabd = mspack_create_cab_decompressor(NULL);
    TEST(cabd != NULL);

    cab = cabd->open(cabd, "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\normal_2files_1folder.cab");
    TEST(cab != NULL);

    TEST(cab->next == NULL);
    TEST(cab->base_offset == 0);
    TEST(cab->length == 253);
    TEST(cab->prevcab == NULL);
    TEST(cab->nextcab == NULL);
    TEST(cab->prevname == NULL);
    TEST(cab->nextname == NULL);
    TEST(cab->previnfo == NULL);
    TEST(cab->nextinfo == NULL);
    TEST(cab->set_id = 1570);
    TEST(cab->set_index == 0);
    TEST(cab->header_resv == 0);
    TEST(cab->flags == 0);

    folder = cab->folders;
    TEST(folder != NULL);
    TEST(folder->next == NULL);
    TEST(folder->comp_type == 0);
    TEST(folder->num_blocks == 1);

    file = cab->files;
    TEST(file != NULL);
    TEST(strcmp(file->filename, "hello.c") == 0);
    TEST(file->length == 77);
    TEST(file->attribs == 0x20);
    TEST(file->time_h == 11);
    TEST(file->time_m == 13);
    TEST(file->time_s == 52);
    TEST(file->date_d == 12);
    TEST(file->date_m == 3);
    TEST(file->date_y == 1997);
    TEST(file->folder == folder);
    TEST(file->offset == 0);

    file = file->next;
    TEST(file != NULL);
    TEST(strcmp(file->filename, "welcome.c") == 0);
    TEST(file->length == 74);
    TEST(file->attribs == 0x20);
    TEST(file->time_h == 11);
    TEST(file->time_m == 15);
    TEST(file->time_s == 14);
    TEST(file->date_d == 12);
    TEST(file->date_m == 3);
    TEST(file->date_y == 1997);
    TEST(file->folder == folder);
    TEST(file->offset == 77);

    cabd->close(cabd, cab);
    mspack_destroy_cab_decompressor(cabd);
}

/* cabs with reserve headers set, ensure they all load correctly */
void cabd_open_test_03()
{
    struct mscab_decompressor *cabd;
    struct mscabd_cabinet *cab;
    unsigned int i;
    char *files[] =
    {
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\reserve_---.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\reserve_--D.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\reserve_-F-.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\reserve_-FD.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\reserve_H--.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\reserve_H-D.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\reserve_HF-.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\reserve_HFD.cab"
    };

    cabd = mspack_create_cab_decompressor(NULL);
    TEST(cabd != NULL);

    for (i = 0; i < (sizeof(files) / sizeof(char *)); i++)
    {
        cab = cabd->open(cabd, files[i]);
        TEST(cab != NULL);
        TEST(cab->files != NULL);
        TEST(cab->files->next != NULL);
        TEST(strcmp(cab->files->filename, "test1.txt") == 0);
        TEST(strcmp(cab->files->next->filename, "test2.txt") == 0);
        cabd->close(cabd, cab);
    }

    mspack_destroy_cab_decompressor(cabd);
}

/* some bad cabs, should not load */
void cabd_open_test_04()
{
    struct mscab_decompressor *cabd;
    struct mscabd_cabinet *cab;

    cabd = mspack_create_cab_decompressor(NULL);
    TEST(cabd != NULL);

    /* cab has enough data for a header, but does not contain real cab data
     * result should be MSPACK_ERR_SIGNATURE */
    cab = cabd->open(cabd, "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\bad_signature.cab");
    TEST(cab == NULL);

    /* cab has 0 folders */
    cab = cabd->open(cabd, "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\bad_nofolders.cab");
    TEST(cab == NULL);

    /* cab has 0 files */
    cab = cabd->open(cabd, "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\bad_nofiles.cab");
    TEST(cab == NULL);

    /* second file in the cab has a folder index for a non-existant folder */
    cab = cabd->open(cabd, "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\bad_folderindex.cab");
    TEST(cab == NULL);

    mspack_destroy_cab_decompressor(cabd);
}

/* cabs which have been cut short
 * result should be MSPACK_ERR_READ for missing headers or
 * MSPACK_ERR_DATAFORMAT for missing/partial strings.
 * If only data blocks are missing, the cab should open()
 */
void cabd_open_test_05()
{
    struct mscab_decompressor *cabd;
    struct mscabd_cabinet *cab;
    unsigned int i;
    char *files[] =
    {
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\partial_shortheader.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\partial_shortextheader.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\partial_nofolder.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\partial_shortfolder.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\partial_nofiles.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\partial_shortfile1.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\partial_shortfile2.cab"
    };
    char *str_files[] =
    {
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\partial_str_nopname.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\partial_str_shortpname.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\partial_str_nopinfo.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\partial_str_shortpinfo.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\partial_str_nonname.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\partial_str_shortnname.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\partial_str_noninfo.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\partial_str_shortninfo.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\partial_str_nofname.cab",
        "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\partial_str_shortfname.cab",
    };

    cabd = mspack_create_cab_decompressor(NULL);
    TEST(cabd != NULL);

    for (i = 0; i < (sizeof(files) / sizeof(char *)); i++)
    {
        cab = cabd->open(cabd, files[i]);
        TEST(cab == NULL);
        TEST(cabd->last_error(cabd) == MSPACK_ERR_READ);
    }

    for (i = 0; i < (sizeof(str_files) / sizeof(char *)); i++)
    {
        //DbgPrint("\n===%s\n",str_files[i]);
        cab = cabd->open(cabd, str_files[i]);
        TEST(cab == NULL);
        //DbgPrint("\n===%d\n",cabd->last_error(cabd));
        TEST(cabd->last_error(cabd) == MSPACK_ERR_DATAFORMAT);
    }

    /* lack of data blocks should NOT be a problem for merely reading */
    cab = cabd->open(cabd, "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\partial_nodata.cab");
    TEST(cab != NULL);

    cabd->close(cabd, cab);
    mspack_destroy_cab_decompressor(cabd);
}

/* open cab with 255 character filename (maximum allowed) */
void cabd_open_test_06()
{
    struct mscab_decompressor *cabd;
    struct mscabd_cabinet *cab;

    cabd = mspack_create_cab_decompressor(NULL);
    TEST(cabd != NULL);

    cab = cabd->open(cabd, "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\normal_255c_filename.cab");
    TEST(cab != NULL);

    cabd->close(cabd, cab);
    mspack_destroy_cab_decompressor(cabd);
}


/* open where search file doesn't exist */
void cabd_search_test_01()
{
    struct mscab_decompressor *cabd;
    struct mscabd_cabinet *cab;

    cabd = mspack_create_cab_decompressor(NULL);
    TEST(cabd != NULL);

    cab = cabd->search(cabd, "!!!FILE_WHICH_DOES_NOT_EXIST");
    TEST(cab == NULL);
    TEST(cabd->last_error(cabd) == MSPACK_ERR_OPEN);

    mspack_destroy_cab_decompressor(cabd);
}

/* search file using 1-byte buffer */
void cabd_search_test_02()
{
    struct mscab_decompressor *cabd;
    struct mscabd_cabinet *cab;

    cabd = mspack_create_cab_decompressor(NULL);
    TEST(cabd != NULL);

    cabd->set_param(cabd, MSCABD_PARAM_SEARCHBUF, 1);
    cab = cabd->search(cabd, "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\search_basic.cab");
    cabd->set_param(cabd, MSCABD_PARAM_SEARCHBUF, 32768);

    TEST(cab != NULL);
    TEST(cab->files != NULL);
    TEST(cab->base_offset == 6);
    TEST(cab->files->next != NULL);
    TEST(strcmp(cab->files->filename, "hello.c") == 0);
    TEST(strcmp(cab->files->next->filename, "welcome.c") == 0);

    TEST(cab->next != NULL);
    TEST(cab->next->base_offset == 265);
    TEST(cab->next->files != NULL);
    TEST(cab->next->files->next != NULL);
    TEST(strcmp(cab->next->files->filename, "hello.c") == 0);
    TEST(strcmp(cab->next->files->next->filename, "welcome.c") == 0);

    TEST(cab->next->next == NULL);

    cabd->close(cabd, cab);
    mspack_destroy_cab_decompressor(cabd);
}

/* tricky searches */
void cabd_search_test_03()
{
    struct mscab_decompressor *cabd;
    struct mscabd_cabinet *cab;

    cabd = mspack_create_cab_decompressor(NULL);
    TEST(cabd != NULL);

    /* there is only ONE cab in this file. it is prepended by 4 bytes, "MSCF"
     * (heh) and reserved fields in the real cab are filled in so the fake one
     * looks real to the scanner but not the real reader
     */
    cab = cabd->search(cabd, "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\search_tricky1.cab");
    TEST(cab != NULL);
    TEST(cab->next == NULL);
    TEST(cab->files != NULL);
    TEST(cab->base_offset == 4);
    TEST(cab->files->next != NULL);
    TEST(strcmp(cab->files->filename, "hello.c") == 0);
    TEST(strcmp(cab->files->next->filename, "welcome.c") == 0);

    cabd->close(cabd, cab);
    mspack_destroy_cab_decompressor(cabd);
}

/* basic parameter failures */
void cabd_merge_test_01()
{
    struct mscab_decompressor *cabd;
    struct mscabd_cabinet *cab1, *cab2;

    cabd = mspack_create_cab_decompressor(NULL);
    TEST(cabd != NULL);

    cab1 = cabd->open(cabd, "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\multi_basic_pt1.cab");
    cab2 = cabd->open(cabd, "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\multi_basic_pt2.cab");
    TEST(cab1 != NULL);
    TEST(cab2 != NULL);
    TEST(cabd->append(cabd,  cab1, NULL) != MSPACK_ERR_OK);
    TEST(cabd->append(cabd,  NULL, cab1) != MSPACK_ERR_OK);
    TEST(cabd->append(cabd,  cab1, cab1) != MSPACK_ERR_OK);
    TEST(cabd->prepend(cabd, cab1, NULL) != MSPACK_ERR_OK);
    TEST(cabd->prepend(cabd, NULL, cab1) != MSPACK_ERR_OK);
    TEST(cabd->prepend(cabd, cab1, cab1) != MSPACK_ERR_OK);

    /* merge cabs, then try merging again every other way */
    TEST(cabd->append(cabd,  cab1, cab2) == MSPACK_ERR_OK);
    TEST(cabd->append(cabd,  cab2, cab1) != MSPACK_ERR_OK);
    TEST(cabd->prepend(cabd, cab1, cab2) != MSPACK_ERR_OK);
    TEST(cabd->prepend(cabd, cab2, cab1) != MSPACK_ERR_OK);
    TEST(cabd->append(cabd,  cab1, cab2) != MSPACK_ERR_OK);

    cabd->close(cabd, cab1);
    mspack_destroy_cab_decompressor(cabd);
}

/* test merging a normal 5 part single folder cabinet set with slightly
 * haphazard ordering.  should still merge fine */
void cabd_merge_test_02()
{
    struct mscab_decompressor *cabd;
    struct mscabd_cabinet *cab[5];

    cabd = mspack_create_cab_decompressor(NULL);
    TEST(cabd != NULL);

    cab[0] = cabd->open(cabd, "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\multi_basic_pt1.cab");
    cab[1] = cabd->open(cabd, "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\multi_basic_pt2.cab");
    cab[2] = cabd->open(cabd, "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\multi_basic_pt3.cab");
    cab[3] = cabd->open(cabd, "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\multi_basic_pt4.cab");
    cab[4] = cabd->open(cabd, "X:\\WXPE\\SYSTEM32\\test_files\\cabd\\multi_basic_pt5.cab");
    TEST(cab[0] != NULL);
    TEST(cab[1] != NULL);
    TEST(cab[2] != NULL);
    TEST(cab[3] != NULL);
    TEST(cab[4] != NULL);
    TEST(cabd->append(cabd,  cab[0], cab[1]) == MSPACK_ERR_OK);
    TEST(cabd->prepend(cabd, cab[2], cab[1]) == MSPACK_ERR_OK);
    TEST(cabd->append(cabd,  cab[3], cab[4]) == MSPACK_ERR_OK);
    TEST(cabd->prepend(cabd, cab[3], cab[2]) == MSPACK_ERR_OK);

    TEST(cab[0]->files != NULL);
    TEST(cab[0]->files->next != NULL);
    TEST(cab[0]->files->next->next != NULL);
    TEST(cab[0]->files->next->next->next == NULL);
    TEST(cab[0]->files == cab[1]->files);
    TEST(cab[1]->files == cab[2]->files);
    TEST(cab[2]->files == cab[3]->files);
    TEST(cab[3]->files == cab[4]->files);

    TEST(cab[0]->folders != NULL);
    TEST(cab[0]->folders->next == NULL);
    TEST(cab[0]->folders == cab[1]->folders);
    TEST(cab[1]->folders == cab[2]->folders);
    TEST(cab[2]->folders == cab[3]->folders);
    TEST(cab[3]->folders == cab[4]->folders);

    cabd->close(cabd, cab[0]);
    mspack_destroy_cab_decompressor(cabd);
}
void CabinetExpand(WCHAR* source, char* files, unsigned int flag , char* target)
{
    struct mscab_decompressor *cabd;
    struct mscabd_cabinet *cab;
    struct mscabd_file *file;
    WCHAR Buf1[MAX_PATH];
    char buf1[MAX_PATH];
    char buf2[MAX_PATH];
    char buf3[MAX_PATH];
    int count_ok=0, count_err=0, count=0;
    int err;
    /* if self-test reveals an error */
    MSPACK_SYS_SELFTEST(err);
    if (err) return ;
    GetFullPath(target?target:"",Buf1,FALSE);
    if (Buf1[wcslen(Buf1) - 1] != L'\\')
    {
      wcscat(Buf1, L"\\");
    }
    wcstombs(buf1, source, MAX_PATH);
    wcstombs(buf2, Buf1, MAX_PATH);
    if ((cabd = mspack_create_cab_decompressor(NULL)))
    {
        if ((cab = cabd->open(cabd, buf1)))
        {
            if(flag){
                /* for all files */
                for (file = cab->files; file; file = file->next,count++)
                {
                    strcpy(buf3,buf2);
                    strcat(buf3,file->filename);
                    if (cabd->extract(cabd, file, buf3) == MSPACK_ERR_OK)
                    {
                        count_ok++;
                        RtlCliDisplayString("%s\n", file->filename);
                    }
                    else
                    {
                        count_err++;
                        RtlCliDisplayString("%s expand error\n", file->filename);
                    }
                }
                RtlCliDisplayString("%d files found, %d files expanded, %d files error.\n", count, count_ok, count_err);
            }
            cabd->close(cabd, cab);
        }
        else
        {
            RtlCliDisplayString("can't open cabinet (%d)\n", cabd->last_error(cabd));
        }
        mspack_destroy_cab_decompressor(cabd);
    }
    else
    {
        RtlCliDisplayString("can't make decompressor\n");
    }
    return ;
}
int selftest;
void CabinetTest()
{
    __asm
    {
        int 3;
    }


    MSPACK_SYS_SELFTEST(selftest);
    TEST(selftest == MSPACK_ERR_OK);

    cabd_open_test_01();
    cabd_open_test_02();
    cabd_open_test_03();
    cabd_open_test_04();
    cabd_open_test_05();
    cabd_open_test_06();

    cabd_search_test_01();
    cabd_search_test_02();
    cabd_search_test_03();

    cabd_merge_test_01();
    cabd_merge_test_02();

}
NTSTATUS
__cdecl
main(INT argc,
     PCHAR argv[],
     PCHAR envp[],
     ULONG DebugFlag OPTIONAL)
{
    PPEB Peb = NtCurrentPeb();
    NTSTATUS Status;
    PCHAR Command;
    hHeap = InitHeapMemory();
    hKey = NULL;

    //
    // Show banner
    //
    RtlCliDisplayString("Native Shell [Version " __NCLI_VER__ "] (" __DATE__ " " __TIME__ ")\n");
    RtlCliDisplayString("(C) Copyright 2010-2011 amdf\n");
    RtlCliDisplayString("(C) Copyright 2006 TinyKRNL Project\n\n");
    RtlCliDisplayString("Type \"help\".\n\n");
    DbgPrint("************Native Shell START************\n");
    // Setup keyboard input
    //
    Status = RtlCliOpenInputDevice(&hKeyboard, KeyboardType);

    //
    // Show initial prompt
    //
    RtlClipDisplayPrompt();

    //
    // Wait for a new line
    //
    while (TRUE)
    {
        //
        // Get the line that was entered and display a new line
        //
        Command = RtlCliGetLine(hKeyboard);
        RtlCliDisplayString("\n");

        //
        // Make sure there's actually a command
        //
        if (*Command)
        {
            //
            // Process the command and do a new line again.
            //
            RtlClipProcessMessage(Command);
            RtlCliDisplayString("\n");
        }

        //
        // Display the prompt, and restart the loop
        //
        RtlClipDisplayPrompt();
        continue;
    }

    DeinitHeapMemory( hHeap );
    NtTerminateProcess( NtCurrentProcess(), 0 );

    //
    // Return
    //
    return STATUS_SUCCESS;
}
