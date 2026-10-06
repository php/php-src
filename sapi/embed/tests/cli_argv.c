#include "php.h"
#include "sapi/cli/cli.h"

int main(int argc, char *argv[])
{
	char *prepend_argv[] = {
		"embedded-php", "-n", "-d", "auto_prepend_file=cli_argv_caf\xc3\xa9.php",
		"-d", "default_charset=Windows-1252", "-f", "cli_argv_main.php", NULL
	};
	char *prepend_ini_argv[] = {
		"embedded-php", "-c", "cli_argv_test.ini", "-d",
		"auto_prepend_file=cli_argv_caf\xc3\xa9.php", "-f", "cli_argv_main.php", NULL
	};
	char *ini_expansion_argv[] = {
		"embedded-php", "-n", "-d", "prefix=cli_ini_caf\xc3\xa9",
		"-d", "caf\xc3\xa9=cli_ini_caf\xc3\xa9", "-d", "auto_prepend_file=${prefix}.php",
		"-d", "include_path=${caf\xc3\xa9}", "-d", "user_agent=${CLI_INI_UTF8}",
		"-d", "alias=Windows-1252", "-d", "default_charset=${alias}", "-f", "cli_ini_utf8.php", NULL
	};
	char *ini_file_expansion_argv[] = {
		"embedded-php", "-c", "cli_ini_utf8.ini", "-d", "auto_prepend_file=${prefix}${suffix}.php",
		"-d", "include_path=cli_ini_caf\xc3\xa9${suffix}", "-d", "user_agent=${caf\xc3\xa9}",
		"-f", "cli_ini_utf8.php", NULL
	};
	char *ini_cp932_argv[] = {
		"embedded-php", "-n", "-d", "auto_prepend_file=cli_ini_cp932.php",
		"-d", "include_path=\xe3\x82\xbd\\main.php", "-d", "token=tail",
		"-d", "user_agent=\xe3\x82\xbd${token}", "-d", "default_charset=CP932", "-f", "cli_ini_utf8.php", NULL
	};
	char *ini_section_argv[] = {
		"embedded-php", "-c", "cli_ini_utf8.ini", "-d", "before=1\n[PATH=collision]\ndefault_charset=UTF-8",
		"-d", "auto_prepend_file=${prefix}.php", "-d", "include_path=${prefix}",
		"-d", "user_agent=${CLI_INI_UTF8}", "-f", "cli_ini_utf8.php", NULL
	};
	char *php_argv[] = {
		"embedded-php", "-n", "-d", "default_charset=UTF-8", "-r",
		"echo json_encode([$argc, array_map('bin2hex', array_slice($argv, 1))]), PHP_EOL; echo sapi_windows_cp_get(), PHP_EOL; exit(23);",
		"--", "caf\xc3\xa9", "argument with spaces", "", "\xf0\x9f\x98\x80", NULL
	};
	if (argc > 1 && strcmp(argv[1], "prepend") == 0) {
		return do_php_cli((int) (sizeof(prepend_argv) / sizeof(*prepend_argv)) - 1, prepend_argv);
	}
	if (argc > 1 && strcmp(argv[1], "prepend-ini") == 0) {
		return do_php_cli((int) (sizeof(prepend_ini_argv) / sizeof(*prepend_ini_argv)) - 1, prepend_ini_argv);
	}
	if (argc > 1 && strcmp(argv[1], "ini-expansion") == 0) {
		return do_php_cli((int) (sizeof(ini_expansion_argv) / sizeof(*ini_expansion_argv)) - 1, ini_expansion_argv);
	}
	if (argc > 1 && strcmp(argv[1], "ini-file-expansion") == 0) {
		return do_php_cli((int) (sizeof(ini_file_expansion_argv) / sizeof(*ini_file_expansion_argv)) - 1, ini_file_expansion_argv);
	}
	if (argc > 1 && strcmp(argv[1], "ini-cp932") == 0) {
		return do_php_cli((int) (sizeof(ini_cp932_argv) / sizeof(*ini_cp932_argv)) - 1, ini_cp932_argv);
	}
	if (argc > 1 && strcmp(argv[1], "ini-section") == 0) {
		return do_php_cli((int) (sizeof(ini_section_argv) / sizeof(*ini_section_argv)) - 1, ini_section_argv);
	}
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
