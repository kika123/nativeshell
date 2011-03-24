/* This file is part of libmspack.
 * (C) 2003-2004 Stuart Caie.
 *
 * libmspack is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License (LGPL) version 2.1
 *
 * For further details, see the file COPYING.LIB distributed with libmspack
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <mspack.h>

#ifndef LARGEFILE_SUPPORT
const char *largefile_msg = "library not compiled to support large files.";
#endif


int mspack_version(int entity)
{
    switch (entity)
    {
    case MSPACK_VER_LIBRARY:
    case MSPACK_VER_SYSTEM:
    case MSPACK_VER_MSCABD:
    case MSPACK_VER_MSCHMD:
    case MSPACK_VER_MSSZDDD:
    case MSPACK_VER_MSKWAJD:
        return 1;
    case MSPACK_VER_MSCABC:
    case MSPACK_VER_MSCHMC:
    case MSPACK_VER_MSLITD:
    case MSPACK_VER_MSLITC:
    case MSPACK_VER_MSHLPD:
    case MSPACK_VER_MSHLPC:
    case MSPACK_VER_MSSZDDC:
    case MSPACK_VER_MSKWAJC:
        return 0;
    }
    return -1;
}

int mspack_sys_selftest_internal(int offt_size)
{
    return (sizeof(off_t) == offt_size) ? MSPACK_ERR_OK : MSPACK_ERR_SEEK;
}

/* validates a system structure */
int mspack_valid_system(struct mspack_system *sys)
{
    return (sys != NULL) && (sys->open != NULL) && (sys->close != NULL) &&
           (sys->read != NULL) && (sys->write != NULL) && (sys->seek != NULL) &&
           (sys->tell != NULL) && (sys->message != NULL) && (sys->alloc != NULL) &&
           (sys->free != NULL) && (sys->copy != NULL) && (sys->null_ptr == NULL);
}

/* returns the length of a file opened for reading */
int mspack_sys_filelen(struct mspack_system *system,
                       struct mspack_file *file, off_t *length)
{
    off_t current;

    if (!system || !file || !length) return MSPACK_ERR_OPEN;

    /* get current offset */
    current = system->tell(file);

    /* seek to end of file */
    if (system->seek(file, (off_t) 0, MSPACK_SYS_SEEK_END))
    {
        return MSPACK_ERR_SEEK;
    }

    /* get offset of end of file */
    *length = system->tell(file);

    /* seek back to original offset */
    if (system->seek(file, current, MSPACK_SYS_SEEK_START))
    {
        return MSPACK_ERR_SEEK;
    }

    return MSPACK_ERR_OK;
}



/* definition of mspack_default_system -- if the library is compiled with
 * MSPACK_NO_DEFAULT_SYSTEM, no default system will be provided. Otherwise,
 * an appropriate default system (e.g. the standard C library, or some native
 * API calls)
 */
//#define MSPACK_NO_DEFAULT_SYSTEM
#ifdef MSPACK_NO_DEFAULT_SYSTEM
struct mspack_system *mspack_default_system = NULL;
#else

/* implementation of mspack_default_system for standard C library */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ntndk.h>
#include <ntddkbd.h>
#include "precomp.h"
struct mspack_file_p
{
    HANDLE fh;
    char *name;
};

static struct mspack_file *msp_open(struct mspack_system *this,
                                    char *filename, int mode)
{
    struct mspack_file_p *fh;
    WCHAR wchr[1024];
    BOOLEAN  status = FALSE , fmode = FALSE;
    HANDLE hFile;
    switch (mode)
    {
    case MSPACK_SYS_OPEN_READ:
        break;
    case MSPACK_SYS_OPEN_WRITE:
        fmode = TRUE ;
        break;
    case MSPACK_SYS_OPEN_UPDATE:
        fmode = TRUE ;
        break;
    case MSPACK_SYS_OPEN_APPEND:
        fmode = TRUE ;
        break;
    default:
        return NULL;
    }
    mbstowcs(wchr , filename , strlen(filename) + 1);
    status = NtFileOpenFile( &hFile , wchr, fmode, FALSE);
    if(!status)
    {
        return NULL;
    }
    if ((fh = RtlAllocateHeap(RtlGetProcessHeap(), 0, sizeof(struct mspack_file_p))))
    {
        fh->name = filename;
        fh->fh = hFile;
        return (struct mspack_file *) fh;
    }
    return NULL;
}

