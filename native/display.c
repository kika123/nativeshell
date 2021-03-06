/*++

Copyright (c) Alex Ionescu.  All rights reserved.
Copyright (c) 2011 amdf.

    THIS CODE AND INFORMATION IS PROVIDED UNDER THE LESSER GNU PUBLIC LICENSE.
    PLEASE READ THE FILE "COPYING" IN THE TOP LEVEL DIRECTORY.

Module Name:

    display.c

Abstract:

    The Native Command Line Interface (NCLI) is the command shell for the
    TinyKRNL OS.
    This module handles displaying output to screen.

Environment:

    Native mode

Revision History:

    Alex Ionescu - Started Implementation - 01-Mar-06
    Alex Ionescu - Reworked architecture - 23-Mar-06
    amdf - Display buffer extended (was too small) - 22-Feb-11

--*/
#include "precomp.h"

WCHAR DisplayBuffer[1024];
USHORT LinePos = 0;
WCHAR _PutChar[2] = L" ";
UNICODE_STRING CharString = {2, 2, _PutChar};
BOOLEAN ECHO_STATUS = TRUE;
BOOLEAN ERROR_STATUS = TRUE;
/*++
 * @name RtlCliPrintString
 *
 * The RtlCliPrintString routine display a unicode string on the display device
 *
 * @param Message
 *        Pointer to a unicode string containing the message to print.
 *
 * @return STATUS_SUCCESS or failure code.
 *
 * @remarks None.
 *
 *--*/
NTSTATUS
RtlCliPrintString(IN PUNICODE_STRING Message)
{
    ULONG i;
    NTSTATUS Status;

    //
    // Loop every character
    //
    for (i = 0; i < (Message->Length / sizeof(WCHAR)); i++)
    {
        //
        // Print the character
        //
        Status = RtlCliPutChar(Message->Buffer[i]);
    }

    //
    // Return status
    //
    return Status;
}

/*++
 * @name RtlCliPutChar
 *
 * The RtlCliPutChar routine displays a character.
 *
 * @param Char
 *        Character to print out.
 *
 * @return STATUS_SUCCESS or failure code.
 *
 * @remarks None.
 *
 *--*/
NTSTATUS
RtlCliPutChar(IN WCHAR Char)
{
    //
    // Initialize the string
    //
    CharString.Buffer[0] = Char;

    //
    // Check for overflow, or simply update.
    //
#if 0
    if (LinePos++ > 80)
    {
        //
        // We'll be on a new line. Do the math and see how far.
        //
        MessageLength = NewPos - 80;
        LinePos = sizeof(WCHAR);
    }
#endif

    //
    // Make sure that this isn't backspace
    //
    if (Char != '\r')
    {
        //
        // Check if it's a new line
        //
        if (Char == '\n')
        {
            //
            // Reset the display buffer
            //
            LinePos = 0;
            DisplayBuffer[LinePos] = UNICODE_NULL;
        }
        else
        {
            //
            // Add the character in our buffer
            //
            DisplayBuffer[LinePos] = Char;
            LinePos++;
        }
    }

    //
    // Print the character
    //
    return NtDisplayString(&CharString);
}

/*++
 * @name RtlClipBackspace
 *
 * The RtlClipBackspace routine handles a backspace command.
 *
 * @param None.
 *
 * @return STATUS_SUCCESS or failure code if printing failed.
 *
 * @remarks Backspace is handled by printing the previous string minus the last
 *          two characters.
 *
 *--*/
NTSTATUS
RtlClipBackspace(VOID)
{
    UNICODE_STRING BackString;

    //
    // Update the line position
    //
    LinePos--;

    //
    // Finalize this buffer and make it unicode
    //
    DisplayBuffer[LinePos] = ANSI_NULL;
    RtlInitUnicodeString(&BackString, DisplayBuffer);

    //
    // Display the buffer
    //
    return NtDisplayString(&BackString);
}

/*++
 * @name RtlCliDisplayString
 *
 * The RtlCliDisplayString routine FILLMEIN
 *
 * @param Message
 *        FILLMEIN
 *
 * @param ... (Ellipsis)
 *        There is no set number of parameters.
 *
 * @return NTSTATUS
 *
 * @remarks Documentation for this routine needs to be completed.
 *
 *--*/
NTSTATUS
__cdecl
RtlCliDisplayString(IN PCH Message, ...)
{
    va_list MessageList;
    PCHAR MessageBuffer;
    UNICODE_STRING MessageString;
    NTSTATUS Status;

    //
    // Allocate Memory for the String Buffer
    //
    MessageBuffer = RtlAllocateHeap(RtlGetProcessHeap(), 0, 512);

    //
    // First, combine the message
    //
    va_start(MessageList, Message);
    _vsnprintf(MessageBuffer, 512, Message, MessageList);
    va_end(MessageList);

    //
    // Now make it a unicode string
    //
    RtlCreateUnicodeStringFromAsciiz(&MessageString, MessageBuffer);

    //
    // Display it on screen
    //
    Status = RtlCliPrintString(&MessageString);

    //
    // Free Memory
    //
    RtlFreeHeap(RtlGetProcessHeap(), 0, MessageBuffer);
    RtlFreeUnicodeString(&MessageString);

    //
    // Return to the caller
    //
    return Status;
}

