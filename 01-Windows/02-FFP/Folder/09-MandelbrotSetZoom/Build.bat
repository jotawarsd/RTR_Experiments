cls

del Mandelbrot.exe
del Mandelbrot.obj
del Mandelbrot.res

cl.exe /c /EHsc Mandelbrot.cpp

rc.exe Mandelbrot.rc

link.exe Mandelbrot.obj Mandelbrot.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Mandelbrot.exe
