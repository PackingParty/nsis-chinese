﻿/*
 * main.c: executable header main code
 *
 * This file is a part of NSIS.
 *
 * Copyright (C) 1999-2009 Nullsoft and Contributors
 *
 * Licensed under the zlib/libpng license (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Licence details can be found in the file COPYING.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.
 *
 * Unicode support by Jim Park -- 08/22/2007
 */

#include "../Platform.h"
#include <shlobj.h>
#include "resource.h"
#include "util.h"
#include "fileform.h"
#include "state.h"
#include "ui.h"
#include "lang.h"
#include "state.h"
#include "exec.h"
#include "plugin.h"

#if !defined(NSIS_CONFIG_VISIBLE_SUPPORT) && !defined(NSIS_CONFIG_SILENT_SUPPORT)
#error One of NSIS_CONFIG_SILENT_SUPPORT or NSIS_CONFIG_VISIBLE_SUPPORT must be defined.
#endif
#ifdef NSIS_COMPRESS_WHOLE
extern HANDLE dbd_hFile;
#endif

TCHAR g_caption[NSIS_MAX_STRLEN * 2];
#ifdef NSIS_CONFIG_VISIBLE_SUPPORT
HWND g_hwnd;
HANDLE g_hInstance;
#endif

void NSISCALL CleanUp();

TCHAR *ValidateTempDir()
{
    validate_filename(state_temp_dir);
    if (!validpathspec(state_temp_dir))
        return NULL;
    addtrailingslash(state_temp_dir);
    CreateDirectory(state_temp_dir, NULL);
    // state_language is used as a temp var here
    return my_GetTempFileName(state_language, state_temp_dir);
}

void *g_SHGetFolderPath;

