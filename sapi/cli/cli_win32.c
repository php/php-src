#define PHP_CLI_WIN32_NO_CONSOLE 1
#include "php_cli.c"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	return do_php_cli(__argc, __argv);
}
