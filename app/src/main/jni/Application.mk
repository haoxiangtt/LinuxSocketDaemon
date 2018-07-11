APP_STL := gnustl_static #使用STL库
APP_CPPFLAGS += -fpermissive #兼容老的语法，使一些错误降为警告
APP_ABI := all   ## 表示生成所有平台的动态库。