int mystrncmp(const TCHAR* a, const TCHAR* b, size_t len)
{
    int rval = 0;
    TCHAR cha, chb;

    do
    {
        cha = *a++;
        chb = *b++;
        --len;

        if (cha > chb)
        {
            rval = 1;
        }
        else if (cha < chb)
        {
            rval = -1;
        }
    } while (rval == 0 && cha != _T('\0') && chb != _T('\0') && len);

    if (len > 0 && rval == 0)
    {
        if (cha != chb)
        {
            rval = cha ? 1 : -1;
        }
    }

    return rval;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPTSTR lpszCmdParam, int nCmdShow)
{
    int ret = 0;
    const TCHAR *m_Err = _LANG_ERRORWRITINGTEMP;

    int cl_flags = 0;

    TCHAR *realcmds;
    TCHAR seekchar = _T(' ');
    TCHAR *cmdline;

    InitCommonControls();

    SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

#if defined(NSIS_SUPPORT_ACTIVEXREG) || defined(NSIS_SUPPORT_CREATESHORTCUT)
    {
        extern HRESULT g_hres;
        g_hres = OleInitialize(NULL);
    }
#endif

    // load shfolder.dll before any script code is executed to avoid
    // weird situations where SetOutPath or even the extraction of 
    // shfolder.dll will cause unexpected behavior.
    //
    // this also prevents the following:
    //
    //  SetOutPath "C:\Program Files\NSIS" # maybe read from reg
    //  File shfolder.dll
    //  Delete $PROGRAMFILES\shfolder.dll # can't be deleted, as the
    //                                    # new shfolder.dll is used
    //                                    # to find its own path.
    g_SHGetFolderPath = myGetProcAddress(MGA_SHGetFolderPath);

    {
        // workaround for bug #1008632
        // http://sourceforge.net/tracker/index.php?func=detail&aid=1008632&group_id=22049&atid=373085
        //
        // without this, SHGetSpecialFolderLocation doesn't always recognize
        // some special folders, like the desktop folder for all users, on
        // Windows 9x. unlike SHGetSpecialFolderPath, which is not available
        // on all versions of Windows, SHGetSpecialFolderLocation doesn't try
        // too hard to make sure the caller gets what he asked for. so we give
        // it a little push in the right direction by doing part of the work
        // for it.
        //
        // part of what SHGetFileInfo does, is to convert a path into an idl.
        // to do this conversion, it first needs to initialize the list of 
        // special idls, which are exactly the idls we use to get the paths
        // of special folders (CSIDL_*).

        SHFILEINFO shfi;
        SHGetFileInfo(_T(""), 0, &shfi, sizeof(SHFILEINFO), 0);
    }

    mystrcpy(g_caption, _LANG_GENERIC_ERROR);

    mystrcpy(state_command_line, GetCommandLine());

#ifdef NSIS_CONFIG_VISIBLE_SUPPORT
    g_hInstance = GetModuleHandle(NULL);
#endif//NSIS_CONFIG_VISIBLE_SUPPORT

    cmdline = state_command_line;
    if (*cmdline == _T('\"')) seekchar = *cmdline++;

    cmdline = findchar(cmdline, seekchar);
    cmdline = CharNext(cmdline);
    realcmds = cmdline;

    while (*cmdline)
    {
        // skip over any spaces
        while (*cmdline == _T(' ')) cmdline++;

        // get char we should look for to get the next parm
        seekchar = _T(' ');
        if (cmdline[0] == _T('\"'))
        {
            cmdline++;
            seekchar = _T('\"');
        }

        // is it a switch?
        if (cmdline[0] == _T('/'))
        {
            cmdline++;

            // this only works with spaces because they have just one bit on
            // Jim Park: this is nice but why would you do that?  Why not just an equality
            // check?  It can't be any faster since you do an equality check also.
            // #define END_OF_ARG(c) (((c)|' ')==' ')
            // Maybe you meant to do this (c | ' ') and since OR is faster than an
            // equality check but optimizing the parsing of the COMMAND LINE ARGS while
            // losing portability and readability is a poor tradeoff.  Do you REALLY
            // have a performance problem with command line parsing?
#define END_OF_ARG(c) (c == _T(' ') || c == _T('\0'))

#if defined(NSIS_CONFIG_VISIBLE_SUPPORT) && defined(NSIS_CONFIG_SILENT_SUPPORT)
            if (cmdline[0] == _T('S') && END_OF_ARG(cmdline[1]))
                cl_flags |= FH_FLAGS_SILENT;
#endif//NSIS_CONFIG_SILENT_SUPPORT && NSIS_CONFIG_VISIBLE_SUPPORT
#ifdef NSIS_CONFIG_CRC_SUPPORT
            //      if (*(LPDWORD)cmdline == CHAR4_TO_DWORD('N','C','R','C') && END_OF_ARG(cmdline[4]))
            // Need my own strcmp?
            if (mystrncmp(cmdline, _T("NCRC"), 4) == 0 && END_OF_ARG(cmdline[4]))
                cl_flags |= FH_FLAGS_NO_CRC;
#endif//NSIS_CONFIG_CRC_SUPPORT

            // if (*(LPDWORD)(cmdline-2) == CHAR4_TO_DWORD(' ', '/', 'D','='))
            if (mystrncmp(cmdline - 2, _T(" /D="), 4) == 0)
            {
                // *(LPDWORD)(cmdline-2)=0; // keep this from being passed to uninstaller if necessary
                memset(cmdline - 2, 0, sizeof(TCHAR) * 4);
                mystrcpy(state_install_directory, cmdline + 2);
                break; // /D= must always be last
            }
        }

        // skip over our parm
        cmdline = findchar(cmdline, seekchar);
        // skip the quote
        if (*cmdline == _T('\"'))
            cmdline++;
    }

    GetTempPath(NSIS_MAX_STRLEN, state_temp_dir);
    if (!ValidateTempDir())
    {
        GetWindowsDirectory(state_temp_dir, NSIS_MAX_STRLEN - 5); // leave space for \Temp
        mystrcat(state_temp_dir, _T("\\Temp"));
        if (!ValidateTempDir())
        {
            goto end;
        }
    }
    DeleteFile(state_language);

    m_Err = loadHeaders(cl_flags);
    if (m_Err) goto end;

#ifdef NSIS_CONFIG_UNINSTALL_SUPPORT
    if (g_is_uninstaller)
    {
        TCHAR *p = findchar(state_command_line, 0);

        // state_command_line has state_install_directory right after it in memory, so reading
        // a bit over state_command_line won't do any harm
        // while (p >= state_command_line && *(LPDWORD)p != CHAR4_TO_DWORD(' ', '_', '?', '=')) p--;
        while (p >= state_command_line &&
            mystrncmp(p, _T(" _?="), 4))
        {
            p--;
        }

        m_Err = _LANG_UNINSTINITERROR;

        if (p >= state_command_line)
        {
            *p = 0; // terminate before "_?="
            p += 4; // skip over " _?="
            if (is_valid_instpath(p))
            {
                mystrcpy(state_install_directory, p);
                mystrcpy(state_output_directory, p);
                m_Err = 0;
            }
            else
            {
                goto end;
            }
        }
        else
        {
            int x;

            mystrcat(state_temp_dir, _T("~nsu.tmp"));

            // check if already running from uninstaller temp dir
            // this prevents recursive uninstaller calls
            if (!lstrcmpi(state_temp_dir, state_exe_directory))
                goto end;

            CreateDirectory(state_temp_dir, NULL);
            SetCurrentDirectory(state_temp_dir);

            if (!state_install_directory[0])
                mystrcpy(state_install_directory, state_exe_directory);

            mystrcpy(g_usrvars[0], realcmds);
            // *(LPWORD)g_usrvars[1] = CHAR2_TO_WORD('A',0);
            mystrcpy(g_usrvars[1], _T("A"));

            for (x = 0; x < 26; x++)
            {
                static TCHAR buf2[NSIS_MAX_STRLEN];

                GetNSISString(buf2, g_header->str_uninstchild); // $TEMP\$1u_.exe

                DeleteFile(buf2); // clean up after all the other ones if they are there

                if (m_Err) // not done yet
                {
                    // copy file
                    if (CopyFile(state_exe_path, buf2, TRUE))
                    {
                        HANDLE hProc;
#ifdef NSIS_SUPPORT_MOVEONREBOOT
                        MoveFileOnReboot(buf2, NULL);
#endif
                        GetNSISString(buf2, g_header->str_uninstcmd); // '"$TEMP\$1u_.exe" $0 _?=$INSTDIR\'
                        hProc = myCreateProcess(buf2);
                        if (hProc)
                        {
                            CloseHandle(hProc);
                            // success
                            m_Err = 0;
                        }
                    }
                }
                g_usrvars[1][0]++;
            }

#ifdef NSIS_SUPPORT_MOVEONREBOOT
            MoveFileOnReboot(state_temp_dir, NULL);
#endif

            goto end;
        }
    }
#endif//NSIS_CONFIG_UNINSTALL_SUPPORT

    g_exec_flags.errlvl = -1;
    ret = ui_doinstall();

#ifdef NSIS_CONFIG_LOG
#if !defined(NSIS_CONFIG_LOG_ODS) && !defined(NSIS_CONFIG_LOG_STDOUT)
    log_write(1);
#endif//!NSIS_CONFIG_LOG_ODS && !NSIS_CONFIG_LOG_STDOUT
#endif//NSIS_CONFIG_LOG
end:

    CleanUp();

#if defined(NSIS_SUPPORT_ACTIVEXREG) || defined(NSIS_SUPPORT_CREATESHORTCUT)
    OleUninitialize();
#endif

    if (m_Err)
    {
        my_MessageBox(m_Err, MB_OK | MB_ICONSTOP | (IDOK << 21));
        ExitProcess(2);
        return 0;
    }

#ifdef NSIS_SUPPORT_REBOOT
    if (g_exec_flags.reboot_called)
    {
        BOOL(WINAPI *OPT)(HANDLE, DWORD, PHANDLE);
        BOOL(WINAPI *LPV)(LPCTSTR, LPCTSTR, PLUID);
        BOOL(WINAPI *ATP)(HANDLE, BOOL, PTOKEN_PRIVILEGES, DWORD, PTOKEN_PRIVILEGES, PDWORD);
        OPT = myGetProcAddress(MGA_OpenProcessToken);
        LPV = myGetProcAddress(MGA_LookupPrivilegeValue);
        ATP = myGetProcAddress(MGA_AdjustTokenPrivileges);
        if (OPT && LPV && ATP)
        {
            HANDLE hToken;
            TOKEN_PRIVILEGES tkp;
            if (OPT(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
            {
                LPV(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
                tkp.PrivilegeCount = 1;
                tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
                ATP(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
            }
        }

        if (!ExitWindowsEx(EWX_REBOOT, 0))
            ExecuteCallbackFunction(CB_ONREBOOTFAILED);
    }
#endif//NSIS_SUPPORT_REBOOT

    if (g_exec_flags.errlvl != -1)
        ret = g_exec_flags.errlvl;

    ExitProcess(ret);
    return 0;
}

void NSISCALL CleanUp()
{
    if (g_db_hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(g_db_hFile);
        g_db_hFile = INVALID_HANDLE_VALUE;
    }
#ifdef NSIS_COMPRESS_WHOLE
    if (dbd_hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(dbd_hFile);
        dbd_hFile = INVALID_HANDLE_VALUE;
    }
#endif
    // Notify plugins that we are about to unload
    Plugins_UnloadAll();
#ifdef NSIS_CONFIG_PLUGIN_SUPPORT
    // Clean up after plug-ins
    myDelete(state_plugins_dir, DEL_DIR | DEL_RECURSE | DEL_REBOOT);
#endif // NSIS_CONFIG_PLUGIN_SUPPORT
}
