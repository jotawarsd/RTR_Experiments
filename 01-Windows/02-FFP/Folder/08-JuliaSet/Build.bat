cls

del Julia.exe
del Julia.obj
del Julia.res

cl.exe /c /EHsc Julia.cpp

rc.exe Julia.rc

link.exe Julia.obj Julia.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Julia.exe
