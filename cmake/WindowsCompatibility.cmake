# SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
# SPDX-License-Identifier: GPL-2.0-or-later

# This file contains compatibility fixes for Windows platform-specific issues
message(STATUS "Applying Windows compatibility fixes")

# Generate Windows compatibility header
if(WIN32)
  set(WINDOWS_COMPAT_DIR "${CMAKE_BINARY_DIR}/win_compat")
  file(MAKE_DIRECTORY "${WINDOWS_COMPAT_DIR}")
  
  # Create POSIX compatibility header for Windows
  file(WRITE "${WINDOWS_COMPAT_DIR}/posix_compat.h" [[
    // Windows POSIX compatibility layer
    #pragma once

    #ifdef _WIN32
    #include <io.h>
    #include <direct.h>
    #include <windows.h>
    #include <sys/types.h>

    // POSIX function replacements
    #define fseeko _fseeki64
    #define fseeeko64 _fseeki64
    #define ftello _ftelli64
    #define ftello64 _ftelli64
    #define ftruncate _chsize
    #define mkdir(path, mode) _mkdir(path)
    #define usleep(usec) Sleep((usec)/1000)

    // POSIX thread compatibility
    #ifndef PTHREAD_CANCEL_ENABLE
    #define PTHREAD_CANCEL_ENABLE 0
    #endif

    // Define missing POSIX types for Windows
    #ifndef _OFF_T_DEFINED
    typedef long long off_t;
    #define _OFF_T_DEFINED
    #endif

    #ifndef HAVE_OFF64_T
    typedef long long off64_t;
    #define HAVE_OFF64_T 1
    #endif

    // Define ssize_t for Windows
    #ifndef _SSIZE_T_DEFINED
    #ifdef _WIN64
    typedef __int64 ssize_t;
    #else
    typedef int ssize_t;
    #endif
    #define _SSIZE_T_DEFINED
    #endif

    // Provide standard POSIX-style file I/O functions
    #ifndef HAVE_PREAD
    static inline ssize_t _pread(int fd, void *buf, size_t count, off_t offset) {
        HANDLE h = (HANDLE)_get_osfhandle(fd);
        if (h == INVALID_HANDLE_VALUE) {
            return -1;
        }
        
        OVERLAPPED overlapped = {0};
        DWORD bytes_read = 0;
        
        overlapped.Offset = (DWORD)(offset & 0xFFFFFFFF);
        overlapped.OffsetHigh = (DWORD)((offset >> 32) & 0xFFFFFFFF);
        
        if (ReadFile(h, buf, (DWORD)count, &bytes_read, &overlapped)) {
            return bytes_read;
        }
        return -1;
    }
    #define pread _pread
    #define HAVE_PREAD 1
    #endif

    #ifndef HAVE_PWRITE
    static inline ssize_t _pwrite(int fd, const void *buf, size_t count, off_t offset) {
        HANDLE h = (HANDLE)_get_osfhandle(fd);
        if (h == INVALID_HANDLE_VALUE) {
            return -1;
        }
        
        OVERLAPPED overlapped = {0};
        DWORD bytes_written = 0;
        
        overlapped.Offset = (DWORD)(offset & 0xFFFFFFFF);
        overlapped.OffsetHigh = (DWORD)((offset >> 32) & 0xFFFFFFFF);
        
        if (WriteFile(h, buf, (DWORD)count, &bytes_written, &overlapped)) {
            return bytes_written;
        }
        return -1;
    }
    #define pwrite _pwrite
    #define HAVE_PWRITE 1
    #endif

    // Dummy unistd.h for Windows
    #ifndef _UNISTD_H
    #define _UNISTD_H 1
    
    #include <stdlib.h>
    #include <process.h>
    #include <io.h>
    
    #define R_OK    4
    #define W_OK    2
    #define F_OK    0
    #define X_OK    1
    
    #define access _access
    #define dup2 _dup2
    #define execve _execve
    #define ftruncate _chsize
    #define unlink _unlink
    #define fileno _fileno
    #define getcwd _getcwd
    #define chdir _chdir
    #define isatty _isatty
    #define lseek _lseek
    
    // Symbolic constants for file access modes
    #define F_RDLCK    1
    #define F_WRLCK    2
    #define F_UNLCK    3
    
    // file type masks
    #define S_IFMT   _S_IFMT
    #define S_IFDIR  _S_IFDIR
    #define S_IFCHR  _S_IFCHR
    #define S_IFREG  _S_IFREG
    #define S_IREAD  _S_IREAD
    #define S_IWRITE _S_IWRITE
    #define S_IEXEC  _S_IEXEC
    
    #endif // _UNISTD_H
    
    #endif // _WIN32
  ]])
  
  # Add the generated directory as an include directory
  include_directories(BEFORE "${WINDOWS_COMPAT_DIR}")
  
  # Add a compile definition to use our compatibility header
  add_compile_definitions(HAVE_WINDOWS_COMPAT=1)
  
  # Generate config.h with necessary defines for Windows
  file(WRITE "${WINDOWS_COMPAT_DIR}/config.h" [[
    // Windows compatibility config
    #pragma once
    
    #define HAVE_WINDOWS_COMPAT 1
    
    #ifdef _WIN32
    #define HAVE_FSEEKO 1
    #define HAVE_OFF64_T 1
    #define HAVE_PREAD 1
    #define HAVE_PWRITE 1
    #endif
  ]])
endif()