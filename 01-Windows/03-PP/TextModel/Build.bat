cls

del OGL.exe
del OGL.obj
del OGL.res

cl.exe /c /EHsc OGL.cpp MeshLoader.cpp noise.cpp

rc.exe OGL.rc

link.exe OGL.obj OGL.res MeshLoader.obj noise.obj /LIBPATH:C:\glew\lib\Release\x64 user32.lib gdi32.lib /SUBSYSTEM:WINDOWS
OGL.exe