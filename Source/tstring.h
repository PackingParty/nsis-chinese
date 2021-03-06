﻿// tstring.h
//
// This file is a part of Unicode NSIS.
//
// Copyright (C) 2007-2009 Jim Park
//
// Licensed under the zlib/libpng license (the "License");
// you may not use this file except in compliance with the License.
//
// This software is provided 'as-is', without any expressed or implied
// warranty.
//
// Provides TSTRING support.

/*
   Unicode support by Jim Park -- 07/23/2007
   */

#ifndef _TSTRING_
#define _TSTRING_

#include "tchar.h"
#include <string>
#include <windows.h>
#include <fstream>

/* Abstract string type as well. */
#ifdef _UNICODE
typedef std::wstring     tstring;
typedef std::wofstream   tofstream;
typedef std::wifstream   tifstream;

class CEXEBuild;

// Sets the current build object.  This is a work around to avoid having to
// pass in the CEXEBuild object pointer to FileOpenUnicodeText() since that
// would break the function signature it needs to share with fopen().
void SetCurrentCEXEBuild(CEXEBuild* build);

// Use the following macros to open text files.
// #define FOPENTEXT(file, mode) _wfopen(file, mode ## L", ccs=Unicode")
FILE* FileOpenUnicodeText(const TCHAR* file, const TCHAR* mode);
#define FOPENTEXT(file, mode) FileOpenUnicodeText(file, mode)
#else
typedef std::string      tstring;
typedef std::ofstream    tofstream;
typedef std::ifstream    tifstream;
// Use the following macros to open text files.
#define FOPENTEXT(file, mode) fopen(file, mode)
#endif

// This is a helpful little function for converting exceptions or
// other system type things that come back ANSI and must be
// utilized as either ANSI or WCHAR depending on _UNICODE.
class CtoTString
{
public:
#ifdef _UNICODE
    CtoTString(const char* str)
    {
        int len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
        m_wStr = (wchar_t*)GlobalAlloc(GPTR, len*sizeof(wchar_t));
        MultiByteToWideChar(CP_ACP, 0, str, -1, m_wStr, len);
    }

    ~CtoTString() { GlobalFree(m_wStr); m_wStr = 0; }

    wchar_t* tstr() { return m_wStr; }

private:
    wchar_t* m_wStr;
#else
    CtoTString(const char* str)
    {
        m_str = str;
    }
    ~CtoTString() { m_str = 0; }
    const char* tstr() { return m_str; }
private:
    const char* m_str;
#endif
};

// There may be system things that require C strings but we
// may actually have Unicode strings.
class TtoCString
{
public:
#ifdef _UNICODE
    TtoCString(const wchar_t* wStr)
    {
        int len = WideCharToMultiByte(CP_ACP, 0, wStr, -1, NULL, 0, 0, 0);
        m_cStr = (char*)GlobalAlloc(GPTR, len);
        WideCharToMultiByte(CP_ACP, 0, wStr, -1, m_cStr, len, 0, 0);
    }

    ~TtoCString() { GlobalFree(m_cStr); m_cStr = 0; }

    const char* cstr() { return m_cStr; }

private:
    char* m_cStr;
#else
    TtoCString(const char* str)
    {
        m_str = str;
    }
    ~TtoCString() { m_str = 0; }
    const char* cstr() { return m_str; }
private:
    const char* m_str;
#endif
};

#endif
