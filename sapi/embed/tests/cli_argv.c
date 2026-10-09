#include "php.h"
#include "sapi/cli/cli.h"
#include "win32/codepage.h"

int wmain(int argc, wchar_t *argv[])
{
	if (argc < 2 || wcscmp(argv[1], L"--") != 0) {
		return 1;
	}
	argc--;
	char **args = calloc((size_t) argc + 1, sizeof(char *));
	if (!args) {
		return 1;
	}
	/* Omit the host's separator so the supplied argv differs from the process argv. */
	for (int i = 0; i < argc; i++) {
		args[i] = php_win32_cp_w_to_utf8(argv[i ? i + 1 : 0]);
		if (!args[i]) {
			PHP_WIN32_CP_FREE_ARRAY(args, argc);
			return 1;
		}
	}
	if (getenv("PHP_EMBED_TEST_INVALID_UTF8") && argc > 1 && args[argc - 1][0]) {
		args[argc - 1][0] = '\xff';
	}
	int status = do_php_cli(argc, args);
	PHP_WIN32_CP_FREE_ARRAY(args, argc);
	return status;
}
