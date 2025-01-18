@echo off

if not exist build mkdir build
pushd build
cl /nologo /FC /std:c11 /W3 /wd4101 /Zi /Zc:preprocessor /D_CRT_SECURE_NO_WARNINGS ..\main.c
popd
