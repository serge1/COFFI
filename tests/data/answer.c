/*
This source code produduces the executable file that is used to compare to the "writer" and "write_obj" examples output.

For "writer", this program is compiled and linked with mingw32:
    gcc -s answer.c -o answer_mingw32.exe -nodefaultlibs -nolibc -nostartfiles

For "write_obj", this program is compiled with mingw32:
    gcc -c -s -fno-asynchronous-unwind-tables answer.c -o answer_mingw32.o
    gcc -s answer_mingw32.o -o answer_mingw32.exe

Running the executable:
    answer_mingw32.exe
    echo %ERRORLEVEL%
*/

#include <stdio.h>

int main()
{
    return 42;
}
