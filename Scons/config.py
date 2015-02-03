# This Python file uses the following encoding: utf-8

Import('defenv')

### 配置选项，这些主要是在输入了scons --help时会显示出来

cfg = Variables()

cfg.Add(
  (
    'NSIS_MAX_STRLEN',
    '定义内置变量和堆栈的最大字符串长度，1024应该都足够了，但如果你的字符串确实超长的，你可能想要修改了。通常这会添加16-32位的内存，因此从1024设置成4096将会增加64KB内存',
    1024
  )
)

cfg.Add(
  (
    'NSIS_MAX_INST_TYPES',
    '定义最大安装类型，注意不要超过32！！',
    32
  )
)

cfg.Add(
  (
    'NSIS_DEFAULT_LANG',
    '定义NSIS使用的默认语言ID，默认值是1033即英语，中文则是2052',
    1033
  )
)

cfg.Add(
  (
    'NSIS_VARS_SECTION',
    '定义包含有运行时变量即内置变量（如$0，$1）信息的PE节的名称，默认值是.ndata',
    '.ndata'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_CONFIG_UNINSTALL_SUPPORT',
    "是否启用卸载功能支持，如果你的安装程序不需要卸载可以关闭它，添加的小于1KB",
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_CONFIG_LICENSEPAGE',
    '是否启用协议页面的支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_CONFIG_COMPONENTPAGE',
    '是否启用安装包可出现一个你可以选择安装什么区段的页面（组件页）的支持，如果禁用了此选项，那么所有区段都被默认安装。',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_CONFIG_COMPONENTPAGE_ALTERNATIVE',
    '启用一个可选/备选的组件页行为。仅在点击复选框本身时切换勾选/不勾选，而不是点击复选框上的文本时切换勾选/不勾选。.onMouseOverSection函数仅在用户选择组件时被调用，而不是在鼠标悬停时被调用。',
    'no'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_CONFIG_SILENT_SUPPORT',
    '是否支持生成完全静默的安装包',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_CONFIG_VISIBLE_SUPPORT',
    '是否支持生成具体可视界面的安装包',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_CONFIG_ENHANCEDUI_SUPPORT',
    '是否启用CreateFont, SetCtlColors, SetBrandingImage, .onGUIInit等命令的支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_CONFIG_COMPRESSION_SUPPORT',
    '是否启用压缩生成的安装包文件的支持（推荐）',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_COMPRESS_BZIP2_SMALLMODE',
    "如果定义了此选项，bzip2在解压缩时使用bzip2的可选/备选的解压方式，这样可以减少运行时的内存使用，但这牺牲了速度（还有文件大小），不推荐",
    'no'
  )
)

cfg.Add(
  (
    'NSIS_COMPRESS_BZIP2_LEVEL',
    'bzip2压缩窗口大小，1-9是合法的。9的话使用内存最多，但是这压缩的会更好些（推荐），1的话使用内存最小，但压缩的就没那么好了',
    9
  )
)


cfg.Add(
  BoolVariable(
    'NSIS_CONFIG_CRC_SUPPORT',
    '是否启用安装包CRC检测，极力推荐！',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_CONFIG_CRC_ANAL',
    '让CRC检测功能特别小心，这也意味着在文件末尾多了一些字节码，或者前面的512字节有变化时会提示出错！如果你是妄想症患者那就启用它吧！ 否则关闭这个比较安全。如果你要对安装包打数字签名，那么最好关闭它',
    'no'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_CONFIG_LOG',
    '是否启用日志功能支持，打开这个选项会增加4KB大小，但是对于调试安装来说这很有用！',
    'no'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_CONFIG_LOG_ODS',
    '让日志使用OutputDebugString输出而不是文件！',
    'no'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_CONFIG_LOG_STDOUT',
    '让日志输出到stdout而不是文件中！',
    'no'
  )
)

cfg.Add(
    BoolVariable(
    'NSIS_CONFIG_LOG_TIMESTAMP',
    '是否在日志文件的每行添加时间戳',
    'no'
    )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_BGBG',
    '是否启用蓝色（好吧，任何颜色都可以）渐变的背景窗口支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_CODECALLBACKS',
    '是否启用安装包代码回调功能，推荐！',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_MOVEONREBOOT',
    '是否启用卸载程序自删除功能（临时目录下）, 还有重启后重命名或删除文件功能',
    'yes'
  )
)