static void msp_close(struct mspack_file *file)
{
    struct mspack_file_p *this = (struct mspack_file_p *) file;
    if (this)
    {
        NtClose(this->fh);
        RtlFreeHeap(RtlGetProcessHeap(), 0, this);
    }
}

static int msp_read(struct mspack_file *file, void *buffer, int bytes)
{
    struct mspack_file_p *this = (struct mspack_file_p *) file;
    IO_STATUS_BLOCK sIoStatus;
    NTSTATUS ntStatus = 0;
    memset(&sIoStatus, 0, sizeof(IO_STATUS_BLOCK));

    ntStatus = NtReadFile( this->fh, NULL, NULL, NULL, &sIoStatus, buffer, bytes, NULL, NULL);
    if (ntStatus == STATUS_SUCCESS)
    {
        return sIoStatus.Information;
    }
    else if(ntStatus == STATUS_END_OF_FILE)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

static int msp_write(struct mspack_file *file, void *buffer, int bytes)
{
    struct mspack_file_p *this = (struct mspack_file_p *) file;
    DWORD count = 0;
    if (this && buffer && bytes >= 0)
    {
        if(NtFileWriteFile(this->fh, buffer, bytes, &count))
        {
            return (int) count;
        }
    }
    return -1;
}

static int msp_seek(struct mspack_file *file, off_t offset, int mode)
{
    struct mspack_file_p *this = (struct mspack_file_p *) file;
    LONGLONG _offset = 0;
    if (this)
    {
        switch (mode)
        {
        case MSPACK_SYS_SEEK_START:
            break;
        case MSPACK_SYS_SEEK_CUR:
            NtFileGetFilePosition(this->fh, &_offset);
            break;
        case MSPACK_SYS_SEEK_END:
            NtFileGetFileSize(this->fh, &_offset);
            break;
        default:
            return -1;
        }
        _offset += offset;
        return !NtFileSeekFile(this->fh, _offset);
    }
    return -1;
}

static off_t msp_tell(struct mspack_file *file)
{
    struct mspack_file_p *this = (struct mspack_file_p *) file;
    LONGLONG offset = 0;
    if(NtFileGetFilePosition(this->fh, &offset))
    {
        return (off_t) offset;
    }
    return 0;
}

static void msp_msg(struct mspack_file *file, const char *format, ...)
{
    size_t size = 512;
    char *buf = RtlAllocateHeap(RtlGetProcessHeap(), 0, size);
    va_list ap;
    va_start(ap, format);
    _vsnprintf (buf, size, format, ap);
    RtlFreeHeap(RtlGetProcessHeap(), 0, buf);
    va_end(ap);
}

static void *msp_alloc(struct mspack_system *this, size_t bytes)
{
    return RtlAllocateHeap(RtlGetProcessHeap(), 0, bytes);
}

static void msp_free(void *buffer)
{
    RtlFreeHeap(RtlGetProcessHeap(), 0, buffer);
}

static void msp_copy(void *src, void *dest, size_t bytes)
{
    memcpy(dest, src, bytes);
}
//////////////////////////////////////////////////////////////
static struct mspack_system msp_system =
{
    &msp_open, &msp_close, &msp_read,  &msp_write, &msp_seek,
    &msp_tell, &msp_msg, &msp_alloc, &msp_free, &msp_copy, NULL
};

struct mspack_system *mspack_default_system = &msp_system;

#endif
