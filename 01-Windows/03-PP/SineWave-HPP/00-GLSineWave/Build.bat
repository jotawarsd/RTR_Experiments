cls

del OGL.exe
del OGL.obj
del OGL.res

nvcc -o OGLCU OGL.cu

cl.exe /c /EHsc OGL.cpp /I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.6\include"

rc.exe OGL.rc

link.exe OGL.obj OGL.res OGLCU.lib /LIBPATH:C:\glew\lib\Release\x64 /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.6\lib\x64" user32.lib gdi32.lib /SUBSYSTEM:WINDOWS
OGL.exe