### 指令/命令启用配置项

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_ACTIVEXREG',
    '是否启用activeX插件的注册和反注册支持，还有CallInstDLL',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_INTOPTS',
    '是否启用IntCmp, IntCmpU, IntOp, 和IntFmt命令支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_STROPTS',
    '是否启用StrCmp, StrCpy, 和StrLen, 还有Get*Local命令支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_STACK',
    '是否启用堆栈操作的支持(Push, Pop, Exch)',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_FILEFUNCTIONS',
    '是否启用FileOpen,FileClose, FileSeek, FileRead, 和FileWrite命令支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_FINDFIRST',
    '是否启用FindFirst, FindNext, 和FindClose命令支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_CREATESHORTCUT',
    '是否启用CreateShortCut命令支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_INIFILES',
    '是否启用ReadINIStr和WriteINIStr命令支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_REGISTRYFUNCTIONS',
    '是否启用ReadRegStr, ReadRegDWORD, WriteRegStr等命令支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_COPYFILES',
    '是否启用CopyFiles命令支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_REBOOT',
    '是否启用Reboot, IfRebootFlag, SetRebootFlag命令支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_FNUTIL',
    '是否启用GetFullPathName, GetTempFileName, 和SearchPath命令支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_EXECUTE',
    '是否启用Exec和ExecWait命令支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_SHELLEXECUTE',
    '是否启用ExecShell命令支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_GETDLLVERSION',
    '是否启用GetDLLVersion命令支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_GETFILETIME',
    '是否启用GetFileTime命令支持',
    'yes'
  )
)

cfg.Add(
   BoolVariable(
    'NSIS_SUPPORT_GETFONTVERSION',
    '是否启用GetFontversion命令支持',
    'yes'
  )
)

cfg.Add(
   BoolVariable(
    'NSIS_SUPPORT_GETFONTNAME',
    '是否启用GetFontName命令支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_HWNDS',
    '是否启用FindWindow, SendMessage, 和IsWindow命令支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_ENVIRONMENT',
    '是否启用ReadEnvStr和ExpandEnvStrings命令支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_RMDIR',
    '是否启用RMDir命令支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_FILE',
    '是否启用File命令支持（解压文件）',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_DELETE',
    '是否启用Delete命令支持（删除文件）',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_RENAME',
    '是否启用Rename命令支持（重命名文件）',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_MESSAGEBOX',
    '是否启用MessageBox支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_VERSION_INFO',
    '是否启用安装包版本信息支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_FIX_DEFINES_IN_STRINGS',
    '修正定义里的定义（如${DEFINE1${DEFINE2}}），更好地处理$字符',
    'no'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_SUPPORT_STANDARD_PREDEFINES',
    '是否启用标准预定义：__FILE__, __LINE__, __DATE__, __TIME__ 和 __TIMESTAMP__',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_LOCKWINDOW_SUPPORT',
    '是否启用LockWindow命令支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_CONFIG_PLUGIN_SUPPORT',
    '是否启用插件命令支持',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_FIX_COMMENT_HANDLING',
    '修正注释处理',
    'yes'
  )
)

cfg.Add(
  BoolVariable(
    'NSIS_CONFIG_CONST_DATA_PATH',
    '决定plugins, includes, stubs是否定位为常量路径',
    defenv['PLATFORM'] != 'win32'
  )
)

### Generate help

Help(cfg.GenerateHelpText(defenv))

### Apply configuration

env = Environment()
cfg.Update(env)

def AddValuedDefine(define):
  defenv.Append(NSIS_CPPDEFINES = [(define, env[define])])

def AddBoolDefine(define):
  if env[define]:
    defenv.Append(NSIS_CPPDEFINES = [define])

def AddStringDefine(define):
  defenv.Append(NSIS_CPPDEFINES = [(define, '"%s"' % env[define])])

AddValuedDefine('NSIS_MAX_STRLEN')
AddValuedDefine('NSIS_MAX_INST_TYPES')
AddValuedDefine('NSIS_DEFAULT_LANG')

