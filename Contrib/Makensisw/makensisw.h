/*
  Copyright (c) 2002 Robert Rainwater
  Contributors: Justin Frankel, Fritz Elfert, Amir Szekely, Sunil Kamath, Joost Verburg

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
  claim that you wrote the original software. If you use this software
  in a product, an acknowledgment in the product documentation would be
  appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
  misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Unicode support by Jim Park -- 08/10/2007
  */
#ifndef MAKENSIS_H
#define MAKENSIS_H

#define _WIN32_IE 0x0400
#include <windows.h>
#include <commctrl.h>
#include "../ExDLL/nsis_tchar.h"
#include "utils.h"
#define _RICHEDIT_VER 0x0200
#include <richedit.h>
#undef _RICHEDIT_VER

// Defines
#define NSIS_URL     "http://nsis.sourceforge.net/"
#define NSIS_FOR     "http://forums.winamp.com/forumdisplay.php?forumid=65"
#define NSIS_UPDATE  "http://nsis.sourceforge.net/update.php?version="
#define NSIS_DL_URL  "http://nsis.sourceforge.net/download/"
#define USAGE        _T("用法：\r\n\r\n - 文件 | 载入脚本...\r\n - 把.nsi文件拖到此窗口\r\n - 在.nsi文件上右击然后选择“编译NSIS脚本”")
#define COPYRIGHT    _T("Copyright ?2002 Robert Rainwater")
#define CONTRIB      _T("Fritz Elfert, Justin Frankel, Amir Szekely, Sunil Kamath, Joost Verburg, (Unicode) Jim Park")
#define DOCPATH      "http://nsis.sourceforge.net/Docs/"
#define LOCALDOCS    _T("\\NSIS.chm")
#define NSISERROR    _T("无法初始化MakeNSIS，请查看makensis.exe是否和makensisw.exe在同一目录中。")
#define DLGERROR     _T("无法初始化MakeNSISW。")
#define SYMBOLSERROR _T("符号不能含有空白字符")
#define MULTIDROPERROR _T("拖入多个脚本目前还不支持")
#define NSISUPDATEPROMPT _T("运行NSIS更新将会关闭MakeNSISW\n是否继续？")
#define REGSEC       HKEY_CURRENT_USER
#define REGSECDEF    HKEY_LOCAL_MACHINE
#define REGKEY       _T("Software\\NSIS")
#define REGLOC       _T("MakeNSISWPlacement")
#define REGCOMPRESSOR _T("MakeNSISWCompressor")
#define REGSYMSUBKEY _T("Symbols")
#define REGMRUSUBKEY _T("MRU")
#define EXENAME      _T("makensis.exe")
#define RAWOPT       _T("/raw") // Sets makensis.exe output in BIN mode for Unicode.
#define MAX_STRING   256
#define TIMEOUT      100
#define MINWIDTH     350
#define MINHEIGHT    180
#define COMPRESSOR_MESSAGE _T("\n\n%s压缩器创建了最小安装包（%d字节）。")
#define RESTORED_COMPRESSOR_MESSAGE _T("\n\n%s压缩器创建了最小安装包（%d字节）。")
#define EXE_HEADER_COMPRESSOR_STAT _T("EXE头部大小:")
#define TOTAL_SIZE_COMPRESSOR_STAT _T("全部大小:")
#define SYMBOL_SET_NAME_MAXLEN 40
#define LOAD_SYMBOL_SET_DLG_NAME _T("载入符号定义设置")
#define SAVE_SYMBOL_SET_DLG_NAME _T("保存符号定义设置")
#define LOAD_BUTTON_TEXT _T("载入")
#define SAVE_BUTTON_TEXT _T("保存")
#define LOAD_SYMBOL_SET_MESSAGE _T("请选择要载入的符号定义设置名称。")
#define SAVE_SYMBOL_SET_MESSAGE _T("请键入或选择要保存的符号定义设置名称。")

#define WM_MAKENSIS_PROCESSCOMPLETE (WM_USER+1001)
#define WM_MAKENSIS_LOADSYMBOLSET (WM_USER+1002)
#define WM_MAKENSIS_SAVESYMBOLSET (WM_USER+1003)

enum {
    MAKENSIS_NOTIFY_SCRIPT,
    MAKENSIS_NOTIFY_WARNING,
    MAKENSIS_NOTIFY_ERROR,
    MAKENSIS_NOTIFY_OUTPUT
};