/*********************************************************************************************************************/

VOID Printf (char* fmt, ...)
{
   char buffer[512];
   va_list ap;
   UNICODE_STRING UnicodeString;
   ANSI_STRING AnsiString;
   if(ECHO_STATUS){
       va_start(ap, fmt);
       vsprintf(buffer, fmt, ap);
       va_end(ap);

       RtlInitAnsiString (&AnsiString, buffer);
       RtlAnsiStringToUnicodeString (&UnicodeString,
                     &AnsiString,
                     TRUE);
       NtDisplayString(&UnicodeString);
       RtlFreeUnicodeString (&UnicodeString);
   }
}

VOID Print (char* buffer)
{
   UNICODE_STRING UnicodeString;
   ANSI_STRING AnsiString;
   if(ECHO_STATUS){
       RtlInitAnsiString (&AnsiString, buffer);
       RtlAnsiStringToUnicodeString (&UnicodeString,
                     &AnsiString,
                     TRUE);
       NtDisplayString(&UnicodeString);
       RtlFreeUnicodeString (&UnicodeString);
   }
}


VOID PutChar(WCHAR pChar)
{
    WCHAR _PutChar[2] = L" ";
    UNICODE_STRING CharString = {2, 2, _PutChar};
    if(ECHO_STATUS){
        CharString.Buffer[0] = pChar;
        NtDisplayString(&CharString);
    }
}

VOID ErrorPrintf (char* fmt, ...)
{
   char buffer[512];
   va_list ap;
   UNICODE_STRING UnicodeString;
   ANSI_STRING AnsiString;
   if(ERROR_STATUS){
       va_start(ap, fmt);
       vsprintf(buffer, fmt, ap);
       va_end(ap);

       RtlInitAnsiString (&AnsiString, buffer);
       RtlAnsiStringToUnicodeString (&UnicodeString,
                     &AnsiString,
                     TRUE);
       NtDisplayString(&UnicodeString);
       RtlFreeUnicodeString (&UnicodeString);
   }
}

VOID ErrorPrint (char* buffer)
{
   UNICODE_STRING UnicodeString;
   ANSI_STRING AnsiString;
   if(ERROR_STATUS){
       RtlInitAnsiString (&AnsiString, buffer);
       RtlAnsiStringToUnicodeString (&UnicodeString,
                     &AnsiString,
                     TRUE);
       NtDisplayString(&UnicodeString);
       RtlFreeUnicodeString (&UnicodeString);
   }
}

VOID ScreenBufferCat(WCHAR* pBuffer)
{
    int len1 = wcslen(ScreenBuffer);
    int len2 = wcslen(pBuffer);
    if(len1 + len2 > 80)
    {
        wcscpy(ScreenBuffer, (WCHAR*)&pBuffer[len2 - (len1 + len2) % 80]);
    }
    else
    {
        wcscat(ScreenBuffer, pBuffer);
    }
}

VOID BufferPrintf (char* fmt, ...)
{
   char buffer[512];
   va_list ap;
   UNICODE_STRING UnicodeString;
   ANSI_STRING AnsiString;
   if(ECHO_STATUS){
       va_start(ap, fmt);
       vsprintf(buffer, fmt, ap);
       va_end(ap);

       RtlInitAnsiString (&AnsiString, buffer);
       RtlAnsiStringToUnicodeString (&UnicodeString,
                     &AnsiString,
                     TRUE);
       ScreenBufferCat(UnicodeString.Buffer);
       NtDisplayString(&UnicodeString);
       RtlFreeUnicodeString (&UnicodeString);
   }
}

VOID BufferPrint (char* buffer)
{
   UNICODE_STRING UnicodeString;
   ANSI_STRING AnsiString;
   if(ECHO_STATUS)
   {
       RtlInitAnsiString (&AnsiString, buffer);
       RtlAnsiStringToUnicodeString (&UnicodeString,
                     &AnsiString,
                     TRUE);
       ScreenBufferCat(UnicodeString.Buffer);
       NtDisplayString(&UnicodeString);
       RtlFreeUnicodeString (&UnicodeString);
   }
}


VOID BufferPutChar(WCHAR pChar)
{
    WCHAR PutChar[2] = L" ";
    UNICODE_STRING CharString = {2, 2, PutChar};
    if(ECHO_STATUS){
        if(pChar == '\b')
        {
            ScreenBuffer[wcslen(ScreenBuffer)] = '\0';
            CharString.Buffer[0] = '\r';
            NtDisplayString(&CharString);
            RtlInitUnicodeString (&CharString, ScreenBuffer);
            NtDisplayString(&CharString);
        }
        else
        {
            CharString.Buffer[0] = pChar;
            ScreenBufferCat(CharString.Buffer);
            NtDisplayString(&CharString);
        }
    }
}

int
__cdecl
vfprintf(FILE *stream, const char *format, va_list argptr)
{
    Printf((char *)format, argptr);

    return 1;
}