AddBoolDefine('NSIS_CONFIG_UNINSTALL_SUPPORT')
AddBoolDefine('NSIS_CONFIG_LICENSEPAGE')
AddBoolDefine('NSIS_CONFIG_COMPONENTPAGE')
AddBoolDefine('NSIS_CONFIG_COMPONENTPAGE_ALTERNATIVE')
AddBoolDefine('NSIS_CONFIG_SILENT_SUPPORT')
AddBoolDefine('NSIS_CONFIG_VISIBLE_SUPPORT')
AddBoolDefine('NSIS_CONFIG_ENHANCEDUI_SUPPORT')
AddBoolDefine('NSIS_CONFIG_COMPRESSION_SUPPORT')
AddBoolDefine('NSIS_COMPRESS_BZIP2_SMALLMODE')

AddValuedDefine('NSIS_COMPRESS_BZIP2_LEVEL')

AddBoolDefine('NSIS_CONFIG_CRC_SUPPORT')
AddBoolDefine('NSIS_CONFIG_CRC_ANAL')
AddBoolDefine('NSIS_CONFIG_LOG')
AddBoolDefine('NSIS_CONFIG_LOG_ODS')
AddBoolDefine('NSIS_CONFIG_LOG_STDOUT')
AddBoolDefine('NSIS_CONFIG_LOG_TIMESTAMP')
AddBoolDefine('NSIS_SUPPORT_BGBG')
AddBoolDefine('NSIS_SUPPORT_CODECALLBACKS')
AddBoolDefine('NSIS_SUPPORT_MOVEONREBOOT')
AddBoolDefine('NSIS_SUPPORT_ACTIVEXREG')
AddBoolDefine('NSIS_SUPPORT_INTOPTS')
AddBoolDefine('NSIS_SUPPORT_STROPTS')
AddBoolDefine('NSIS_SUPPORT_STACK')
AddBoolDefine('NSIS_SUPPORT_FILEFUNCTIONS')
AddBoolDefine('NSIS_SUPPORT_FINDFIRST')
AddBoolDefine('NSIS_SUPPORT_CREATESHORTCUT')
AddBoolDefine('NSIS_SUPPORT_INIFILES')
AddBoolDefine('NSIS_SUPPORT_REGISTRYFUNCTIONS')
AddBoolDefine('NSIS_SUPPORT_COPYFILES')
AddBoolDefine('NSIS_SUPPORT_REBOOT')
AddBoolDefine('NSIS_SUPPORT_FNUTIL')
AddBoolDefine('NSIS_SUPPORT_EXECUTE')
AddBoolDefine('NSIS_SUPPORT_SHELLEXECUTE')
AddBoolDefine('NSIS_SUPPORT_GETDLLVERSION')
AddBoolDefine('NSIS_SUPPORT_GETFILETIME')
AddBoolDefine('NSIS_SUPPORT_GETFONTVERSION')
AddBoolDefine('NSIS_SUPPORT_GETFONTNAME')
AddBoolDefine('NSIS_SUPPORT_HWNDS')
AddBoolDefine('NSIS_SUPPORT_ENVIRONMENT')
AddBoolDefine('NSIS_SUPPORT_RMDIR')
AddBoolDefine('NSIS_SUPPORT_FILE')
AddBoolDefine('NSIS_SUPPORT_DELETE')
AddBoolDefine('NSIS_SUPPORT_RENAME')
AddBoolDefine('NSIS_SUPPORT_MESSAGEBOX')
AddBoolDefine('NSIS_SUPPORT_VERSION_INFO')
AddBoolDefine('NSIS_FIX_DEFINES_IN_STRINGS')
AddBoolDefine('NSIS_SUPPORT_STANDARD_PREDEFINES')
AddBoolDefine('NSIS_LOCKWINDOW_SUPPORT')
AddBoolDefine('NSIS_CONFIG_PLUGIN_SUPPORT')
AddBoolDefine('NSIS_FIX_COMMENT_HANDLING')
AddBoolDefine('NSIS_CONFIG_CONST_DATA_PATH')

AddStringDefine('NSIS_VARS_SECTION')
