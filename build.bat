@echo off

IF NOT EXIST ./dist mkdir dist
pushd dist
set INCLUDES=/I "W:\CppProject\CppLib\GL\include" /I "W:\CppProject\CppLib\GLFW\include"
set LIBPATH=/LIBPATH:"W:\CppProject\CppLib\GL\lib" /LIBPATH:"W:\CppProject\CppLib\GLFW\lib-vc2019"
set LINKS=glew32s.lib glfw3.lib opengl32.lib gdi32.lib user32.lib shell32.lib

cl.exe %INCLUDES% /MD ../src/main.c /link %LIBPATH% %LINKS% /out:"main.exe"
popd dist

