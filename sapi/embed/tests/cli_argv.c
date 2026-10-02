#include "php.h"
#include "sapi/cli/cli.h"

int main(int argc, char *argv[])
{
	char *php_argv[] = {
		"embedded-php", "-n", "-d", "default_charset=UTF-8", "-r",
		"echo json_encode([$argc, array_map('bin2hex', array_slice($argv, 1))]), PHP_EOL; exit(23);",
		"--", "caf\xc3\xa9", "argument with spaces", "", "\xf0\x9f\x98\x80", NULL
	};
	if (argc > 1 && strcmp(argv[1], "default-charset") == 0) {
		php_argv[3] = "default_charset=Windows-1252";
		php_argv[10] = "\xe2\x82\xac";
	} else if (argc > 1 && strcmp(argv[1], "internal-encoding") == 0) {
		php_argv[3] = "internal_encoding=Windows-1252";
		php_argv[10] = "\xe2\x82\xac";
	}
	if (argc > 2 && strcmp(argv[2], "file") == 0) {
		php_argv[4] = "-f";
		php_argv[5] = "cli_argv_caf\xc3\xa9.php";
	} else if (argc > 2 && strcmp(argv[2], "invalid-utf8") == 0) {
		/* Latin-1 cafe with an accented e, which is not valid UTF-8. */
		php_argv[7] = "caf\xe9";
	}
	return do_php_cli((int) (sizeof(php_argv) / sizeof(*php_argv)) - 1, php_argv);
}
