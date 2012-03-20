#include <windows.h>
#include <stdio.h>

extern int main(int, char**);

int WINAPI
WinMain(HINSTANCE current, HINSTANCE prev, LPSTR cmdline, int showcmd)
{
    return main(0, NULL);
}
