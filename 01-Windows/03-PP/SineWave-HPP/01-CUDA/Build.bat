cls

del OGL.exe
del OGL.obj
del OGL.res

rc.exe OGL.rc

nvcc -I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.6\include" -L "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.6\lib\x64" user32.lib gdi32.lib OGL.res -L "C:\glew\lib\Release\x64"  -o OGL OGL.cu

OGL.exe