typedef enum {
    COMPRESSOR_NONE_SELECTED = -1,
    COMPRESSOR_SCRIPT = 0,
    COMPRESSOR_ZLIB,
    COMPRESSOR_ZLIB_SOLID,
    COMPRESSOR_BZIP2,
    COMPRESSOR_BZIP2_SOLID,
    COMPRESSOR_LZMA,
    COMPRESSOR_LZMA_SOLID,
    COMPRESSOR_BEST,
} NCOMPRESSOR;

#ifdef MAKENSISW_CPP
TCHAR *compressor_names[] = { _T(""),
_T("zlib"),
_T("/SOLID zlib"),
_T("bzip2"),
_T("/SOLID bzip2"),
_T("lzma"),
_T("/SOLID lzma"),
_T("Best") };
TCHAR *compressor_display_names[] = { _T("脚本定义/编译器默认"),
_T("ZLIB"),
_T("ZLIB (固实)"),
_T("BZIP2"),
_T("BZIP2 (固实)"),
_T("LZMA"),
_T("LZMA (固实)"),
_T("最佳压缩器") };
WORD compressor_commands[] = { IDM_COMPRESSOR_SCRIPT,
IDM_ZLIB,
IDM_ZLIB_SOLID,
IDM_BZIP2,
IDM_BZIP2_SOLID,
IDM_LZMA,
IDM_LZMA_SOLID,
IDM_BEST };
#endif

#ifdef TOOLBAR_CPP
int compressor_bitmaps[] = { IDB_COMPRESSOR_SCRIPT,
IDB_COMPRESSOR_ZLIB,
IDB_COMPRESSOR_ZLIB,
IDB_COMPRESSOR_BZIP2,
IDB_COMPRESSOR_BZIP2,
IDB_COMPRESSOR_LZMA,
IDB_COMPRESSOR_LZMA,
IDB_COMPRESSOR_BEST };
int compressor_strings[] = { IDS_SCRIPT,
IDS_ZLIB,
IDS_ZLIB_SOLID,
IDS_BZIP2,
IDS_BZIP2_SOLID,
IDS_LZMA,
IDS_LZMA_SOLID,
IDS_BEST };
#endif

// Extern Variables

extern const TCHAR* NSISW_VERSION;

int WINAPI     _tWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, TCHAR *cmdParam, int cmdShow);
BOOL           CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI   MakeNSISProc(LPVOID p);
BOOL CALLBACK  DialogResize(HWND hWnd, LPARAM /* unused*/);
BOOL CALLBACK  AboutNSISProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK  AboutProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK  SettingsProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK  SymbolSetProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK  CompressorProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void           SetScript(const TCHAR *script, bool clearArgs = true);
void           CompileNSISScript();
TCHAR*         BuildSymbols();
void           SetCompressor(NCOMPRESSOR);
void           RestoreSymbols();
void           SaveSymbols();
void           DeleteSymbolSet(TCHAR *);
TCHAR**        LoadSymbolSet(TCHAR *);
void           SaveSymbolSet(TCHAR *, TCHAR **);
void           RestoreMRUList();
void           SaveMRUList();

typedef struct NSISScriptData {
    TCHAR *script;
    HGLOBAL script_cmd_args;
    TCHAR *compile_command;
    TCHAR *output_exe;
    TCHAR *input_script;
    TCHAR *branding;
    TCHAR *brandingv;
    TCHAR **symbols;
    int retcode;
    BOOL userSelectCompressor;
    DWORD logLength;
    DWORD warnings;
    HINSTANCE hInstance;
    HWND hwnd;
    HMENU menu;
    HMENU fileSubmenu;
    HMENU editSubmenu;
    HMENU toolsSubmenu;
    HANDLE thread;
    HANDLE sigint_event;
    HWND focused_hwnd;
    CHARRANGE textrange;
    NCOMPRESSOR default_compressor;
    NCOMPRESSOR compressor;
    TCHAR *compressor_name;
    TCHAR compressor_stats[512];
    TCHAR *best_compressor_name;
    // Added by Darren Owen (DrO) on 1/10/2003
    int recompile_test;
    // Added by Jim Park
    bool isNSISUnicode;
} NSCRIPTDATA;

extern NSCRIPTDATA g_sdata;

typedef struct ResizeData {
    RECT resizeRect;
    RECT griprect;
    int dx;
    int dy;
} NRESIZEDATA;

typedef struct FindReplaceDialog {
    FINDREPLACE fr;
    UINT uFindReplaceMsg;
    HWND hwndFind;
} NFINDREPLACE;

typedef struct ToolTipStruct {
    HWND tip;
    HWND tip_p;
    HHOOK hook;
} NTOOLTIP;

#endif
