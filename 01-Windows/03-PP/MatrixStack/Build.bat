cls

del OGL.exe
del OGL.obj
del OGL.res

cl.exe /c /EHsc OGL.cpp ArrayStack.cpp

rc.exe OGL.rc

link.exe OGL.obj OGL.res ArrayStack.obj /LIBPATH:C:\glew\lib\Release\x64 user32.lib gdi32.lib /SUBSYSTEM:WINDOWS
OGL.exe